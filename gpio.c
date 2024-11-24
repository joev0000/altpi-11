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

#include "gpio.h"

/**
 * Convert an array of pin indices into a 64 bit value. Pins over 63 are
 * ignored.
 *
 * @param pins the list of pins.
 * @param n the number of pins in the list.
 * @returns an unsigned 64-bit integer
 */
uint64_t pins_to_bits(pin_t *pins, size_t n) {
  uint64_t bits = 0;
  for (int i = 0; i < n; i++) {
    if (pins[i] < 64) {
      bits |= 1 << pins[i];
    }
  }
  return bits;
}

/**
 * Populate a list of pins based on the bits set in a 64 bit value.
 *
 * @param bits the bits to check.
 * @param pins the list of pins to populate
 * @param n the maximum number of pins.
 * @return the number of pins that were set.
 */
int bits_to_pins(uint64_t bits, pin_t *pins, size_t n) {
  int index = 0;
  for (int bit = 0; bit < n; bit++) {
    if ((bits & (1 << bit)) != 0) {
      pins[index++] = bit;
    }
    bit++;
  }
  return index;
}

int gpio_close(gpio_t *gpio) { return (gpio->close)(gpio); }

int gpio_set_function_pins(gpio_t *gpio, pin_t *pins, size_t n,
                           pin_function_t value) {
  if (gpio->set_function_pins) {
    return (gpio->set_function_pins)(gpio, pins, n, value);
  } else {
    return (gpio->set_function_bits)(gpio, pins_to_bits(pins, n), value);
  }
}

int gpio_set_function_bits(gpio_t *gpio, uint64_t pins, pin_function_t value) {
  if (gpio->set_function_bits) {
    return (gpio->set_function_bits)(gpio, pins, value);
  } else {
    pin_t converted_pins[64];
    int n = bits_to_pins(pins, converted_pins, 64);
    return (gpio->set_function_pins)(gpio, converted_pins, n, value);
  }
}

int gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                       pull_control_t value) {
  if (gpio->set_pull_pins) {
    return (gpio->set_pull_pins)(gpio, pins, n, value);
  } else {
    return (gpio->set_pull_bits)(gpio, pins_to_bits(pins, n), value);
  }
}

int gpio_set_pull_bits(gpio_t *gpio, uint64_t pins, pull_control_t value) {
  if (gpio->set_pull_bits) {
    return (gpio->set_pull_bits)(gpio, pins, value);
  } else {
    pin_t converted_pins[64];
    int n = bits_to_pins(pins, converted_pins, 64);
    return (gpio->set_pull_pins)(gpio, converted_pins, n, value);
  }
}

int gpio_set_pins(gpio_t *gpio, pin_t *pins, size_t n, char value) {
  if (gpio->set_pins) {
    return (gpio->set_pins)(gpio, pins, n, value);
  } else {
    return (gpio->set_bits)(gpio, pins_to_bits(pins, n), value);
  }
}

int gpio_set_bits(gpio_t *gpio, uint64_t pins, char value) {
  if (gpio->set_bits) {
    return (gpio->set_bits)(gpio, pins, value);
  } else {
    pin_t converted_pins[64];
    int n = bits_to_pins(pins, converted_pins, 64);
    return (gpio->set_pins)(gpio, converted_pins, n, value);
  }
}

int gpio_get_pins(gpio_t *gpio, pin_t *pins, char *values, size_t n) {
  if (gpio->get_pins) {
    return (gpio->get_pins)(gpio, pins, values, n);
  } else {
    uint64_t value;
    int ret = (gpio->get_bits)(gpio, &value);
    if (ret == 0) {
      for (int i = 0; i < n; i++) {
        values[i] = (value & (1 << pins[1])) ? -1 : 0;
      }
    }
    return ret;
  }
}

int gpio_get_bits(gpio_t *gpio, uint64_t *value) {
  if (gpio->get_bits) {
    return (gpio->get_bits)(gpio, value);
  } else {
    pin_t pins[64] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                      26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                      39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
    char values[64];
    int ret = (gpio->get_pins)(gpio, pins, values, 64);
    uint64_t v = 0;
    for (int i = 0; i < 64; i++) {
      if (values[i]) {
        v |= (1 << i);
      }
    }
    *value = v;
    return ret;
  }
}
