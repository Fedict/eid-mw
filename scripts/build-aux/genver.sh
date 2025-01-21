#!/bin/sh

get_from_dotfile() {
	if [ -f ".version" ]; then
		cat .version
	else
		if [ $FORCE_VERSION -gt 1 ]; then
			>&2 echo "ERROR: Cannot detect version: not a git repository, and no .version file is provided."
			sleep 10
			exit 1
		else
			echo "0.0.unknown"
			>&2 echo "WARNING: could not detect package version. Defaulting to 0.0.unknown!"
		fi
	fi
}

if ! command -v git 2>&1 >/dev/null; then
	# no git binary available -> rely on .version file, or bail out
	GITDESC="$(get_from_dotfile)"
else
	GITDESC="$(git describe --match="v5*" --all --dirty | sed -e 's,^tags/,,g;s/-/./g')"
	if [ $? != 0 ]; then
		GITDESC="$(get_from_dotfile)"
	else
		VERCLEAN="$(echo "$GITDESC"|sed -Ee 's/^.*v([0-9]+\.[0-9]+(\.[0-9]+|bp|\.pre[0-9]*)).*$/\1/')"
		FORCE_VERSION="${FORCE_VERSION:-0}"

		GITDESC="$VERCLEAN-$GITDESC"

		if [ "$GITDESC" = "-" ]; then
			GITDESC="$(get_from_dotfile)"
		fi
	fi
fi

echo "$GITDESC" > .version
echo "$GITDESC"
