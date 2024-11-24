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

#ifndef PIDP11_H

#include <pthread.h>

#include "gpio.h"

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

typedef struct _pidp11_t {
  gpio_t *gpio;
  pthread_t update_thread;

  // The lamps
  uint32_t address;
  uint16_t data;
  addr_mode_t addr_mode;
  data_mode_t data_mode;
  addressing_length_t addressing_length;
  char parity_high;
  char parity_low;

  char parity_err;
  char address_err;
  run_state_t run_state;
  run_level_t run_level;
  char data_ref;

  // The switches
  uint32_t switch_reg;
  char switch_test;
  char switch_load_add;
  char switch_exam;
  char switch_dep;
  char switch_cont;
  char switch_ena_halt;
  char switch_sing_inst;
  char switch_start;
  char switch_addr;
  char switch_addr_rot1;
  char switch_addr_rot2;
  char switch_data;
  char switch_data_rot1;
  char switch_data_rot2;
} pidp11_t;

/**
 * Initialize PiDP11. Starts a thread to continuously refresh the display.
 *
 * @param[in] pidp11 The PiDP11 data structure
 * @param[in] gpio The GPIO connected to PiDP11
 * @return zero on success.
 */
int pidp11_init(pidp11_t *pidp11, gpio_t *gpio);

/**
 * Close PiDP11. Cancells the display update thread.
 *
 * @param[in] pidp11 The PiDP11 data structure
 * @return zero on success.
 */
int pidp11_close(pidp11_t *pidp11);
#endif
