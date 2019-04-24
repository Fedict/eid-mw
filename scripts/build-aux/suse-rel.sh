#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -a DISTNAMES
declare -A VERARCHS

DISTVERS=([opensuse]="15.0 42.3")
DISTNAMES=([opensuse]="opensuse")
VERARCHS=([15.0]="x86_64" [42.3]="x86_64")
for dist in opensuse
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
		(cd /srv/repo/repo/rpm/$TARGET/$dist/$vers && gpg --yes --batch --passphrase "" --no-tty -b --armor repomd.xml)
	done
done
