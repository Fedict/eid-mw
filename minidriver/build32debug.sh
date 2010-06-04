#!/bin/sh

mingwbin=/c/mingw-w32/bin

# 32 bit debug build
make clean
export PATH=$mingwbin:$PATH
./configure --host=i686-w64-mingw32 --prefix=`pwd` --enable-debug && make && make install || exit


