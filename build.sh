set -xe

for f in *.c ; do
  gcc -DDEBUG -g -Wall -c $f
done

gcc -o test *.o

