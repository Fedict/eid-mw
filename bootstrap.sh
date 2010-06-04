#!/bin/sh
libtoolize --copy --force
autoconf
automake --add-missing
autoreconf -i
