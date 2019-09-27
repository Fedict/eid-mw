#!/bin/bash

set -e

EIDMW_BUILD_CONFIG=Export

# Go to the mac installers directory
cd $(dirname $0)/../../installers/eid-mw/mac

#-----------------------------------------
# make sure scripts are executable
#-----------------------------------------
CURRDIR=`pwd`
chmod +x ./create_export_package.sh

#-----------------------------------------
# build the eID MW
#-----------------------------------------

pushd ../../..
echo "creating beidpkcs11"
xcodebuild -project beidmw.xcodeproj -target "beidpkcs11" -configuration Release clean build
popd

pushd "../../../cardcomm/ctkToken"
xcodebuild -project "BEIDToken.xcodeproj" -scheme "BEIDTokenApp" -configuration Release clean archive
popd

#-----------------------------------------
# create the pkg files
#-----------------------------------------
echo "creating the beid package..."
source ./create_export_package.sh

echo "[Info ] Done..."
