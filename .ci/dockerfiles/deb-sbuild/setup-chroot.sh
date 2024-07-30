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
mkdir -p /srv/chroot
if [ -z $ACNG ]
then
	ACNG=http://
fi
case $DIST in
	debian)
		sbuild-createchroot --arch=$ARCH $CODE /srv/chroot/$CODE ${ACNG}deb.debian.org/debian
	;;
	ubuntu)
		if [ "$ARCH" == "armhf" ] || [ "$ARCH" == "arm64" ]
		then
			sbuild-createchroot --include=debhelper --components=main,universe --arch=$ARCH $CODE /srv/chroot/$CODE ${ACNG}ports.ubuntu.com /usr/share/debootstrap/scripts/gutsy
		else
			sbuild-createchroot --include=debhelper --components=main,universe --arch=$ARCH $CODE /srv/chroot/$CODE ${ACNG}archive.ubuntu.com/ubuntu /usr/share/debootstrap/scripts/gutsy
		fi
		# Revert the "default to xz compression" option in recent
		# Ubuntu distributions
		sed -i -e "s/my @dpkg_options;/my @dpkg_options = ('-Zxz');/" /srv/chroot/$CODE/usr/bin/dh_builddeb
	;;
	*)
		echo "E: unknown dist: $DIST" >&2
		exit 1
	;;
esac
