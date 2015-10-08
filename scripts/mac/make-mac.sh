#!/bin/bash

set -e


# Ensure we're in the right directory
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
xcodebuild -project beidmw.xcodeproj clean
xcodebuild -project beidmw.xcodeproj -target beidpkcs11 -configuration Release
popd


pushd "../../../plugins_tools/aboutmw/OSX/eID Middleware"
xcodebuild -project "eID Middleware.xcodeproj" clean
xcodebuild -project "eID Middleware.xcodeproj" -target "eID Middleware" -configuration Release
popd


pushd "../../../plugins_tools/plistMerger"
xcodebuild -project "plistmerger.xcodeproj" clean
xcodebuild -project "plistmerger.xcodeproj" -target "plistMerger" -configuration Release
popd

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
