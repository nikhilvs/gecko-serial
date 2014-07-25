#!/bin/bash


GECKO_LIB_DIR=/root/gecko_serial
USER_DIR=/root/lib_user
GECKO_LIB=$GECKO_LIB_DIR/Debug/libgecko-serial.a
OPTIONS=$GECKO_LIB" -lpthread"

cd $GECKO_LIB_DIR/Debug
make clean && make ||{
echo "make failed do terminating"
exit -1
}

cd $USER_DIR
echo $OPTIONS
gcc -g -o gecko example1.c $OPTIONS||{

echo "gecko build failed"
exit -2
}
valgrind  --leak-check=full --show-leak-kinds=all --read-var-info=yes ./gecko