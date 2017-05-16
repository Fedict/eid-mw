#!/bin/bash

. set_eidmw_version.sh

hdiutil convert "eID Viewer-$REL_VERSION.dmg" -format UDRW -o "tmp-eidviewer.dmg"
DEVNAME=$(hdiutil attach -readwrite -noverify -noautoopen "tmp-eidviewer.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
codesign -s "Developer ID Application" "/Volumes/eID Viewer/eID Viewer.app/Contents/Library/QuickLook/eidfilepreview.qlgenerator/"
codesign -s "Developer ID Application" "/Volumes/eID Viewer/eID Viewer.app/Contents/Frameworks/BeidView.framework/Versions/A/Frameworks/libbeidpkcs11.$REL_VERSION.dylib"
codesign -s "Developer ID Application" "/Volumes/eID Viewer/eID Viewer.app/Contents/Frameworks/BeidView.framework"
codesign -s "Developer ID Application" "/Volumes/eID Viewer/eID Viewer.app"
hdiutil detach $DEVNAME
hdiutil convert tmp-eidviewer.dmg -format UDBZ -o "eID Viewer-$REL_VERSION-signed.dmg"
