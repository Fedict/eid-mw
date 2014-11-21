#!/bin/sh


# Figure out version numbers:
# SVN
# Can't use gitrev since that is only rebuilt as part of the
# build step below (which means it currently contains the 'old' version
# number)
GITREV=$(cat gitrev)
# Upstream
MAINVER=$(./configure --version|grep ^eid-mw|cut -d' ' -f3)
# Distribution name
DIST=$(lsb_release -c -s)

# Generate changelog entry with correct version number, suppressing a possible
# warning from dch that the distribution name may or may not be valid.
# This would be true for an official upload, but this isn't that.
yes | dch -v ${MAINVER}${GITDATE}-0${DIST}1 -D${DIST} "Snapshot build"

# Build
make distclean
debuild -uc -us -i -I.svn -b
STATE=$?

# Reset changelog to keep working directory clean
#svn revert debian/changelog

exit $STATE
