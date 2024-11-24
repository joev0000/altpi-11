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

#ifndef RP1_GPIO_H
#define RP1_GPIO_H

#include "gpio.h"
#include <stddef.h>

// clang-format off
static const int RP1_GPIO0_STATUS  = 0x000 >> 2;
static const int RP1_GPIO0_CTRL    = 0x004 >> 2;
static const int RP1_GPIO1_STATUS  = 0x008 >> 2;
static const int RP1_GPIO1_CTRL    = 0x00C >> 2;
static const int RP1_GPIO2_STATUS  = 0x010 >> 2;
static const int RP1_GPIO2_CTRL    = 0x014 >> 2;
static const int RP1_GPIO3_STATUS  = 0x018 >> 2;
static const int RP1_GPIO3_CTRL    = 0x01C >> 2;
static const int RP1_GPIO4_STATUS  = 0x020 >> 2;
static const int RP1_GPIO4_CTRL    = 0x024 >> 2;
static const int RP1_GPIO5_STATUS  = 0x028 >> 2;
static const int RP1_GPIO5_CTRL    = 0x02C >> 2;
static const int RP1_GPIO6_STATUS  = 0x030 >> 2;
static const int RP1_GPIO6_CTRL    = 0x034 >> 2;
static const int RP1_GPIO7_STATUS  = 0x038 >> 2;
static const int RP1_GPIO7_CTRL    = 0x03C >> 2;
static const int RP1_GPIO8_STATUS  = 0x040 >> 2;
static const int RP1_GPIO8_CTRL    = 0x044 >> 2;
static const int RP1_GPIO9_STATUS  = 0x048 >> 2;
static const int RP1_GPIO9_CTRL    = 0x04C >> 2;
static const int RP1_GPIO10_STATUS = 0x050 >> 2;
static const int RP1_GPIO10_CTRL   = 0x054 >> 2;
static const int RP1_GPIO11_STATUS = 0x058 >> 2;
static const int RP1_GPIO11_CTRL   = 0x05C >> 2;
static const int RP1_GPIO12_STATUS = 0x060 >> 2;
static const int RP1_GPIO12_CTRL   = 0x064 >> 2;
static const int RP1_GPIO13_STATUS = 0x068 >> 2;
static const int RP1_GPIO13_CTRL   = 0x06C >> 2;
static const int RP1_GPIO14_STATUS = 0x070 >> 2;
static const int RP1_GPIO14_CTRL   = 0x074 >> 2;
static const int RP1_GPIO15_STATUS = 0x078 >> 2;
static const int RP1_GPIO15_CTRL   = 0x07C >> 2;
static const int RP1_GPIO16_STATUS = 0x080 >> 2;
static const int RP1_GPIO16_CTRL   = 0x084 >> 2;
static const int RP1_GPIO17_STATUS = 0x088 >> 2;
static const int RP1_GPIO17_CTRL   = 0x08C >> 2;
static const int RP1_GPIO18_STATUS = 0x090 >> 2;
static const int RP1_GPIO18_CTRL   = 0x094 >> 2;
static const int RP1_GPIO19_STATUS = 0x098 >> 2;
static const int RP1_GPIO19_CTRL   = 0x09C >> 2;
static const int RP1_GPIO20_STATUS = 0x0A0 >> 2;
static const int RP1_GPIO20_CTRL   = 0x0A4 >> 2;
static const int RP1_GPIO21_STATUS = 0x0A8 >> 2;
static const int RP1_GPIO21_CTRL   = 0x0AC >> 2;
static const int RP1_GPIO22_STATUS = 0x0B0 >> 2;
static const int RP1_GPIO22_CTRL   = 0x0B4 >> 2;
static const int RP1_GPIO23_STATUS = 0x0B8 >> 2;
static const int RP1_GPIO23_CTRL   = 0x0BC >> 2;
static const int RP1_GPIO24_STATUS = 0x0C0 >> 2;
static const int RP1_GPIO24_CTRL   = 0x0C4 >> 2;
static const int RP1_GPIO25_STATUS = 0x0C8 >> 2;
static const int RP1_GPIO25_CTRL   = 0x0CC >> 2;
static const int RP1_GPIO26_STATUS = 0x0D0 >> 2;
static const int RP1_GPIO26_CTRL   = 0x0D4 >> 2;
static const int RP1_GPIO27_STATUS = 0x0D8 >> 2;
static const int RP1_GPIO27_CTRL   = 0x0DC >> 2;
static const int RP1_INTR          = 0x100 >> 2;
static const int RP1_PROC0_INTE    = 0x104 >> 2;
static const int RP1_PROC0_INTF    = 0x108 >> 2;
static const int RP1_PROC0_INTS    = 0x10C >> 2;
static const int RP1_PROC1_INTE    = 0x110 >> 2;
static const int RP1_PROC1_INTF    = 0x114 >> 2;
static const int RP1_PROC1_INTS    = 0x118 >> 2;
static const int RP1_PCIE_INTE     = 0x11C >> 2;
static const int RP1_PCIE_INTF     = 0x120 >> 2;
static const int RP1_PCIE_INTS     = 0x124 >> 2;
// clang-format on

typedef struct {
  volatile uint32_t *base;
} rp1_gpio_ext_t;

/**
 * Initialize the RP1 GPIO data structure.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] base Pointer to the RP1 GPIO registers.
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_init(gpio_t *gpio, rp1_gpio_ext_t *ext);

/**
 * Close a RP1 data structure.
 *
 * @param[in] gpio The GPIO data structure
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_close(gpio_t *gpio);

/**
 * Set the GPIO pin functions.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The mask of pins whose function to change.
 * @param[in] value The function to assign to the pins.
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_set_function_pins(gpio_t *gpio, pin_t *pins, size_t n,
                               pin_function_t value);

/**
 * Set the internal pullup / pulldown state of the GPIO pins.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of pins whose pullup/down configuration will change.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value The pullup/pulldown value to apply to the pins.
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                           pull_control_t value);

/**
 * Set or clear GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins to set or clear.
 * @param[in] n The number of elements in the pins list.
 * @param[in] value if zero, clear the bits, otherwise set the bits.
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_set_pins(gpio_t *gpio, pin_t *pins, size_t n, char value);

/**
 * Get the values of the GPIO bits.
 *
 * @param[in] gpio The GPIO data structure
 * @param[in] pins The list of GPIO pins to set or clear.
 * @param[out] values The values of the GPIO pins.
 * @param[in] n The number of elements in the pins and values lists.
 * @return GPIO_SUCCESS on success.
 */
int rp1_gpio_get_pins(gpio_t *gpio, pin_t *pins, char *values, size_t n);
#endif
