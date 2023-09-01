#!/bin/sh

set -x
# set -e

rmmod -f mydev.ko
insmod mydev.ko

./writer daniel &
./reader 192.168.5.3 8888 /dev/mydev
