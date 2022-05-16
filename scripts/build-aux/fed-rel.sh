#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -A DISTNAMES
declare -A DISTARCHS

DISTS="fedora epel"

DISTVERS=([fedora]="$FEDORA_OLDSTABLE_VERSION $FEDORA_STABLE_VERSION" [epel]="$EL_OLDSTABLE_VERSION $EL_STABLE_VERSION")
DIST_TARGET_NAMES=([fedora]="fedora" [epel]="el")
DISTARCHS=([fedora]="i386 x86_64" [epel]="x86_64")
DIST_SOURCE_NAMES=(["epel:$EL_OLDSTABLE_VERSION"]="$EL_OLDSTABLE_DIST" ["epel:$EL_STABLE_VERSION"]="$EL_STABLE_DIST" ["fedora:$FEDORA_STABLE_VERSION"]="fedora" ["fedora:$FEDORA_OLDSTABLE_VERSION"]="fedora")

for dist in $DISTS
do
	for vers in ${DISTVERS[$dist]}
	do
		for arch in ${DISTARCHS[$dist]}
		do
			mkdir -p $REPO_LOC/rpm/$TARGET/${DIST_TARGET_NAMES[$dist]}/$vers/RPMS/$arch
			if [ -d products/${DIST_SOURCE_NAMES["$dist:$vers"]}-$vers-$arch ]
			then
				for i in products/${DIST_SOURCE_NAMES["$dist:$vers"]}-$vers-$arch/*rpm
				do
					targetfile=$REPO_LOC/rpm/$TARGET/${DIST_TARGET_NAMES[$dist]}/$vers/RPMS/$arch/$(basename $i)
					cp $i $targetfile
					rpmsign --resign --key-id=$GPG_TEST_KEY_ID $targetfile
				done
			fi
		done
		createrepo_c $REPO_LOC/rpm/$TARGET/${DIST_TARGET_NAMES[$dist]}/$vers
	done
done
