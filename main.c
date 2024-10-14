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

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bcm2835_gpio.h"
#include "pidp11.h"

#include "sim_frontpanel.h"

static uint16_t reg_pc = 0;
static uint16_t reg_dr = 0;

void display_callback(PANEL *panel, unsigned long long simulation_time,
                      void *context) {
  pidp11_t *pidp11 = (pidp11_t *)context;
  pidp11->data = reg_dr;
}

int main(int argc, char **argv) {
  bcm2835_gpio_t gpio = {0};
  pidp11_t pidp11 = {0};

  if (argc < 3) {
    fprintf(stderr, "Usage: %s {sim_path} {ini_path}\n", argv[0]);
    return -1;
  }

  PANEL *panel;
  if (!(panel = sim_panel_start_simulator(argv[1], argv[2], 0))) {
    fprintf(stderr, "Could not start simulator.  %s\n", sim_panel_get_error());
    return -1;
  }

  int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  size_t mem_length = 0x100;
  volatile uint32_t *base = (uint32_t *)mmap(
      NULL, mem_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  close(mem_fd);

  bcm2835_gpio_init(&gpio, base);
  pidp11_init(&pidp11, &gpio);

  // 1000 005003 CLR R3
  // 1002 010327 MOV R3, #177570
  // 1004 177570
  // 1006 010227 MOV R2, #177777
  // 1010 177777
  // 1012 005302 DEC R2
  // 1014 001376 BNE 1012
  // 1016 005203 INC R3
  // 1020 000770 BR 1002
  // 1022 000000 HALT

  // Enter in simple program to increment the display register.
  uint16_t addr = 01000;
  sim_panel_gen_deposit(panel, "PC", 2, &addr);
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "CLR R3");
  addr += 2;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "MOV R3, @#177570");
  addr += 4;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "MOV #177777, R2");
  addr += 4;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "DEC R2");
  addr += 2;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "BNE 1012");
  addr += 2;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "INC R3");
  addr += 2;
  sim_panel_mem_deposit_instruction(panel, 2, &addr, "BR 1002");
  addr += 2;

  uint16_t value;

  for (addr = 01000; addr < 01024; addr += 2) {
    sim_panel_mem_examine(panel, 2, &addr, 2, &value);
    printf("%06o: %06o\n", addr, value);
  }

  sim_panel_add_register(panel, "PC", NULL, sizeof(reg_pc), &reg_pc);
  sim_panel_add_register(panel, "DR", NULL, sizeof(reg_dr), &reg_dr);
  sim_panel_set_display_callback_interval(panel, display_callback, &pidp11,
                                          10000); // 10ms

  if (sim_panel_exec_start(panel)) {
    fprintf(stderr, "Error starting simulator: %s", sim_panel_get_error());
  }

  printf("Sleeping for 5 seconds.\n");
  usleep(5000000);

  sim_panel_destroy(panel);
  pidp11_close(&pidp11);
  bcm2835_gpio_close(&gpio);

  munmap((void *)base, mem_length);
  return 0;
}
