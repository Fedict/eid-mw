#!/bin/sh

# This script makes sure there are no local source diffs not accounted for by a patch file in the patches directory.

if [ "$1" == "clean" ]; then
   # nothing to do
   exit 0
fi

SCRIPTDIR=$( dirname "$0" )
"$SCRIPTDIR"/generate-patch.sh /tmp/$$.unaccounted.patch

#if [ -e "/tmp/$$.unaccounted.patch" ]; then
#   echo "/tmp/$$.unaccounted.patch:0: error: local unaccounted source diffs"
#   if [ "$RC_XBS" != "" ]; then
#      exit 1
#   fi
#fi

PLISTVER=$( /usr/libexec/PlistBuddy -c 'Print :OpenSourceVersion' "$SCRIPTDIR"/../OpenSSL101j.plist )
SRCVER=$( cat "$SCRIPTDIR"/../src/.version )

if [ "$PLISTVER" != "$SRCVER" ]; then
   LINENUM=$( grep -n OpenSourceVersion "$SCRIPTDIR/../OpenSSL101j.plist" | awk -F: '{print $1}' )
   echo "$SCRIPTDIR/../OpenSSL101j.plist:$LINENUM: error: OpenSourceVersion is wrong"
   if [ "$RC_XBS" != "" ]; then
      exit 1
   fi
fi