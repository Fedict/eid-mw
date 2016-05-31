#!/bin/sh

cat <<EOF
#ifndef $1
#define $1

#define $2 \\
EOF
sed -e 's/\\/\\\\/g;s/"/\\"/g;s/^/"/;s/$/"\\/'
echo ""
echo "#endif"
