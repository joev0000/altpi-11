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

#include <stddef.h>

#include "bcm2711_gpio.h"
#include "bcm2835_gpio.h"
#include "gpio.h"

int bcm2711_gpio_set_pull_bits(gpio_t *gpio, uint64_t pins,
                               pull_control_t value) {
  volatile uint32_t *base = ((bcm2711_gpio_ext_t *)gpio->ext)->base;

  int v;

  // Note: the bits for UP and DOWN differ from the BCM2835.
  switch (value) {
  case OFF:
    v = 0;
    break;
  case UP:
    v = 1;
    break;
  case DOWN:
    v = 2;
    break;
  }

  for(int reg = 0; reg < 3; reg++) {
    uint32_t reg_value = 0;
    for(int i = 0; i < 16; i++) {
      if((pins & (1 << i)) != 0) {
        reg_value = (reg_value & ~(3 << (i << 1))) | (v << (i << 1));
      }
    }
    *(base + GPIO_PUP_PDN_CNTRL_REG0 + reg) = reg_value;
  }

  // Check bits 48-57 for CNTRL_REG3
  if((pins & (0x03ff000000000000)) != 0) {
    uint64_t p = pins >> 48;
    uint32_t reg_value = 0;
    for(int i = 0; i < 10; i++) {
      if((p & (1 << i)) != 0) {
        reg_value = (reg_value & ~(3 << (i << 1))) | (v << (i << 1));
      }
    }
    *(base + GPIO_PUP_PDN_CNTRL_REG3) = reg_value;
  }
  return GPIO_SUCCESS;
}

int bcm2711_gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                               pull_control_t value) {
  volatile uint32_t *base = ((bcm2711_gpio_ext_t *)gpio->ext)->base;

  uint32_t gpio_pup_pdn_cntrl[4] = {
      *(base + GPIO_PUP_PDN_CNTRL_REG0), *(base + GPIO_PUP_PDN_CNTRL_REG1),
      *(base + GPIO_PUP_PDN_CNTRL_REG2), *(base + GPIO_PUP_PDN_CNTRL_REG3)};

  int v;

  // Note: the bits for UP and DOWN differ from the BCM2835.
  switch (value) {
  case OFF:
    v = 0;
    break;
  case UP:
    v = 1;
    break;
  case DOWN:
    v = 2;
    break;
  }

  for (int i = 0; i < n; i++) {
    if (pins[i] <= 57) {
      int reg = pins[i] >> 4;
      int shift = (pins[i] & 0xf) << 1;
      gpio_pup_pdn_cntrl[reg] =
          (gpio_pup_pdn_cntrl[reg] & ~(0x3 << shift)) | (v << shift);
    } else {
      return GPIO_ERR_INVALID_PIN;
    }
  }

  *(base + GPIO_PUP_PDN_CNTRL_REG0) = gpio_pup_pdn_cntrl[0];
  *(base + GPIO_PUP_PDN_CNTRL_REG1) = gpio_pup_pdn_cntrl[1];
  *(base + GPIO_PUP_PDN_CNTRL_REG2) = gpio_pup_pdn_cntrl[2];
  *(base + GPIO_PUP_PDN_CNTRL_REG3) = gpio_pup_pdn_cntrl[3];

  return GPIO_SUCCESS;
}

int bcm2711_gpio_init(gpio_t *gpio, bcm2711_gpio_ext_t *ext) {
  if (gpio == NULL || ext == NULL || ext->base == NULL) {
    return -1; // TODO: better error return value.
  }

  // Same as BCM2835, except the pull functions.
  gpio->close = bcm2835_gpio_close;
  gpio->set_function_pins = bcm2835_gpio_set_function_pins;
  gpio->set_function_bits = bcm2835_gpio_set_function_bits;
  gpio->set_pull_pins = bcm2711_gpio_set_pull_pins;
  gpio->set_pull_bits = bcm2711_gpio_set_pull_bits;
  gpio->set_pins = bcm2835_gpio_set_pins;
  gpio->set_bits = bcm2835_gpio_set_bits;

  gpio->ext = ext;

  return GPIO_SUCCESS;
}
