#!/bin/sh
# Run configure script.
cd "${PROJECT_DIR}"
./configure --prefix="${PROJECT_DIR}/../libgmp"
make
make install

