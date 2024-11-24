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

#ifndef BCM2711_GPIO_H
#define BCM2711_GPIO_H

#include <stddef.h>
#include "gpio.h"
#include "bcm2835_gpio.h"

static const int GPIO_PUP_PDN_CNTRL_REG0 = 0xE4 >> 2;
static const int GPIO_PUP_PDN_CNTRL_REG1 = 0xE8 >> 2;
static const int GPIO_PUP_PDN_CNTRL_REG2 = 0xEC >> 2;
static const int GPIO_PUP_PDN_CNTRL_REG3 = 0xF0 >> 2;

// Same structure as BCM2835
typedef bcm2835_gpio_ext_t bcm2711_gpio_ext_t;

/**
 * Set the internal pullup / pulldown state of the GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of pins whose pullup/down configuration will change.
 * @param[in] value The pullup/pulldown value to apply to the pins.
 * @return GPIO_SUCCESS on success.
 */
int bcm2711_gpio_set_pull_bits(gpio_t *gpio, uint64_t pins,
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
int bcm2711_gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                               pull_control_t value);

int bcm2711_gpio_init(gpio_t *gpio, bcm2711_gpio_ext_t *ext);
#endif
