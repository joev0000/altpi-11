#!/bin/sh
set -xeu

SIMH_SRC=${SIMH_SRC:-../simh}
SIMH_OBJ="sim_sock.o"

COMMON_OBJ="pidp11.o bcm2835_gpio.o"

CC_FLAGS=${CC_FLAGS:-"-DDEBUG -g"}

cp $SIMH_SRC/sim_sock.h \
   $SIMH_SRC/sim_sock.c \
   $SIMH_SRC/sim_frontpanel.h \
   $SIMH_SRC/sim_frontpanel.c \
   .

for f in *.c ; do
  gcc -Wall $CC_FLAGS -c $f
done


gcc -o test main.o $SIMH_OBJ $COMMON_OBJ
gcc -o pidp11-off pidp11-off.o $COMMON_OBJ
