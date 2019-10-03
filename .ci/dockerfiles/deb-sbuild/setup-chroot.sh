#!/bin/bash

set -e
set -x

if [ -z "$CODE" ] && [ ! -z "$1" ]
then
	CODE=$1
fi
shift
if [ -z "$ARCH" ] && [ ! -z "$1" ]
then
	ARCH=$1
fi
shift
if [ -z "$DIST" ] && [ ! -z "$1" ]
then
	DIST=$1
fi
shift
case $DIST in
	debian)
		/usr/local/bin/download-debuerreotype-tarball.sh $CODE $ARCH
	;;
	ubuntu)
		mkdir -p /srv/chroot
		sbuild-createchroot --arch=$ARCH $CODE /srv/chroot/$CODE http://archive.ubuntu.com/ubuntu /usr/share/debootstrap/scripts/gutsy
	;;
	*)
		echo "E: unknown dist: $DIST" >&2
		exit 1
	;;
esac

schroot -c source:$CODE-$ARCH-sbuild apt-get update
schroot -c source:$CODE-$ARCH-sbuild apt-get -y install eatmydata
echo "command-prefix=eatmydata" >> /etc/schroot/chroot.d/*
