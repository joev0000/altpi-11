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

#include <stdio.h>
#include <unistd.h>

#include "bcm2835_gpio.h"
#include "pidp11.h"

static pin_t col_pins[] = {26, 27, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static pin_t led_pins[] = {20, 21, 22, 23, 24, 25};
static pin_t row_pins[] = {16, 17, 18};
static int n_led_pins = sizeof led_pins / sizeof led_pins[0];
static int n_col_pins = sizeof col_pins / sizeof col_pins[0];
static int n_row_pins = sizeof row_pins / sizeof row_pins[0];

void pidp11_cleanup(void *context) {
  pidp11_t *pidp11 = (pidp11_t *)context;
  bcm2835_gpio_t *gpio = pidp11->gpio;

  printf("Setting GPIOs to IN.\n");
  bcm2835_gpio_set_function_pins(gpio, led_pins, n_led_pins, IN);
  bcm2835_gpio_set_function_pins(gpio, col_pins, n_col_pins, IN);
  bcm2835_gpio_set_function_pins(gpio, row_pins, n_row_pins, IN);

  printf("Restoring default pullup/down state.\n");
  pin_t default_up[] = {4, 5, 6, 7, 8};
  pin_t default_down[] = {26, 27, 9,  10, 11, 12, 13, 20,
                          21, 22, 23, 24, 25, 16, 17, 18};
  bcm2835_gpio_set_pull_pins(gpio, default_up,
                             sizeof default_up / sizeof default_up[0], UP);
  bcm2835_gpio_set_pull_pins(
      gpio, default_down, sizeof default_down / sizeof default_down[0], DOWN);
}

void *pidp11_update(void *context) {
  pidp11_t *pidp11 = (pidp11_t *)context;
  bcm2835_gpio_t *gpio = pidp11->gpio;

  pthread_cleanup_push(pidp11_cleanup, pidp11);
  while (1) {
    bcm2835_gpio_set_function_pins(gpio, col_pins, n_col_pins, OUT);
    for (int i = 0; i < n_led_pins; i++) {
      if (pidp11->switch_test) {
        bcm2835_gpio_set_pins(gpio, col_pins, n_col_pins, 0);
      } else {
        switch (i) {
        case 0:
          for (int j = 0; j < n_col_pins; j++) {
            bcm2835_gpio_set_pins(gpio, &col_pins[j], 1,
                                  !(pidp11->address & (1 << j)));
          }
          break;
        case 1:
          for (int j = 0; j < 10; j++) {
            bcm2835_gpio_set_pins(gpio, &col_pins[j], 1,
                                  !(pidp11->address & (1 << (j + 12))));
          }
          break;
        case 2:
          bcm2835_gpio_set_pins(gpio, &col_pins[0], 1,
                                pidp11->addressing_length != ADDRESS_22);
          bcm2835_gpio_set_pins(gpio, &col_pins[1], 1,
                                pidp11->addressing_length != ADDRESS_18);
          bcm2835_gpio_set_pins(gpio, &col_pins[2], 1,
                                pidp11->addressing_length != ADDRESS_16);
          bcm2835_gpio_set_pins(gpio, &col_pins[3], 1, !pidp11->data_ref);
          bcm2835_gpio_set_pins(gpio, &col_pins[4], 1,
                                pidp11->run_level != RUN_LEVEL_KERNEL);
          bcm2835_gpio_set_pins(gpio, &col_pins[5], 1,
                                pidp11->run_level != RUN_LEVEL_SUPER);
          bcm2835_gpio_set_pins(gpio, &col_pins[6], 1,
                                pidp11->run_level != RUN_LEVEL_USER);
          bcm2835_gpio_set_pins(gpio, &col_pins[7], 1,
                                pidp11->run_state != RUN_STATE_MASTER);
          bcm2835_gpio_set_pins(gpio, &col_pins[8], 1,
                                pidp11->run_state != RUN_STATE_PAUSE);
          bcm2835_gpio_set_pins(gpio, &col_pins[9], 1,
                                pidp11->run_state != RUN_STATE_RUN);
          bcm2835_gpio_set_pins(gpio, &col_pins[10], 1, !pidp11->address_err);
          bcm2835_gpio_set_pins(gpio, &col_pins[11], 1, !pidp11->parity_err);
          break;
        case 3:
          for (int j = 0; j < n_col_pins; j++) {
            bcm2835_gpio_set_pins(gpio, &col_pins[j], 1,
                                  !(pidp11->data & (1 << j)));
          }
          break;
        case 4:
          for (int j = 0; j < 4; j++) {
            bcm2835_gpio_set_pins(gpio, &col_pins[j], 1,
                                  !(pidp11->data & (1 << (j + 12))));
          }
          bcm2835_gpio_set_pins(gpio, &col_pins[4], 1, !pidp11->parity_low);
          bcm2835_gpio_set_pins(gpio, &col_pins[5], 1, !pidp11->parity_high);
          bcm2835_gpio_set_pins(gpio, &col_pins[6], 1,
                                pidp11->addr_mode != ADDR_USER_D);
          bcm2835_gpio_set_pins(gpio, &col_pins[7], 1,
                                pidp11->addr_mode != ADDR_SUPER_D);
          bcm2835_gpio_set_pins(gpio, &col_pins[8], 1,
                                pidp11->addr_mode != ADDR_KERNEL_D);
          bcm2835_gpio_set_pins(gpio, &col_pins[9], 1,
                                pidp11->addr_mode != ADDR_CONS_PHY);
          bcm2835_gpio_set_pins(gpio, &col_pins[10], 1,
                                pidp11->data_mode != DATA_PATHS);
          bcm2835_gpio_set_pins(gpio, &col_pins[11], 1,
                                pidp11->data_mode != DATA_BUS_REG);
          break;
        case 5:
          bcm2835_gpio_set_pins(gpio, &col_pins[6], 1,
                                pidp11->addr_mode != ADDR_USER_I);
          bcm2835_gpio_set_pins(gpio, &col_pins[7], 1,
                                pidp11->addr_mode != ADDR_SUPER_I);
          bcm2835_gpio_set_pins(gpio, &col_pins[8], 1,
                                pidp11->addr_mode != ADDR_KERNEL_I);
          bcm2835_gpio_set_pins(gpio, &col_pins[9], 1,
                                pidp11->addr_mode != ADDR_PROG_PHY);
          bcm2835_gpio_set_pins(gpio, &col_pins[10], 1,
                                pidp11->data_mode != DATA_MU_A_FPP_CPU);
          bcm2835_gpio_set_pins(gpio, &col_pins[11], 1,
                                pidp11->data_mode != DATA_DISP_REG);
          break;
#ifdef DEBUG
        default:
          printf("DANGER: There are only six led rows.");
#endif
        }
      }
      bcm2835_gpio_set_pins(gpio, &led_pins[i], 1, 1);

      usleep((100000 / 60) / 6);
      bcm2835_gpio_set_pins(gpio, &led_pins[i], 1, 0);
    }
    // Capture switch state
    bcm2835_gpio_set_pins(gpio, row_pins, n_row_pins, 1);
    bcm2835_gpio_set_pull_pins(gpio, col_pins, n_col_pins, UP);
    bcm2835_gpio_set_function_pins(gpio, col_pins, n_col_pins, IN);
    for (int i = 0; i < n_row_pins; i++) {
      pin_t row_pin[] = {row_pins[i]};
      bcm2835_gpio_set_pins(gpio, row_pin, 1, 0);
      usleep(10);
      uint64_t value;
      bcm2835_gpio_get_bits(gpio, &value);
      switch (i) {
      case 0:
        pidp11->switch_reg &= 0xfffffffffffff000;
        pidp11->switch_reg |=
            ((~value & 0x00003ff0) >> 2) | ((~value & 0x0c000000) >> 26);
        break;
      case 1:
        pidp11->switch_reg &= 0xffffffffffc00fff;
        pidp11->switch_reg |=
            ((~value & 0x00000ff0) << 10) | ((~value & 0x0c000000) >> 14);
        pidp11->switch_addr = (value & (1 << 12)) == 0;
        pidp11->switch_data = (value & (1 << 13)) == 0;
        break;
      case 2:
        pidp11->switch_test = (value & (1 << 26)) != 0; // NOTE: inverted.
        pidp11->switch_load_add = (value & (1 << 27)) == 0;
        pidp11->switch_exam = (value & (1 << 4)) == 0;
        pidp11->switch_dep = (value & (1 << 5)) == 0;
        pidp11->switch_cont = (value & (1 << 6)) == 0;
        pidp11->switch_ena_halt = (value & (1 << 7)) == 0;
        pidp11->switch_sing_inst = (value & (1 << 8)) == 0;
        pidp11->switch_start = (value & (1 << 9)) == 0;
        pidp11->switch_addr_rot1 = (value & (1 << 10)) == 0;
        pidp11->switch_addr_rot2 = (value & (1 << 11)) == 0;
        pidp11->switch_data_rot1 = (value & (1 << 12)) == 0;
        pidp11->switch_data_rot2 = (value & (1 << 13)) == 0;
        break;
#ifdef DEBUG
      default:
        printf("DANGER: There are only three rows.");
#endif
      }
      bcm2835_gpio_set_pins(gpio, row_pin, 1, 1);
    }
    bcm2835_gpio_set_pull_pins(gpio, col_pins, n_col_pins, OFF);

    // Do stuff with the switch values.
    // if switch_dep set_mem(addr, data), etc.

    pthread_testcancel();
  }
  pthread_cleanup_pop(1);
  return NULL;
}

int pidp11_init(pidp11_t *pidp11, bcm2835_gpio_t *gpio) {
  pidp11->gpio = gpio;

  bcm2835_gpio_set_function_pins(gpio, led_pins, n_led_pins, OUT);
  bcm2835_gpio_set_function_pins(gpio, col_pins, n_col_pins, OUT);
  bcm2835_gpio_set_function_pins(gpio, row_pins, n_row_pins, OUT);

  bcm2835_gpio_set_pins(gpio, led_pins, n_led_pins, 0);
  bcm2835_gpio_set_pins(gpio, col_pins, n_col_pins, 1);
  bcm2835_gpio_set_pins(gpio, row_pins, n_row_pins, 1);

  pthread_create(&pidp11->update_thread, NULL, pidp11_update, pidp11);
  return 0;
}

int pidp11_close(pidp11_t *pidp11) {
  pthread_cancel(pidp11->update_thread);
  void *retval;
  pthread_join(pidp11->update_thread, &retval);
  return 0;
}
