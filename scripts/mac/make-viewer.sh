#!/bin/bash

if [ -z "$MAC_BUILD_CONFIG" ]
then
	MAC_BUILD_CONFIG=Release
fi

pushd $(dirname $0)

. set_eidmw_version.sh

rm -rf release-viewer
mkdir -p release-viewer
rm tmp-eidviewer.dmg
rm -f "eID Viewer-$REL_VERSION.dmg"

pushd "../../plugins_tools/eid-viewer/OSX/eID Viewer"
xcodebuild -project "eID Viewer.xcodeproj" clean
xcodebuild -project "eID Viewer.xcodeproj" -target "eID Viewer" -configuration $MAC_BUILD_CONFIG
popd

hdiutil create -srcdir release-viewer -volname "eID Viewer" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 100m "tmp-eidviewer.dmg"
DEVNAME=$(hdiutil attach -readwrite -noverify -noautoopen "tmp-eidviewer.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
mkdir -p "/Volumes/eID Viewer/.background/"
cp -a ../../installers/eid-viewer/mac/bg.png "/Volumes/eID Viewer/.background/"
cp -a "../../plugins_tools/eid-viewer/OSX/eID Viewer/build/$MAC_BUILD_CONFIG/eID Viewer.app" "/Volumes/eID Viewer/"
ln -s /Applications "/Volumes/eID Viewer/ "
/usr/bin/osascript "../../installers/eid-viewer/mac/setlayout.applescript" "eID Viewer" || true
sleep 4
hdiutil detach $DEVNAME
hdiutil convert tmp-eidviewer.dmg -format UDBZ -o "eID Viewer-$REL_VERSION.dmg"
popd
