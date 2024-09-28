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
#include <stdint.h>
#include <unistd.h>

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

#include "bcm2837_gpio.h"

void bcm2837_gpio_set_pin_function(volatile uint32_t *gpio, pin_t *pins,
                                   pin_function_t value, size_t n) {
  uint32_t gpfsel[6] = {
      *(gpio + BCM2837_GPIO_GPFSEL0), *(gpio + BCM2837_GPIO_GPFSEL1),
      *(gpio + BCM2837_GPIO_GPFSEL2), *(gpio + BCM2837_GPIO_GPFSEL3),
      *(gpio + BCM2837_GPIO_GPFSEL4), *(gpio + BCM2837_GPIO_GPFSEL5)};

  int v = value & 0x7;

  for (int i = 0; i < n; i++) {
    int pin = pins[i];
#ifdef DEBUG
    if (pin < 4 || pin == 14 || pin == 15 || pin > 27) {
      printf("DANGER: _set_pin_function: pin %i being set.\n", pin);
      exit(1);
    }
#endif
    if (pin <= 53) { // ignore nonexistant pin > 53.
      int reg = pin / 10;
      int shift = (pin % 10) * 3;

      gpfsel[reg] = (gpfsel[reg] & ~(0x7 << shift)) | (v << shift);
    }
  }

  *(gpio + BCM2837_GPIO_GPFSEL0) = gpfsel[0];
  *(gpio + BCM2837_GPIO_GPFSEL1) = gpfsel[1];
  *(gpio + BCM2837_GPIO_GPFSEL2) = gpfsel[2];
  *(gpio + BCM2837_GPIO_GPFSEL3) = gpfsel[3];
  *(gpio + BCM2837_GPIO_GPFSEL4) = gpfsel[4];
  *(gpio + BCM2837_GPIO_GPFSEL5) = gpfsel[5];
}

void bcm2837_gpio_set_bits(volatile uint32_t *gpio, uint64_t bits) {
#ifdef DEBUG
  static uint64_t mask = 0x000000000ff73ff0;
  if ((bits & ~mask) != 0) {
    printf("DANGER: _set_bits: bits outisde Pi gpio range: %lx\n",
           bits & ~mask);
    exit(1);
  }
#endif
  *(gpio + BCM2837_GPIO_GPSET0) = bits;
  *(gpio + BCM2837_GPIO_GPSET1) = bits >> 32;
}

void bcm2837_gpio_set_pins(volatile uint32_t *gpio, pin_t *pins, size_t n) {
  uint64_t bits = 0;
  for (int i = 0; i < n; i++) {
    if (pins[i] <= 53) {
      bits |= (1 << pins[i]);
    }
  }
  bcm2837_gpio_set_bits(gpio, bits);
}

void bcm2837_gpio_clear_bits(volatile uint32_t *gpio, uint64_t bits) {
#ifdef DEBUG
  static uint64_t mask = 0x000000000ff73ff0;
  if ((bits & ~mask) != 0) {
    printf("DANGER: _clear_bits: bits outisde Pi gpio range: %lx\n",
           bits & ~mask);
    exit(1);
  }
#endif
  *(gpio + BCM2837_GPIO_GPCLR0) = bits;
  *(gpio + BCM2837_GPIO_GPCLR1) = bits >> 32;
}

void bcm2837_gpio_clear_pins(volatile uint32_t *gpio, pin_t *pins, size_t n) {
  uint64_t bits = 0;
  for (int i = 0; i < n; i++) {
    if (pins[i] <= 53) {
      bits |= (1 << pins[i]);
    }
  }
  bcm2837_gpio_clear_bits(gpio, bits);
}

void bcm2837_gpio_pull_bits(volatile uint32_t *gpio, uint64_t bits,
                            pull_control_t pull) {
#ifdef DEBUG
  static uint64_t mask = 0x000000000ff73ff0;
  if ((bits & ~mask) != 0) {
    printf("DANGER: _pull_bits: bits outisde Pi gpio range: %lx, pull: %i\n",
           bits & ~mask, pull);
    exit(1);
  }
#endif
  *(gpio + BCM2837_GPIO_GPPUD) = pull;
  usleep(10); // from raspi-gpio
  *(gpio + BCM2837_GPIO_GPPUDCLK0) = bits;
  *(gpio + BCM2837_GPIO_GPPUDCLK1) = bits >> 32;
  usleep(10);
  *(gpio + BCM2837_GPIO_GPPUD) = OFF;
  usleep(10);
  *(gpio + BCM2837_GPIO_GPPUDCLK0) = 0;
  *(gpio + BCM2837_GPIO_GPPUDCLK1) = 0;
  usleep(10);
}

void bcm2837_gpio_pull_pins(volatile uint32_t *gpio, pin_t *pins,
                            pull_control_t pull, size_t n) {
  uint64_t bits = 0;
  for (int i = 0; i < n; i++) {
    if (pins[i] <= 53) {
      bits |= (1 << pins[i]);
    }
  }
  bcm2837_gpio_pull_bits(gpio, bits, pull);
}

void bcm2837_gpio_get_bits(volatile uint32_t *gpio, uint64_t *bits) {
  *bits = *(gpio + BCM2837_GPIO_GPLEV0) |
          ((uint64_t)(*(gpio + BCM2837_GPIO_GPLEV1) & 0x001fffff) << 32);
}

void bcm2837_gpio_init(volatile uint32_t *gpio) {}
