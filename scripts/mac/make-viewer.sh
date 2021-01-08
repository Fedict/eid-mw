#!/bin/bash

set -e

if [ -z "$MAC_BUILD_CONFIG" ]
then
	MAC_BUILD_CONFIG=Release
fi

pushd $(dirname $0)

. set_eidmw_version.sh

rm -rf release-viewer
mkdir -p release-viewer
rm -f tmp-eidviewer.dmg
rm -f "eID Viewer-$REL_VERSION.dmg"

pushd "../../"
pushd "plugins_tools/eid-viewer/OSX/support"
pushd "ossl"
make clean-builds
popd
pushd "xml"
make clean-builds
popd
popd
xcodebuild -project "beidmw.xcodeproj" -target "eID Viewer" -configuration $MAC_BUILD_CONFIG clean
xcodebuild -project "beidmw.xcodeproj" -target "eID Viewer" -configuration $MAC_BUILD_CONFIG
popd

hdiutil create -srcdir release-viewer -volname "eID Viewer" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 100m "tmp-eidviewer.dmg"
DEVNAME=$(hdiutil attach -readwrite -noverify -noautoopen "tmp-eidviewer.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
mkdir -p "/Volumes/eID Viewer/.background/"
cp -a ../../installers/eid-viewer/mac/bg.png "/Volumes/eID Viewer/.background/"
cp -a "../../$MAC_BUILD_CONFIG/eID Viewer.app" "/Volumes/eID Viewer/"
ln -s /Applications "/Volumes/eID Viewer/ "
/usr/bin/osascript "../../installers/eid-viewer/mac/setlayout.applescript" "eID Viewer" || true
for i in $(seq 1 10); do
	sleep 10
	if hdiutil eject ${DEVNAME}s1; then
		hdiutil convert tmp-eidviewer.dmg -format UDBZ -o "eID Viewer-$REL_VERSION.dmg"
		exit 0
	fi
done
exit 1
