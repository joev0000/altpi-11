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

#include "bcm2835_gpio.h"

int bcm2835_gpio_close(gpio_t *gpio) { return GPIO_SUCCESS; }

int bcm2835_gpio_set_function_bits(gpio_t *gpio, uint64_t pins,
                                   pin_function_t value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;

  if (pins & 0xffc0000000000000) {
    return GPIO_ERR_INVALID_PIN;
  }

  // handle first four GPFSEL registers.
  for (int gpfsel = 0; gpfsel < 5; gpfsel++) {

    // Check ten bits at a time- GPFSEL0-4 each handle ten pins.
    if ((pins & 0x3ff) != 0) {
      uint32_t reg_value = 0;

      // If the bit is set, OR the function code to the register value.
      for (int i = 0; i < 10; i++) {
        if ((pins & (1 << i)) != 0) {
          reg_value |= value << (i * 3);
        }
      }

      *(base + GPFSEL0 + gpfsel) = reg_value;
    }

    pins = pins >> 10;
  }

  // Check bits 50-53, and set GPFSEL5 accordingly.
  if ((pins & 0xf) != 0) {
    uint32_t reg_value = 0;
    for (int i = 0; i < 4; i++) {
      if ((pins & (1 << i)) != 0) {
        reg_value |= value << (i * 3);
      }
    }
    *(base + GPFSEL5) = reg_value;
  }

  return GPIO_SUCCESS;
};

int bcm2835_gpio_set_function_pins(gpio_t *gpio, pin_t *pins, size_t n,
                                   pin_function_t value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;

  uint32_t gpfsel[6] = {*(base + GPFSEL0), *(base + GPFSEL1),
                        *(base + GPFSEL2), *(base + GPFSEL3),
                        *(base + GPFSEL4), *(base + GPFSEL5)};

  for (int i = 0; i < n; i++) {
    int pin = pins[i];
    if (pin < 54) {
      int reg = pin / 10;
      int shift = (pin % 10) * 3;
      int v = value & 0x7;

      gpfsel[reg] = (gpfsel[reg] & ~(0x7 << shift)) | (v << shift);
    } else {
      return GPIO_ERR_INVALID_PIN; // bcm2835 has 54 GPIO pins.
    }
  }

  *(base + GPFSEL0) = gpfsel[0];
  *(base + GPFSEL1) = gpfsel[1];
  *(base + GPFSEL2) = gpfsel[2];
  *(base + GPFSEL3) = gpfsel[3];
  *(base + GPFSEL4) = gpfsel[4];
  *(base + GPFSEL5) = gpfsel[5];

  return GPIO_SUCCESS;
}

int bcm2835_gpio_get_function_pins(gpio_t *gpio, pin_t *pins,
                                   pin_function_t *values, size_t n) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;

  for (int i = 0; i < n; i++) {
    if (pins[i] < 54) {
      int reg = pins[i] / 10;
      int bit = pins[i] % 10;

      values[i] = (*(base + GPFSEL0 + reg) >> (bit * 3)) & 0x7;
    } else {
      return GPIO_ERR_INVALID_PIN;
    }
  }
  return GPIO_SUCCESS;
}

int bcm2835_gpio_set_bits(gpio_t *gpio, uint64_t pins, char value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;

  if ((pins & 0xffc0000000000000) != 0) {
    return GPIO_ERR_INVALID_PIN;
  }
  if (value == 0) {
    *(base + GPCLR0) = pins;
    *(base + GPCLR1) = pins >> 32;
  } else {
    *(base + GPSET0) = pins;
    *(base + GPSET1) = pins >> 32;
  }

  return GPIO_SUCCESS;
};

int bcm2835_gpio_set_pins(gpio_t *gpio, pin_t *pins, size_t n, char value) {
  return bcm2835_gpio_set_bits(gpio, pins_to_bits(pins, n), value);
}

int bcm2835_gpio_get_bits(gpio_t *gpio, uint64_t *pins) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  *pins = *(base + GPLEV0) | ((uint64_t)(*(base + GPLEV1) & 0x001fffff) << 32);
  return GPIO_SUCCESS;
}

