#!/bin/sh

GITDESC=$(git describe --match="v5*" --dirty 2>/dev/null | sed -e 's/-/./g')
VERCLEAN=$(echo $GITDESC|sed -Ee 's/^.*v([0-9]+\.[0-9]+(\.[0-9]+|bp|\.pre[0-9]*)).*$/\1/')
FAIL=$1

GITDESC="$VERCLEAN-$GITDESC"

if [ "$GITDESC" = "-" ]; then
	if [ -f ".version" ]; then
		GITDESC=$(cat .version)
	else
		if [ -z "$FAIL" ]; then
			GITDESC="0.unknown"
		else
			exit 1
		fi
	fi
fi

echo $GITDESC > .version
echo $GITDESC
