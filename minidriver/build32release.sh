#!/bin/sh

mingwbin=/c/mingw-w32/bin

# 32 bit release build
make clean
export PATH=$mingwbin:$PATH
./configure --host=i686-w64-mingw32 --prefix=`pwd` && make && make install || exit