int bcm2835_gpio_get_pins(gpio_t *gpio, pin_t *pins, char *values, size_t n) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  for (int i = 0; i < n; i++) {
    if (pins[i] < 32) {
      if ((*(base + GPLEV0) & (1 << pins[i])) != 0) {
        values[i] = -1; // all ones.
      }
    } else if (pins[i] < 54) {
      if ((*(base + GPLEV1) & (1 << (pins[i] - 32))) != 0) {
        values[i] = -1; // all ones.
      }
    } else {
      return GPIO_ERR_INVALID_PIN;
    }
  }

  return GPIO_SUCCESS;
}

// get the GPEDSn values.  Set all 54 GPEDSn bits to clear them.
// TODO: maybe have a mask param?
int bcm2835_gpio_get_and_clear_events(gpio_t *gpio, uint64_t *values) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  *values = ((uint64_t) * (base + GPEDS1) << 32) | *(base + GPEDS0);
  *(base + GPEDS1) = 0x003fffff;
  *(base + GPEDS0) = 0xffffffff;

  return GPIO_SUCCESS;
}

int bcm2835_gpio_set_enable_event_detect_bits(gpio_t *gpio, uint64_t pins,
                                              detection_type_t value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  if (pins & 0xffc0000000000000) {
    return GPIO_ERR_INVALID_PIN;
  }
  if (value & DETECT_RISING) {
    *(base + GPREN0) |= pins;
    *(base + GPREN1) |= pins >> 32;
  }
  if (value & DETECT_FALLING) {
    *(base + GPFEN0) |= pins;
    *(base + GPFEN1) |= pins >> 32;
  }
  if (value & DETECT_HI) {
    *(base + GPHEN0) |= pins;
    *(base + GPHEN1) |= pins >> 32;
  }
  if (value & DETECT_LO) {
    *(base + GPLEN0) |= pins;
    *(base + GPLEN1) |= pins >> 32;
  }
  if (value & DETECT_ASYNC_RISING) {
    *(base + GPAREN0) |= pins;
    *(base + GPAREN1) |= pins >> 32;
  }
  if (value & DETECT_ASYNC_FALLING) {
    *(base + GPAFEN0) |= pins;
    *(base + GPAFEN1) |= pins >> 32;
  }

  return GPIO_SUCCESS;
}

int bcm2835_gpio_set_enable_event_detect_pins(gpio_t *gpio, pin_t *pins,
                                              size_t n,
                                              detection_type_t value) {
  return bcm2835_gpio_set_enable_event_detect_bits(gpio, pins_to_bits(pins, n),
                                                   value);
}

int bcm2835_gpio_clear_enable_event_detect_bits(gpio_t *gpio, uint64_t pins,
                                                detection_type_t value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;

  if (pins & 0xffc0000000000000) {
    return GPIO_ERR_INVALID_PIN;
  }
  if (value & DETECT_RISING) {
    *(base + GPREN0) &= ~pins;
    *(base + GPREN1) &= ~pins >> 32;
  }
  if (value & DETECT_FALLING) {
    *(base + GPFEN0) &= ~pins;
    *(base + GPFEN1) &= ~pins >> 32;
  }
  if (value & DETECT_HI) {
    *(base + GPHEN0) &= ~pins;
    *(base + GPHEN1) &= ~pins >> 32;
  }
  if (value & DETECT_LO) {
    *(base + GPLEN0) &= ~pins;
    *(base + GPLEN1) &= ~pins >> 32;
  }
  if (value & DETECT_ASYNC_RISING) {
    *(base + GPAREN0) &= ~pins;
    *(base + GPAREN1) &= ~pins >> 32;
  }
  if (value & DETECT_ASYNC_FALLING) {
    *(base + GPAFEN0) &= ~pins;
    *(base + GPAFEN1) &= ~pins >> 32;
  }

  return GPIO_SUCCESS;
}

int bcm2835_gpio_clear_enable_event_detect_pins(gpio_t *gpio, pin_t *pins,
                                                size_t n,
                                                detection_type_t value) {
  return bcm2835_gpio_clear_enable_event_detect_bits(
      gpio, pins_to_bits(pins, n), value);
}

