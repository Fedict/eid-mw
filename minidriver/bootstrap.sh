#!/bin/sh
libtoolize
autoconf
automake --add-missing
autoreconf -i
