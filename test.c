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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bcm2835_gpio.h"

static pin_t col_pins[] = {26, 27, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static pin_t led_pins[] = {20, 21, 22, 23, 24, 25};
static pin_t row_pins[] = {16, 17, 18};
static int n_led_pins = sizeof led_pins / sizeof led_pins[0];
static int n_col_pins = sizeof col_pins / sizeof col_pins[0];
static int n_row_pins = sizeof row_pins / sizeof row_pins[0];

typedef enum _addr_mode_t {
  ADDR_USER_D,
  ADDR_SUPER_D,
  ADDR_KERNEL_D,
  ADDR_CONS_PHY,
  ADDR_PROG_PHY,
  ADDR_KERNEL_I,
  ADDR_SUPER_I,
  ADDR_USER_I
} addr_mode_t;

typedef enum _data_mode_t {
  DATA_PATHS,
  DATA_BUS_REG,
  DATA_MU_A_FPP_CPU,
  DATA_DISP_REG
} data_mode_t;

typedef enum _addressing_length_t {
  ADDRESS_16,
  ADDRESS_18,
  ADDRESS_22
} addressing_length_t;

typedef enum _run_state_t {
  RUN_STATE_RUN,
  RUN_STATE_PAUSE,
  RUN_STATE_MASTER
} run_state_t;

typedef enum _run_level_t {
  RUN_LEVEL_USER,
  RUN_LEVEL_SUPER,
  RUN_LEVEL_KERNEL
} run_level_t;

struct pidp11 {
  uint32_t address;
  uint16_t data;
  addr_mode_t addr_mode;
  data_mode_t data_mode;
  addressing_length_t addressing_length;
  bool parity_high;
  bool parity_low;

  bool parity_err;
  bool address_err;
  run_state_t run_state;
  run_level_t run_level;
  bool data_ref;

  uint32_t switch_reg;
  bool switch_test;
  bool switch_load_add;
  bool switch_exam;
  bool switch_dep;
  bool switch_cont;
  bool switch_ena_halt;
  bool switch_sing_inst;
  bool switch_start;
  bool switch_addr;
  bool switch_addr_rot1;
  bool switch_addr_rot2;
  bool switch_data;
  bool switch_data_rot1;
  bool switch_data_rot2;
};

int main(int argc, char **argv) {
  struct pidp11 pidp11 = {0};

  int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  size_t mem_length = 0x100;
  volatile uint32_t *base = (uint32_t *)mmap(
      NULL, mem_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  close(mem_fd);

  bcm2835_gpio_t gpio;

  printf("Initializing bcm2835 GPIO\n");
  bcm2835_gpio_init(&gpio, base);

  // PiDP11: initialize
  printf("Initializing PiDP11\n");
  bcm2835_gpio_set_function_pins(&gpio, led_pins, n_led_pins, OUT);
  bcm2835_gpio_set_function_pins(&gpio, col_pins, n_col_pins, OUT);
  bcm2835_gpio_set_function_pins(&gpio, row_pins, n_row_pins, OUT);

  bcm2835_gpio_set_pins(&gpio, led_pins, n_led_pins, 0);
  bcm2835_gpio_set_pins(&gpio, col_pins, n_col_pins, 1);
  bcm2835_gpio_set_pins(&gpio, row_pins, n_row_pins, 1);

  printf("Press the HALT switch to quit.\n");
  while (!pidp11.switch_ena_halt) {
    bcm2835_gpio_set_function_pins(&gpio, col_pins, n_col_pins, OUT);
    // Go through each row and column in turn.
    for (int i = 0; i < n_led_pins; i++) {
      pin_t led_pin[] = {led_pins[i]};
      if (pidp11.switch_test) {
        bcm2835_gpio_set_pins(&gpio, col_pins, n_col_pins, 0);
      } else {
        switch (i) {
        case 0:
          for (int j = 0; j < n_col_pins; j++) {
            bcm2835_gpio_set_pins(&gpio, &col_pins[j], 1,
                                  !(pidp11.address & (1 << j)));
          }
          break;
        case 1:
          for (int j = 0; j < 10; j++) {
            bcm2835_gpio_set_pins(&gpio, &col_pins[j], 1,
                                  !(pidp11.address & (1 << (j + 12))));
          }
          break;
        case 2:
          // set_or_clear_pin(&gpio, col_pins[0], pidp11.addressing_length ==
          // ADDRESS_22);
          bcm2835_gpio_set_pins(&gpio, &col_pins[0], 1,
                                pidp11.addressing_length != ADDRESS_22);
          bcm2835_gpio_set_pins(&gpio, &col_pins[1], 1,
                                pidp11.addressing_length != ADDRESS_18);
          bcm2835_gpio_set_pins(&gpio, &col_pins[2], 1,
                                pidp11.addressing_length != ADDRESS_16);
          bcm2835_gpio_set_pins(&gpio, &col_pins[3], 1, !pidp11.data_ref);
          bcm2835_gpio_set_pins(&gpio, &col_pins[4], 1,
                                pidp11.run_level != RUN_LEVEL_KERNEL);
          bcm2835_gpio_set_pins(&gpio, &col_pins[5], 1,
                                pidp11.run_level != RUN_LEVEL_SUPER);
          bcm2835_gpio_set_pins(&gpio, &col_pins[6], 1,
                                pidp11.run_level != RUN_LEVEL_USER);
          bcm2835_gpio_set_pins(&gpio, &col_pins[7], 1,
                                pidp11.run_state != RUN_STATE_MASTER);
          bcm2835_gpio_set_pins(&gpio, &col_pins[8], 1,
                                pidp11.run_state != RUN_STATE_PAUSE);
          bcm2835_gpio_set_pins(&gpio, &col_pins[9], 1,
                                pidp11.run_state != RUN_STATE_RUN);
          bcm2835_gpio_set_pins(&gpio, &col_pins[10], 1, !pidp11.address_err);
          bcm2835_gpio_set_pins(&gpio, &col_pins[11], 1, !pidp11.parity_err);
          break;
        case 3:
          for (int j = 0; j < n_col_pins; j++) {
            bcm2835_gpio_set_pins(&gpio, &col_pins[j], 1,
                                  !(pidp11.data & (1 << j)));
          }
          break;
        case 4:
          for (int j = 0; j < 4; j++) {
            bcm2835_gpio_set_pins(&gpio, &col_pins[j], 1,
                                  !(pidp11.data & (1 << (j + 12))));
          }
          bcm2835_gpio_set_pins(&gpio, &col_pins[4], 1, !pidp11.parity_low);
          bcm2835_gpio_set_pins(&gpio, &col_pins[5], 1, !pidp11.parity_high);
          bcm2835_gpio_set_pins(&gpio, &col_pins[6], 1,
                                pidp11.addr_mode != ADDR_USER_D);
          bcm2835_gpio_set_pins(&gpio, &col_pins[7], 1,
                                pidp11.addr_mode != ADDR_SUPER_D);
          bcm2835_gpio_set_pins(&gpio, &col_pins[8], 1,
                                pidp11.addr_mode != ADDR_KERNEL_D);
          bcm2835_gpio_set_pins(&gpio, &col_pins[9], 1,
                                pidp11.addr_mode != ADDR_CONS_PHY);
          bcm2835_gpio_set_pins(&gpio, &col_pins[10], 1,
                                pidp11.data_mode != DATA_PATHS);
          bcm2835_gpio_set_pins(&gpio, &col_pins[11], 1,
                                pidp11.data_mode != DATA_BUS_REG);
          break;
        case 5:
          bcm2835_gpio_set_pins(&gpio, &col_pins[6], 1,
                                pidp11.addr_mode != ADDR_USER_I);
          bcm2835_gpio_set_pins(&gpio, &col_pins[7], 1,
                                pidp11.addr_mode != ADDR_SUPER_I);
          bcm2835_gpio_set_pins(&gpio, &col_pins[8], 1,
                                pidp11.addr_mode != ADDR_KERNEL_I);
          bcm2835_gpio_set_pins(&gpio, &col_pins[9], 1,
                                pidp11.addr_mode != ADDR_PROG_PHY);
          bcm2835_gpio_set_pins(&gpio, &col_pins[10], 1,
                                pidp11.data_mode != DATA_MU_A_FPP_CPU);
          bcm2835_gpio_set_pins(&gpio, &col_pins[11], 1,
                                pidp11.data_mode != DATA_DISP_REG);
          break;
#ifdef DEBUG
        default:
          printf("DANGER: There are only six led rows.");
#endif
        }
      }

      bcm2835_gpio_set_pins(&gpio, led_pin, 1, 1);
      // Let the LED stay on for a short amount of time
      // Refresh 6 led rows every 60 seconds.  Move this elsewhere.
      usleep((1000000 / 60) / 6);
      bcm2835_gpio_set_pins(&gpio, led_pin, 1, 0);
    }

    bcm2835_gpio_set_pins(&gpio, row_pins, n_row_pins, 1);
    bcm2835_gpio_set_pull_pins(&gpio, col_pins, n_col_pins, UP);
    bcm2835_gpio_set_function_pins(&gpio, col_pins, n_col_pins, IN);
    for (int i = 0; i < n_row_pins; i++) {
      pin_t row_pin[] = {row_pins[i]};
      bcm2835_gpio_set_pins(&gpio, row_pin, 1, 0);
      usleep(10);
      uint64_t value;
      bcm2835_gpio_get_bits(&gpio, &value);
      switch (i) {
      case 0:
        pidp11.switch_reg &= 0xfffffffffffff000;
        pidp11.switch_reg |=
            ((~value & 0x00003ff0) >> 2) | ((~value & 0x0c000000) >> 26);
        break;
      case 1:
        pidp11.switch_reg &= 0xffffffffffc00fff;
        pidp11.switch_reg |=
            ((~value & 0x00000ff0) << 10) | ((~value & 0x0c000000) >> 14);
        pidp11.switch_addr = (value & (1 << 12)) == 0;
        pidp11.switch_data = (value & (1 << 13)) == 0;
        break;
      case 2:
        pidp11.switch_test = (value & (1 << 26)) != 0; // NOTE: inverted.
        pidp11.switch_load_add = (value & (1 << 27)) == 0;
        pidp11.switch_exam = (value & (1 << 4)) == 0;
        pidp11.switch_dep = (value & (1 << 5)) == 0;
        pidp11.switch_cont = (value & (1 << 6)) == 0;
        pidp11.switch_ena_halt = (value & (1 << 7)) == 0;
        pidp11.switch_sing_inst = (value & (1 << 8)) == 0;
        pidp11.switch_start = (value & (1 << 9)) == 0;
        pidp11.switch_addr_rot1 = (value & (1 << 10)) == 0;
        pidp11.switch_addr_rot2 = (value & (1 << 11)) == 0;
        pidp11.switch_data_rot1 = (value & (1 << 12)) == 0;
        pidp11.switch_data_rot2 = (value & (1 << 13)) == 0;
        break;
#ifdef DEBUG
      default:
        printf("DANGER: There are only three rows.");
#endif
      }
      bcm2835_gpio_set_pins(&gpio, row_pin, 1, 1);
    }
    bcm2835_gpio_set_pull_pins(&gpio, col_pins, n_col_pins, OFF);

    // Set the address to the switch register.
    pidp11.address = pidp11.switch_reg;

    // Increment the data value.
    pidp11.data += 1;

    // Let the knobs spin the address and data display leds.
    // This needs to be debounced.
    if (!pidp11.switch_addr_rot1 && pidp11.switch_addr_rot2) {
      pidp11.addr_mode =
          pidp11.addr_mode == ADDR_USER_I ? ADDR_USER_D : pidp11.addr_mode + 1;
    }
    if (pidp11.switch_addr_rot1 && !pidp11.switch_addr_rot2) {
      pidp11.addr_mode =
          pidp11.addr_mode == ADDR_USER_D ? ADDR_USER_I : pidp11.addr_mode - 1;
    }
    if (!pidp11.switch_data_rot1 && pidp11.switch_data_rot2) {
      pidp11.data_mode =
          pidp11.data_mode == DATA_DISP_REG ? DATA_PATHS : pidp11.data_mode + 1;
    }
    if (pidp11.switch_data_rot1 && !pidp11.switch_data_rot2) {
      pidp11.data_mode =
          pidp11.data_mode == DATA_PATHS ? DATA_DISP_REG : pidp11.data_mode - 1;
    }
  }

  printf("HALT detected.\n");
  printf("Setting GPIOs to IN.\n");
  bcm2835_gpio_set_function_pins(&gpio, led_pins, n_led_pins, IN);
  bcm2835_gpio_set_function_pins(&gpio, col_pins, n_col_pins, IN);
  bcm2835_gpio_set_function_pins(&gpio, row_pins, n_row_pins, IN);

  printf("Restoring default pullup/down state.\n");
  pin_t default_up[] = {4, 5, 6, 7, 8};
  pin_t default_down[] = {26, 27, 9,  10, 11, 12, 13, 20,
                          21, 22, 23, 24, 25, 16, 17, 18};
  bcm2835_gpio_set_pull_pins(&gpio, default_up,
                             sizeof default_up / sizeof default_up[0], UP);
  bcm2835_gpio_set_pull_pins(
      &gpio, default_down, sizeof default_down / sizeof default_down[0], DOWN);

  bcm2835_gpio_close(&gpio);
  munmap((void *)base, mem_length);

  return 0;
}
