#!/bin/sh
LANG=$1
DIRNAME=`dirname $0`

echo $$

/usr/sbin/installer -pkg $DIRNAME/beid.pkg -target /

#beid.conf
cp -f $DIRNAME/beid.conf.$LANG /usr/local/etc/beid.conf
