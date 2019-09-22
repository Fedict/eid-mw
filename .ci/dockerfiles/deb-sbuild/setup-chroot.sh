#!/bin/bash

set -e
set -x

CODE=shift
ARCH=shift
DIST=shift
case $DIST in
	debian)
		/usr/local/bin/download-debuerreotype-tarball.sh $CODE $ARCH
	;;
	ubuntu)
		mkdir -p /srv/chroot
		sbuild-createchroot --arch=$ARCH $CODE /srv/chroot/$CODE https://archive.ubuntu.com/ubuntu
	;;
	*)
		echo "E: unknown dist: $DIST" >&2
		exit 1
	;;
esac
