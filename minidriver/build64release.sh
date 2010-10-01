#!/bin/sh

mingwbin=/c/eid_dev_env/mingw64-64/bin

# 64 bit debug build
make clean
export PATH=$mingwbin:$PATH
./configure --host=x86_64-w64-mingw32 --prefix=`pwd` && make && make install || exit
