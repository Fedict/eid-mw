#!/bin/sh

GITDESC=$(git describe --dirty|sed -e 's/-/./g' 2>/dev/null)

if [ -z "$GITDESC" ]; then
	GITDESC="0.unknown"
fi

echo $GITDESC
