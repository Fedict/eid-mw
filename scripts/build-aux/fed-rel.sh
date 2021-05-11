#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -A DISTNAMES
declare -A DISTARCHS

DISTS="fedora epel"

DISTVERS=([fedora]="$FEDORA_OLDSTABLE_VERSION $FEDORA_STABLE_VERSION" [epel]="$CENTOS_OLDSTABLE_VERSION $CENTOS_STABLE_VERSION")
DISTNAMES=([fedora]="fedora" [epel]="el")
DISTARCHS=([fedora]="i386 x86_64" [epel]="x86_64")

for dist in $DISTS
do
	for vers in ${DISTVERS[$dist]}
	do
		for arch in ${DISTARCHS[$dist]}
		do
			mkdir -p $REPO_LOC/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch
			if [ -d products/$dist-$vers-$arch ]
			then
				for i in products/$dist-$vers-$arch/*rpm
				do
					targetfile=$REPO_LOC/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch/$(basename $i)
					echo "$i => $targetfile"
					cp $i $targetfile
					rpmsign --resign --key-id=$GPG_TEST_KEY_ID $targetfile
				done
			fi
		done
		createrepo $REPO_LOC/rpm/$TARGET/${DISTNAMES[$dist]}/$vers
	done
done
