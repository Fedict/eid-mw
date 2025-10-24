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

mmdebstrap --variant=buildd --arch=$ARCH --skip=output/mknod --format=tar --components=$components --keyring=$keyring $CODE /root/.cache/sbuild/$fulldist-$ARCH.tar $mirror
