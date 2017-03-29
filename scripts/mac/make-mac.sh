#!/bin/bash

set -e


# Go to the mac installers directory
cd $(dirname $0)/../../installers/eid-mw/mac

#-----------------------------------------
# make sure scripts are executable
#-----------------------------------------
CURRDIR=`pwd`
chmod +x ./create_package.sh
chmod +x ../../../plugins_tools/xpi/build.sh

#-----------------------------------------
# build the eID MW
#-----------------------------------------

pushd ../../..
echo "cleaning former project..."
xcodebuild -project beidmw.xcodeproj -target beidpkcs11 clean
xcodebuild -project beidmw.xcodeproj -target beidpkcs11 -configuration Release

# store the SDK version (with own dialogs)


xcodebuild -project beidmw.xcodeproj -target beid_ff_pkcs11 clean
xcodebuild -project beidmw.xcodeproj -target beid_ff_pkcs11 -configuration Release GCC_PREPROCESSOR_DEFINITIONS_NOT_USED_IN_PRECOMPS='NO_DIALOGS=1' 
popd


pushd "../../../plugins_tools/aboutmw/OSX/eID Middleware"
xcodebuild -project "eID Middleware.xcodeproj" clean
xcodebuild -project "eID Middleware.xcodeproj" -target "eID Middleware" -configuration Release
popd

#pushd "../../../plugins_tools/eid-viewer/OSX/eID Viewer"
#xcodebuild -project "eID Viewer.xcodeproj" clean
#xcodebuild -project "eID Viewer.xcodeproj" -target "eID Viewer" -configuration Release
#popd


#pushd "../../../plugins_tools/plistMerger"
#xcodebuild -project "plistmerger.xcodeproj" clean
#xcodebuild -project "plistmerger.xcodeproj" -target "plistMerger" -configuration Release
#popd

#-----------------------------------------
# create the xpi
#-----------------------------------------
echo "creating the XPI..."
pushd ../../../plugins_tools/xpi
./build.sh -f
popd

#-----------------------------------------
# create a pkg file
#-----------------------------------------
echo "creating the beid package..."
sudo ./create_package.sh

echo "[Info ] Done..."
