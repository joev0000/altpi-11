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

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "pidp11.h"
#include "bcm2835_gpio.h"

int main(int argc, char **argv) {
  bcm2835_gpio_t gpio = {0};
  pidp11_t pidp11 = {0};

  int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  size_t mem_length = 0x100;
  volatile uint32_t *base = (uint32_t *)mmap(
      NULL, mem_length, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  close(mem_fd);

  bcm2835_gpio_init(&gpio, base);
  pidp11_init(&pidp11, &gpio);
  for (int i = 0; i < 1000; i++) {
    pidp11.data = 1 << (i % 15);
    pidp11.address = pidp11.switch_reg;
    usleep(5000);
  }
  pidp11_close(&pidp11);
  bcm2835_gpio_close(&gpio);

  munmap((void *)base, mem_length);
  return 0;
}
