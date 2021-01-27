#!/bin/sh

GITDESC=$(git describe --match="v5*" --dirty 2>/dev/null | sed -e 's/-/./g')
VERCLEAN=$(echo $GITDESC|sed -Ee 's/^.*v([0-9]+\.[0-9]+(\.[0-9]+|bp|\.pre[0-9]*)).*$/\1/')
FORCE_VERSION=${FORCE_VERSION:-0}

GITDESC="$VERCLEAN-$GITDESC"

if [ "$GITDESC" = "-" ]; then
	if [ -f ".version" ]; then
		GITDESC=$(cat .version)
	else
		if [ $FORCE_VERSION -gt 0 ]; then
			git fetch --all
			export FORCE_VERSION=0
			exec $0 "$@"
		fi
		GITDESC="0.unknown"
	fi
fi

echo $GITDESC > .version
echo $GITDESC
