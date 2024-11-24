#!/bin/sh
set -xeu

SIMH_SRC=${SIMH_SRC:-../simh}
SIMH_OBJ="sim_sock.o"

COMMON_OBJ="pidp11.o gpio.o bcm2835_gpio.o bcm2711_gpio.o rp1_gpio.o"

CC_FLAGS=${CC_FLAGS:-"-Wall"}
DEBUG_FLAGS=${DEBUG_FLAGS:-"-DDEBUG -g"}

for f in *.c ; do
  gcc $CC_FLAGS $DEBUG_FLAGS -I$SIMH_SRC -c $f
done

gcc -Wno-unused-function $CC_FLAGS $DEBUG_FLAGS -I$SIMH_SRC -c $SIMH_SRC/sim_sock.c

gcc -o pidp11 main.o $SIMH_OBJ $COMMON_OBJ
gcc -o pidp11-off pidp11-off.o $COMMON_OBJ
