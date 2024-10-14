set -xe

SIMH_SRC=${SIMH_SRC:-../simh}

cp $SIMH_SRC/sim_sock.h \
   $SIMH_SRC/sim_sock.c \
   $SIMH_SRC/sim_frontpanel.h \
   $SIMH_SRC/sim_frontpanel.c \
   .

for f in *.c ; do
  gcc -DDEBUG -g -Wall -c $f
done

gcc -o test *.o

