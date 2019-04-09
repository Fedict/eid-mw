#!/bin/bash

set -e

find ./products -print

declare -A ARCHMAP
declare -A DISTS
declare -A VERARCHS

DISTS=([fedora]="28 29" [epel]="6 7")
VERARCHS=([6]="i386 x86_64" [7]="x86_64" [28]="i386 x86_64" [29]="i386 x86_64")
for dist in fedora epel
do
	for vers in ${DISTS[$dist]}
	do
		for arch in ${VERARCHS[$vers]}
		do
			mkdir -p /srv/repo/repo/rpm/$TARGET/$dist/$vers/RPMS/$arch
			for i in products/$dist-$vers-$arch/*
			do
				targetfile=/srv/repo/repo/rpm/$TARGET/$dist/$vers/RPMS/$arch/$(basename $i)
				echo "$i => $targetfile"
				cp $i $targetfile
				rpmsign --resign --key-id=$GPG_TEST_KEY_ID $targetfile
			done
		done
		createrepo /srv/repo/repo/rpm/$TARGET/$dist/$vers
	done
done
