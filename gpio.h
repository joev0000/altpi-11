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

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

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

typedef struct _gpio_t {
  int (*close)(struct _gpio_t *gpio);

  int (*set_function_pins)(struct _gpio_t *gpio, pin_t *pins, size_t n,
                           pin_function_t value);
  int (*set_function_bits)(struct _gpio_t *gpio, uint64_t pins,
                           pin_function_t value);

  int (*set_pull_pins)(struct _gpio_t *gpio, pin_t *pins, size_t n,
                       pull_control_t value);
  int (*set_pull_bits)(struct _gpio_t *gpio, uint64_t pins,
                       pull_control_t value);

  int (*set_pins)(struct _gpio_t *gpio, pin_t *pins, size_t n, char value);
  int (*set_bits)(struct _gpio_t *gpio, uint64_t pins, char value);

  int (*get_pins)(struct _gpio_t *gpio, pin_t *pins, char *values, size_t n);
  int (*get_bits)(struct _gpio_t *gpio, uint64_t *value);

  void *ext;
} gpio_t;

/**
 * Close a GPIO device.
 *
 * @param gpio the GPIO device to close.
 * @return zero on success.
 */
int gpio_close(gpio_t *gpio);

/**
 * Set pin functions on the list of pins.
 *
 * @param gpio the GPIO device
 * @param pins the list of pins whose function is to be set.
 * @param n the number of elements in the pins array.
 * @param value the function to assign to the pins.
 * @return zero on success.
 */
int gpio_set_function_pins(gpio_t *gpio, pin_t *pins, size_t n,
                           pin_function_t value);

/**
 * Set pin function on pins set in the mask.
 *
 * @param gpio the GPIO device.
 * @param pins set the function on the pins in the bit set.
 * @param value the function to assign to the pins.
 * @return zero on success.
 */
int gpio_set_function_bits(gpio_t *gpio, uint64_t pins, pin_function_t value);

/**
 * Set the pull-up/pull-down on the list of pins.
 *
 * @param gpio the GPIO device.
 * @param pins the list of pins whose pull setting is to be set.
 * @param n the number of elements in the pins array.
 * @param value the pull setting to set.
 * @return zero on success.
 */
int gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                       pull_control_t value);

/**
 * Set the pull-up/pull-down on the set of pins.
 *
 * @param gpio the GPIO device.
 * @param pins set the pull setting on the pins in the bit set.
 * @param value the pull setting to set.
 * @return zero on success.
 */
int gpio_set_pull_bits(gpio_t *gpio, uint64_t pins, pull_control_t value);

/**
 * Set the value on the list of pins.
 *
 * @param gpio the GPIO device.
 * @param pins the list of pins whose value is to be set.
 * @param n the number of elements in the pins array.
 * @param value if zero, set the pins low, otherwise, set the pins high.
 * @return zero on success.
 */
int gpio_set_pins(gpio_t *gpio, pin_t *pins, size_t n, char value);

/**
 * Set the value of the set of pins.
 *
 * @param gpio the GPIO device.
 * @param pins the set of pins whose values will be set or cleared.
 * @param value if zero, set the pins low, otherwise, set the pins high.
 * @return zero on success.
 */
int gpio_set_bits(gpio_t *gpio, uint64_t pins, char value);

/**
 * Get the values of the list of pins.
 *
 * @param gpio the GPIO device.
 * @param pins the list of pins whose values will be retrieved.
 * @param values the destination of the values that are retrieved.
 * @param n the number of elements in the pins and values arrays.
 * @return zero on success.
 */
int gpio_get_pins(gpio_t *gpio, pin_t *pins, char *values, size_t n);

/**
 * Get the values of the first 64 pins.
 *
 * @param gpio the GPIO device.
 * @param value the destination of the values of the pins.
 * @return zero on success.
 */
int gpio_get_bits(gpio_t *gpio, uint64_t *value);

/**
 * Iterate through the pins array, setting the bits in the resulting value.
 *
 * @param pins the pins to set in the result.
 * @param n the number of elements in the array.
 * @return a uint64_t whose bits are set if the corresponding pin appears in the
 * pin array.
 */
uint64_t pins_to_bits(pin_t *pins, size_t n);

/**
 * Populate a pin array based on which bits are set.
 *
 * @param bits the value woth the bits to check.
 * @param pins the destination array with values corresponding to the set bits.
 * @param n the maxumum number of pins to check.
 * @return the number of valid pins in the pins array.
 */
int bits_to_pins(uint64_t bits, pin_t *pins, size_t n);
#endif
