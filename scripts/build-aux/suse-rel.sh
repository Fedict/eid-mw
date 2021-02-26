#!/bin/bash

set -e

find ./products -print

declare -A DISTVERS
declare -a DISTNAMES
declare -A VERARCHS

DISTS=(opensuse)

DISTVERS=([opensuse]="$OPENSUSE_OLDSTABLE_VERSION $OPENSUSE_STABLE_VERSION")
DISTNAMES=([opensuse]="opensuse")
DISTARCHS=([opensuse]="x86_64")

for dist in $DISTS
do
	for vers in ${DISTVERS[$dist]}
	do
		for arch in ${DISTARCHS[$dist]}
		do
			mkdir -p $REPO_LOC/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch
			if [ -d products/$dist-$vers-$arch ]
			then
				for i in products/$dist-$vers-$arch/*
				do
					targetfile=$REPO_LOC/rpm/$TARGET/${DISTNAMES[$dist]}/$vers/RPMS/$arch/$(basename $i)
					if [ "$(basename $i)" != '*' -a "$(basename $targetfile)" != '*' ]; then
						echo "$i => $targetfile"
						cp $i $targetfile
						rpmsign --resign --key-id=$GPG_TEST_KEY_ID $targetfile
					else
						echo "ignoring empty directories"
					fi
				done
			fi
		done
		createrepo $REPO_LOC/rpm/$TARGET/$dist/$vers
		(cd $REPO_LOC/rpm/$TARGET/$dist/$vers/repodata && gpg --yes --batch --passphrase "" --default-key $GPG_TEST_KEY_ID --no-tty -b --armor repomd.xml)
	done
done
