#!/bin/bash

set -e


# Go to the mac installers directory
cd $(dirname $0)/../../installers/eid-mw/mac

#-----------------------------------------
# make sure scripts are executable
#-----------------------------------------
CURRDIR=`pwd`
chmod +x ./create_package.sh

#-----------------------------------------
# build the eID MW
#-----------------------------------------

pushd ../../..
echo "creating beidpkcs11"
xcodebuild -project beidmw.xcodeproj -scheme "beidpkcs11" -configuration Release clean archive
popd

pushd "../../../cardcomm/ctkToken"
xcodebuild -project "BEIDToken.xcodeproj" -scheme "BEIDTokenApp" -configuration Release clean archive
popd

#-----------------------------------------
# create the pkg files
#-----------------------------------------
echo "creating the beid package..."
sudo ./create_export_package.sh

echo "[Info ] Done..."
