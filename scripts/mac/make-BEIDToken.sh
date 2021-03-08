#!/bin/bash

pushd $(dirname $0)

. set_eidmw_version.sh

rm -rf release-middleware
mkdir -p release-middleware
rm tmp-eidmiddleware.dmg
rm -f "eID Middleware-$REL_VERSION.dmg"

#pushd "../../cardcomm/ctkToken"
#xcodebuild -project "BEIDToken.xcodeproj" clean
#xcodebuild -project "BEIDToken.xcodeproj" -target "BEIDTokenApp" -configuration Release
#popd

hdiutil create -srcdir release-middleware -volname "eID Middleware" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 100m "tmp-eidmiddleware.dmg"
DEVNAME=$(hdiutil attach -readwrite -noverify -noautoopen "tmp-eidmiddleware.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
mkdir -p "/Volumes/eID Middleware/.background/"
cp -a ../../installers/eid-mw/mac/DD/bg.png "/Volumes/eID Middleware/.background/"
cp -Rf "../../cardcomm/ctkToken/build/Release/BEIDToken.app" "/Volumes/eID Middleware/"
ln -s /Applications "/Volumes/eID Middleware/ "
/usr/bin/osascript "../../installers/eid-mw/mac/DD/setlayout.applescript" "eID Middleware" || true
sleep 4
hdiutil detach $DEVNAME
hdiutil convert tmp-eidmiddleware.dmg -format UDBZ -o "eID Middleware-$REL_VERSION.dmg"
popd
