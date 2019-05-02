#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -A DISTNAMES
declare -A VERARCHS

DISTS=(fedora epel)

DISTVERS=([fedora]="30 29" [epel]="6 7")
DISTNAMES=([fedora]="fedora" [epel]="el")
VERARCHS=([6]="i386 x86_64" [7]="x86_64" [30]="i386 x86_64" [29]="i386 x86_64")

if [ ! -z "$1" ]
then
	if [ -z "$EXTRADIST" -o -z "$EXTRAVER" ]
	then
		echo "E: extra packages but EXTRADIST or EXTRAVER is not set" >&2
		exit 1
	fi
	DISTS=($1)
	DISTVERS[$1]=$EXTRAVER
	VERARCHS[$1]="x86_64 i386"
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
		createrepo /srv/repo/repo/rpm/$TARGET/${DISTNAMES[$dist]}/$vers
	done
done
