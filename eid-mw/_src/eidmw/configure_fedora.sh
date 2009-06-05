#!/bin/sh
##########################
# script to configure the eID MW build process
# on fedora 9/10
# Run this script after setting the $PATH variable
##########################
QT4DIR=`stat -c %n /usr/local/Trolltech/Qt*/bin 2> /dev/null` 
if [ $QT4DIR != "" ]
then
./configure --lib+=-L${QT4DIR}/../lib --lib+=-L/usr/lib/xerces-c-2.7.0 --include+=/usr/include/xercesc-2.7.0 --include+=${QT4DIR}/../include/Qt BUILD_SDK=1
else
./configure --lib+=-L/usr/lib/xerces-c-2.7.0 --include+=/usr/include/xercesc-2.7.0 BUILD_SDK=1
fi
