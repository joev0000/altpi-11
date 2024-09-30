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

#ifndef bcm2835_bcm2835_GPIO_GPIO_H
#define bcm2835_bcm2835_GPIO_GPIO_H

#include <stddef.h>
#include <stdint.h>

// clang-format off
#define bcm2835_GPIO_GPFSEL0   (0x00 >> 2)
#define bcm2835_GPIO_GPFSEL1   (0x04 >> 2)
#define bcm2835_GPIO_GPFSEL2   (0x08 >> 2)
#define bcm2835_GPIO_GPFSEL3   (0x0C >> 2)
#define bcm2835_GPIO_GPFSEL4   (0x10 >> 2)
#define bcm2835_GPIO_GPFSEL5   (0x14 >> 2)
#define bcm2835_GPIO_GPSET0    (0x1C >> 2)
#define bcm2835_GPIO_GPSET1    (0x20 >> 2)
#define bcm2835_GPIO_GPCLR0    (0x28 >> 2)
#define bcm2835_GPIO_GPCLR1    (0x2C >> 2)
#define bcm2835_GPIO_GPLEV0    (0x34 >> 2)
#define bcm2835_GPIO_GPLEV1    (0x38 >> 2)
#define bcm2835_GPIO_GPEDS0    (0x40 >> 2)
#define bcm2835_GPIO_GPEDS1    (0x44 >> 2)
#define bcm2835_GPIO_GPREN0    (0x4C >> 2)
#define bcm2835_GPIO_GPREN1    (0x50 >> 2)
#define bcm2835_GPIO_GPFEN0    (0x48 >> 2)
#define bcm2835_GPIO_GPFEN1    (0x5C >> 2)
#define bcm2835_GPIO_GPHEN0    (0x64 >> 2)
#define bcm2835_GPIO_GPHEN1    (0x68 >> 2)
#define bcm2835_GPIO_GPLEN0    (0x70 >> 2)
#define bcm2835_GPIO_GPLEN1    (0x74 >> 2)
#define bcm2835_GPIO_GPPAREN0  (0x7C >> 2)
#define bcm2835_GPIO_GPPAREN1  (0x80 >> 2)
#define bcm2835_GPIO_GPPAFEN0  (0x88 >> 2)
#define bcm2835_GPIO_GPPAFEN1  (0x8C >> 2)
#define bcm2835_GPIO_GPPUD     (0x94 >> 2)
#define bcm2835_GPIO_GPPUDCLK0 (0x98 >> 2)
#define bcm2835_GPIO_GPPUDCLK1 (0x9C >> 2)
// clang-format on

typedef unsigned int pin_t;

typedef enum _pin_function_t {
  IN,
  OUT,
  ALT5,
  ALT4,
  ALT0,
  ALT1,
  ALT2,
  ALT3
} pin_function_t;

typedef enum _pull_control_t { OFF, DOWN, UP } pull_control_t;

void bcm2835_gpio_init(volatile uint32_t *gpio);
void bcm2835_gpio_set_pin_function(volatile uint32_t *gpio, pin_t *pins,
                                   pin_function_t value, size_t n);
void bcm2835_gpio_set_bits(volatile uint32_t *gpio, uint64_t bits);
void bcm2835_gpio_set_pins(volatile uint32_t *gpio, pin_t *pins, size_t n);
void bcm2835_gpio_clear_bits(volatile uint32_t *gpio, uint64_t bits);
void bcm2835_gpio_clear_pins(volatile uint32_t *gpio, pin_t *pins, size_t n);
void bcm2835_gpio_get_bits(volatile uint32_t *gpio, uint64_t *bits);
void bcm2835_gpio_pull_bits(volatile uint32_t *gpio, uint64_t bits,
                            pull_control_t pull);
void bcm2835_gpio_pull_pins(volatile uint32_t *gpio, pin_t *pins,
                            pull_control_t pull, size_t n);

#endif
