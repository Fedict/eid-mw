#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -a DISTNAMES
declare -A VERARCHS

DISTS=(opensuse)

DISTVERS=([opensuse]="15.0 42.3")
DISTNAMES=([opensuse]="opensuse")
VERARCHS=([15.0]="x86_64" [42.3]="x86_64")

if [ ! -z "$1" ]
then
	if [ -z "$EXTRADIST" -o -z "$EXTRAVER" ]
	then
		echo "E: extra packages but EXTRADIST or EXTRAVER is not set" >&2
		exit 1
	fi
	DISTS=($1)
	DISTVERS[$1]=$EXTRAVER
	VERARCHS[$1]="x86_64"
fi

for dist in $DISTS
do
	for vers in ${DISTVERS[$dist]}
	do
		for arch in ${VERARCHS[$vers]}
		do
			mkdir -p /srv/repo/repo/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch
			for i in products/$dist-$vers-$arch/*
			do
				targetfile=/srv/repo/repo/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch/$(basename $i)
				echo "$i => $targetfile"
				cp $i $targetfile
				rpmsign --resign --key-id=$GPG_TEST_KEY_ID $targetfile
			done
		done
		createrepo /srv/repo/repo/rpm/$TARGET/$dist/$vers
		(cd /srv/repo/repo/rpm/$TARGET/$dist/$vers/repodata && gpg --yes --batch --passphrase "" --default-key $GPG_TEST_KEY_ID --no-tty -b --armor repomd.xml)
	done
done
