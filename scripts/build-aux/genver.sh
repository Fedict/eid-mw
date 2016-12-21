#!/bin/sh

GITDESC=$(git describe --dirty|sed -e 's/-/./g' 2>/dev/null)
VERCLEAN=$(echo $GITDESC|sed -Ee 's/^.*v([0-9]+\.[0-9]+(\.[0-9]+|bp)).*$/\1/')

GITDESC="$VERCLEAN-$GITDESC"

if [ -z "$GITDESC" ]; then
	if [ -f ".version" ]; then
		GITDESC=$(cat .version)
	else
		GITDESC="0.unknown"
	fi
fi

echo $GITDESC > .version
echo $GITDESC
