#!/bin/sh

cat <<EOF
#ifndef ABOUT_GLADE_H
#define ABOUT_GLADE_H

#define ABOUT_GLADE_STRING \\
EOF
sed -e 's/"/\\"/g;s/^/"/;s/$/"\\/'
echo ""
echo "#endif"
