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

#include "rp1_gpio.h"
#include <stddef.h>

int rp1_gpio_close(gpio_t *gpio) { return GPIO_SUCCESS; }

int rp1_gpio_set_function_pins(gpio_t *gpio, pin_t *pins, size_t n,
                               pin_function_t value) {
  return -1;
}

int rp1_gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                           pull_control_t value) {
  return -1;
}

int rp1_gpio_set_pins(gpio_t *gpio, pin_t *pins, size_t n, char value) {
  return -1;
}

int rp1_gpio_get_pins(gpio_t *gpio, pin_t *pins, char *values, size_t n) {
  volatile uint32_t *base = ((rp1_gpio_ext_t *)gpio->ext)->base;

  for (int i = 0; i < n; i++) {
    if (pins[i] < 28) {
      if (*(base + RP1_GPIO0_STATUS + (i * 2)) & (1 << 23)) {
        values[i] = -1;
      }
    }
  }

  return GPIO_SUCCESS;
}

int rp1_gpio_init(gpio_t *gpio, rp1_gpio_ext_t *ext) {
  if (gpio == NULL || ext == NULL || ext->base == NULL) {
    return -1; // TODO: better error return value.
  }

  gpio->close = rp1_gpio_close;
  gpio->set_function_pins = rp1_gpio_set_function_pins;
  gpio->set_pull_pins = rp1_gpio_set_pull_pins;
  gpio->set_pins = rp1_gpio_set_pins;
  gpio->get_pins = rp1_gpio_get_pins;

  gpio->ext = ext;

  return GPIO_SUCCESS;
}
