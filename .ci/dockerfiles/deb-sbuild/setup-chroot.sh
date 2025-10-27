#!/bin/bash

set -e
set -x

if [ -z "$CODE" ] && [ ! -z "$1" ]
then
        CODE=$1
        shift
fi
if [ -z "$ARCH" ] && [ ! -z "$1" ]
then
        ARCH=$1
        shift
fi
if [ -z "$DIST" ] && [ ! -z "$1" ]
then
        DIST=$1
        shift
fi
mkdir -p /srv/chroot
if [ -z $ACNG ]
then
        ACNG=http://
fi
if [ -z "$TARGET" ] && [ ! -z "$1" ]
then
        TARGET=$1
        shift
fi
if [ ! -z "$TARGET" ]
then
        fulldist=$TARGET-$CODE
else
        fulldist=$CODE
fi

mkdir -p /root/.cache/sbuild
case $DIST in
        debian)
                mirror=${ACNG}deb.debian.org/debian
                components=main
                keyring=/usr/share/keyrings/debian-archive-keyring.gpg
        ;;
        ubuntu)
                components=main,universe
                keyring=/usr/share/keyrings/ubuntu-archive-keyring.gpg
                if [ "$ARCH" == "armhf" ] || [ "$ARCH" == "arm64" ]
                then
                        mirror=${ACNG}ports.ubuntu.com/
                else
                        mirror=${ACNG}archive.ubuntu.com/ubuntu
                fi
        ;;
        *)
                echo "E: unknown dist: $DIST" >&2
                exit 1
        ;;
esac

mmdebstrap --include=debhelper --variant=buildd --arch=$ARCH --skip=output/mknod --components=$components --keyring=$keyring $CODE /srv/chroot $mirror
sed -i -e "s/my @dpkg_options;/my @dpkg_options = ('-Zxz');/" /srv/chroot/usr/bin/dh_builddeb
sbuild-createchroot --setup-only --arch=$ARCH $CODE-$ARCH-sbuild /srv/chroot || true
cat > /srv/chroot/etc/apt/sources.list <<EOF
deb $mirror $code $components
deb-src $mirror $code $components
EOF
sed -i -e "s/,/ /g" /srv/chroot/etc/apt/sources.list
