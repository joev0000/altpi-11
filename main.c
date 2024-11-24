/*
 * Copyright (c) 2024 Joseph Vigneau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bcm2835_gpio.h"
#include "pidp11.h"

// sim_frontpanel.c: suppress compiler warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include "sim_frontpanel.c"
#pragma GCC diagnostic pop

static uint16_t reg_pc = 0;
static uint16_t reg_dr = 0;
static uint16_t reg_r0 = 0;

static int interrupt = 0;

enum step_t { None, Exam, Dep };

void sigint_handler(int signum) { interrupt = 1; }

void update_display(pidp11_t *pidp11) {
  switch (pidp11->data_mode) {
  case DATA_PATHS:
    pidp11->data = reg_r0;
    break;
  case DATA_DISP_REG:
    pidp11->data = reg_dr;
    break;
  default:
    pidp11->data = reg_r0; // TODO: support the other modes.
  }

  switch (pidp11->addr_mode) {
  case ADDR_PROG_PHY:
    pidp11->address = reg_pc;
  default:
    pidp11->address = reg_pc; // TODO: support the other modes.
  }
}

void display_callback(PANEL *panel, unsigned long long simulation_time,
                      void *context) {
  pidp11_t *pidp11 = (pidp11_t *)context;
  if (panel->State == Run) {
    update_display(pidp11);
  }
}

/**
 * Check for a rising edge by comparing the current value with
 * the previous value. If the previous value was zero and the current
 * value is non-zero, an edge was detected.
 * Set the previous value to the current value for the next time
 * this is called.
 */
int rising_edge(int current_value, int *previous_value) {
  int edge_detected = 0;
  if (current_value && !*previous_value) {
    edge_detected = 1;
  }
  *previous_value = current_value;
  return edge_detected;
}

int main(int argc, char **argv) {
  gpio_t gpio = {0};
  bcm2835_gpio_ext_t ext = {0};
  pidp11_t pidp11 = {0};

  if (argc < 3) {
    fprintf(stderr, "Usage: %s {sim_path} {ini_path}\n", argv[0]);
    return -1;
  }

  struct sigaction sigint_action = {.sa_handler = sigint_handler,
                                    .sa_flags = 0};
  if (sigaction(SIGINT, &sigint_action, NULL)) {
    fprintf(stderr, "Could not install SIGINT signal handler.\n");
    return -1;
  }
  if (sigaction(SIGTERM, &sigint_action, NULL)) {
    fprintf(stderr, "Could not install SIGTERM signal handler.\n");
    return -1;
  }

  printf("Starting simulator.\n");
  // NOTE: sim_panel_start_simulator blocks until something connects to the
  // console port. We could spawn a thread and wait, and open a connection here.
#ifdef DEBUG
  PANEL *panel =
      sim_panel_start_simulator_debug(argv[1], argv[2], 0, "pidp11-debug.log");
#else
  PANEL *panel = sim_panel_start_simulator(argv[1], argv[2], 0);
#endif

  if (!panel) {
    fprintf(stderr, "Could not start simulator.  %s\n", sim_panel_get_error());
    return -1;
  }
#ifdef DEBUG
  sim_panel_set_debug_mode(panel, DBG_REQ | DBG_RSP | DBG_APP);
#endif

  printf("Simulator started.\n");

  int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  size_t mem_length = 0x100;
  volatile uint32_t *base = (uint32_t *)mmap(
      NULL, mem_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  close(mem_fd);

  ext.base = base;
  bcm2835_gpio_init(&gpio, &ext);
  pidp11_init(&pidp11, &gpio);

  sim_panel_add_register(panel, "PC", NULL, sizeof(reg_pc), &reg_pc);
  sim_panel_add_register(panel, "R0", NULL, sizeof(reg_pc), &reg_r0);
  sim_panel_add_register(panel, "DR", NULL, sizeof(reg_dr), &reg_dr);
  sim_panel_set_display_callback_interval(panel, display_callback, &pidp11,
                                          1000000 / 60); // 60Hz

  int prev_load_add = 0;
  int prev_exam = 0;
  int prev_dep = 0;
  int prev_cont = 0;
  int prev_start = 0;
  enum step_t step = None;
  while (!interrupt) {
    switch (panel->State) {
    case Run:
      if (pidp11.switch_ena_halt) {
        printf("Halt (PC: %o)\n", reg_pc);
        sim_panel_exec_halt(panel);
        update_display(&pidp11);
      }
      break;
    case Halt:
      if (rising_edge(pidp11.switch_load_add, &prev_load_add)) {
        step = None;
        pidp11.address = pidp11.switch_reg;
        printf("Load address %o\n", pidp11.address);
      }

      if (rising_edge(pidp11.switch_exam, &prev_exam)) {
        if (step == Exam) {
          // TODO: check if we're at a GR address
          pidp11.address += 2;
        }
        step = Exam;
        uint16_t value;
        sim_panel_mem_examine(panel, sizeof(pidp11.address), &pidp11.address,
                              sizeof(value), &value);
        printf("Examine %o: %06o\n", pidp11.address, value);
        pidp11.data = value; // TODO: if data select switch is DATA PATHS
      }
      if (rising_edge(pidp11.switch_dep, &prev_dep)) {
        if (step == Dep) {
          // TODO: check if we're at a GR address
          pidp11.address += 2;
        }
        step = Dep;
        uint16_t value = pidp11.switch_reg;
        printf("Deposit %o: %06o\n", pidp11.address, value);
        sim_panel_mem_deposit(panel, sizeof(pidp11.address), &pidp11.address,
                              sizeof(value), &value);
        pidp11.data = value; // TODO: if the data select switch is DATA PATHS
      }

      if (rising_edge(pidp11.switch_cont, &prev_cont)) {
        step = None;
        if (pidp11.switch_ena_halt) {
          printf("Stepping. (PC: %o)\n", reg_pc);
          sim_panel_exec_step(panel);
          update_display(&pidp11);
        } else {
          printf("Running. (PC: %o)\n", reg_pc);
          sim_panel_exec_run(panel);
        }
      }

      if (rising_edge(pidp11.switch_start, &prev_start)) {
        step = None;
        if (pidp11.switch_ena_halt) {
          printf("Starting.\n");
          sim_panel_exec_start(panel);
        } else {
          printf("Starting at %o\n", pidp11.address);
          sim_panel_gen_deposit(panel, "PC", sizeof(pidp11.address),
                                &pidp11.address);
          sim_panel_exec_start(panel);
        }
      }

      break;
    default:
      interrupt = 1;
    }
    usleep(100000);
  }

  printf("Shutting down.\n");
#ifdef DEBUG
  sim_panel_flush_debug(panel);
#endif
  sim_panel_destroy(panel);
  pidp11_close(&pidp11);
  gpio_close(&gpio);

  munmap((void *)base, mem_length);
  return 0;
}
