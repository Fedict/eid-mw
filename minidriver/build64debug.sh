#!/bin/sh

mingwbin=/c/mingw-w64/bin

# 64 bit debug build
make clean
export PATH=$mingwbin:$PATH
./configure --host=x86_64-w64-mingw32 --prefix=`pwd` --enable-debug && make && make install || exit

