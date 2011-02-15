#!/bin/sh


# Figure out version numbers:
# SVN
# Can't use svn_revision, since that is only rebuilt as part of the
# build step below (which means it currently contains the 'old' version
# number)
SVNREV=$(cat svn_revision)
# Upstream
MAINVER=$(./configure --version|grep ^eid-mw|cut -d' ' -f3)

# Generate changelog entry with correct version number
dch -v ${MAINVER}r${SVNREV} "Snapshot build"

# Build
make distclean
debuild -uc -us -i -I.svn -b
STATE=$?

# Reset changelog to keep working directory clean
#svn revert debian/changelog

exit $STATE