int bcm2835_gpio_get_enable_event_detect_pins(gpio_t *gpio, pin_t *pins,
                                              detection_type_t *values,
                                              size_t n) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  uint64_t gpren = (((uint64_t) * (base + GPREN1)) << 32) | *(base + GPREN0);
  uint64_t gpfen = (((uint64_t) * (base + GPFEN1)) << 32) | *(base + GPFEN0);
  uint64_t gphen = (((uint64_t) * (base + GPHEN1)) << 32) | *(base + GPHEN0);
  uint64_t gplen = (((uint64_t) * (base + GPLEN1)) << 32) | *(base + GPLEN0);
  uint64_t gparen = (((uint64_t) * (base + GPAREN1)) << 32) | *(base + GPAREN0);
  uint64_t gpafen = (((uint64_t) * (base + GPAFEN1)) << 32) | *(base + GPAFEN0);

  for (int i = 0; i < n; i++) {
    if (pins[i] > 53) {
      return GPIO_ERR_INVALID_PIN;
    }
    uint64_t mask = 1 << pins[i];
    detection_type_t value = 0;
    if (gpren & mask) {
      value |= DETECT_RISING;
    }
    if (gpfen & mask) {
      value |= DETECT_FALLING;
    }
    if (gphen & mask) {
      value |= DETECT_HI;
    }
    if (gplen & mask) {
      value |= DETECT_LO;
    }
    if (gparen & mask) {
      value |= DETECT_ASYNC_RISING;
    }
    if (gpafen & mask) {
      value |= DETECT_ASYNC_FALLING;
    }

    values[i] = value;
  }
  return GPIO_SUCCESS;
}

int bcm2835_gpio_set_pull_bits(gpio_t *gpio, uint64_t pins,
                               pull_control_t value) {
  volatile uint32_t *base = ((bcm2835_gpio_ext_t *)gpio->ext)->base;
  /*
   * The sequence to set internal pullup/pulldown resistors is
   *
   *   1. Set the pull up/down register with the pull value.
   *   2. Wait 150 cycles.
   *   3. Set the pull up/down clock with the mask of the pins to set.
   *   4. Wait 150 cycles.
   *   5. Zero the pull up/down register.
   *   6. Wait 150 cycles.
   *   7. Zero the pull up/down clock.
   *   8. Wait 150 cycles.
   */
  if (pins & 0xffc0000000000000) {
    return GPIO_ERR_INVALID_PIN;
  }
  *(base + GPPUD) = value;
  usleep(10); // from raspi-gpio
  *(base + GPPUDCLK0) = pins;
  *(base + GPPUDCLK1) = pins >> 32;
  usleep(10);
  *(base + GPPUD) = OFF;
  usleep(10);
  *(base + GPPUDCLK0) = 0;
  *(base + GPPUDCLK1) = 0;
  usleep(10);

  return GPIO_SUCCESS;
};

int bcm2835_gpio_set_pull_pins(gpio_t *gpio, pin_t *pins, size_t n,
                               pull_control_t value) {
  return bcm2835_gpio_set_pull_bits(gpio, pins_to_bits(pins, n), value);
}

int bcm2835_gpio_init(gpio_t *gpio, bcm2835_gpio_ext_t *ext) {
  if (gpio == NULL || ext == NULL || ext->base == NULL) {
    return -1; // TODO: better error return value.
  }

  gpio->close = bcm2835_gpio_close;
  gpio->set_function_pins = bcm2835_gpio_set_function_pins;
  gpio->set_function_bits = bcm2835_gpio_set_function_bits;
  gpio->set_pull_pins = bcm2835_gpio_set_pull_pins;
  gpio->set_pull_bits = bcm2835_gpio_set_pull_bits;
  gpio->set_pins = bcm2835_gpio_set_pins;
  gpio->set_bits = bcm2835_gpio_set_bits;
  gpio->get_pins = bcm2835_gpio_get_pins;
  gpio->get_bits = bcm2835_gpio_get_bits;

  gpio->ext = ext;

  return GPIO_SUCCESS;
}
