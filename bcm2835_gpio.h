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
/* The offests of the GPIO registers. */
static const int GPFSEL0   = 0x00 >> 2;
static const int GPFSEL1   = 0x04 >> 2;
static const int GPFSEL2   = 0x08 >> 2;
static const int GPFSEL3   = 0x0C >> 2;
static const int GPFSEL4   = 0x10 >> 2;
static const int GPFSEL5   = 0x14 >> 2;
static const int GPSET0    = 0x1C >> 2;
static const int GPSET1    = 0x20 >> 2;
static const int GPCLR0    = 0x28 >> 2;
static const int GPCLR1    = 0x2C >> 2;
static const int GPLEV0    = 0x34 >> 2;
static const int GPLEV1    = 0x38 >> 2;
static const int GPEDS0    = 0x40 >> 2;
static const int GPEDS1    = 0x44 >> 2;
static const int GPREN0    = 0x4C >> 2;
static const int GPREN1    = 0x50 >> 2;
static const int GPFEN0    = 0x48 >> 2;
static const int GPFEN1    = 0x5C >> 2;
static const int GPHEN0    = 0x64 >> 2;
static const int GPHEN1    = 0x68 >> 2;
static const int GPLEN0    = 0x70 >> 2;
static const int GPLEN1    = 0x74 >> 2;
static const int GPAREN0   = 0x7C >> 2;
static const int GPAREN1   = 0x80 >> 2;
static const int GPAFEN0   = 0x88 >> 2;
static const int GPAFEN1   = 0x8C >> 2;
static const int GPPUD     = 0x94 >> 2;
static const int GPPUDCLK0 = 0x98 >> 2;
static const int GPPUDCLK1 = 0x9C >> 2;
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

typedef struct _bcm2835_gpio_t {
  volatile uint32_t *base;
} bcm2835_gpio_t;

typedef uint32_t detection_type_t;

// clang-format off
static const detection_type_t DETECT_RISING        = 1 << 0;
static const detection_type_t DETECT_FALLING       = 1 << 1;
static const detection_type_t DETECT_HI            = 1 << 2;
static const detection_type_t DETECT_LO            = 1 << 3;
static const detection_type_t DETECT_ASYNC_RISING  = 1 << 4;
static const detection_type_t DETECT_ASYNC_FALLING = 1 << 5;
// clang-format on
static const detection_type_t DETECT_ALL =
    DETECT_RISING | DETECT_FALLING | DETECT_HI | DETECT_LO |
    DETECT_ASYNC_RISING | DETECT_ASYNC_FALLING;

static const int GPIO_SUCCESS = 0;
static const int GPIO_ERR_INVALID_PIN = 1;
static const int GPIO_ERR_INVALID_BASE = 2;

/**
 * Initialize the bcm2835 GPIO data structure.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] base Pointer to the BCM2835 GPIO registers.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_init(bcm2835_gpio_t *gpio, volatile uint32_t *base);

/**
 * Close a bcm2835 data structure.
 *
 * @param[in] gpio The GPIO data structure
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_close(bcm2835_gpio_t *gpio);

/**
 * Set the GPIO pin functions.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of pins whose function to change.
 * @param[in] value The function to assign to the pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_function_bits(bcm2835_gpio_t *gpio, uint64_t pins,
                                   pin_function_t value);

/**
 * Set the GPIO pin functions.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of pins whose function to be changed.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value The function to assign to the pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_function_pins(bcm2835_gpio_t *gpio, pin_t *pins, size_t n,
                                   pin_function_t value);

/**
 * Get the GPIO pin functions. The caller provides a pointer to an
 * array that holds the function values for the requested pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of pins whose function to be changed.
 * @param[out] values The function values for the pins.
 * @param[in] n The number of elements in the pins and values lists.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_get_function_pins(bcm2835_gpio_t *gpio, pin_t *pins,
                                   pin_function_t *values, size_t n);

/**
 * Set or clear (up to the first 64) GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] bits The mask of GPIO bits to set or clear.
 * @param[in] value if zero, clear the bits, otherwise set the bits.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_bits(bcm2835_gpio_t *gpio, uint64_t pins, char value);

/**
 * Set or clear GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins to set or clear.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value if zero, clear the bits, otherwise set the bits.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_pins(bcm2835_gpio_t *gpio, pin_t *pins, size_t n,
                          char value);

/**
 * Get the values of the first 64 GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[out] values The values of the GPIO pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_get_bits(bcm2835_gpio_t *gpio, uint64_t *values);

/**
 * Get the values of the GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins to set or clear.
 * @param[out] values The values of the GPIO pins.
 * @param[in] n The number of elements in the pins and values lists.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_get_pins(bcm2835_gpio_t *gpio, pin_t *pins, char *values,
                          size_t n);

/**
 * Get the event flags, and clear them to prepare for the next events.
 *
 * @param[in] gpio The GPIO data structure.
 * @param[out] vaues The pins which have at least one triggered event.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_get_and_clear_events(bcm2835_gpio_t *gpio, uint64_t *values);

/**
 * Set the enable event detection bits in the first 64 GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of GPIO bits whose event detection will be enabled.
 * @param[in] value The event detection type to enable.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_enable_event_detect_bits(bcm2835_gpio_t *gpio,
                                              uint64_t pins,
                                              detection_type_t value);

/**
 * Set the enable event detection bits in the first 64 GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins whose event detection will be enabled.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value The event detection type to enable.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_enable_event_detect_pins(bcm2835_gpio_t *gpio, pin_t *pins,
                                              size_t n, detection_type_t value);

/**
 * Clear the enable event detection bits in the first 64 GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of GPIO bits whose event detection will be enabled.
 * @param[in] value The event detection type to enable.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_clear_enable_event_detect_bits(bcm2835_gpio_t *gpio,
                                                uint64_t pins,
                                                detection_type_t value);

/**
 * Clear the enable event detection bits in the first 64 GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins whose event detection will be disabled.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value The event detection type to enable.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_clear_enable_event_detect_pins(bcm2835_gpio_t *gpio,
                                                pin_t *pins, size_t n,
                                                detection_type_t value);

/**
 * Get the set of pins within the first 64 GPIO pins whose event detection
 * is enabled.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of pins to check.
 * @param[out] value The event detection type for the corresponding pin.
 * @param[in] n The number of elements in the pins and value lists.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_get_enable_event_detect_pins(bcm2835_gpio_t *gpio, pin_t *pins,
                                              detection_type_t *values,
                                              size_t n);

/**
 * Set the internal pullup / pulldown state of the GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of pins whose pullup/down configuration will change.
 * @param[in] value The pullup/pulldown value to apply to the pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_pull_bits(bcm2835_gpio_t *gpio, uint64_t pins,
                               pull_control_t value);

/**
 * Set the internal pullup / pulldown state of the GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of pins whose pullup/down configuration will change.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value The pullup/pulldown value to apply to the pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2835_gpio_set_pull_pins(bcm2835_gpio_t *gpio, pin_t *pins, size_t n,
                               pull_control_t value);

#endif
