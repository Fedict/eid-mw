#!/bin/bash

set -e

SIGN_BUILD=${SIGN_BUILD:-1}

# Go to the mac installers directory
cd $(dirname $0)/../../installers/eid-mw/mac

if [ $SIGN_BUILD -ne 0 ]
then
	SIGNOPTS='CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED="NO" CODE_SIGN_ENTITLEMENTS="" CODE_SIGNING_ALLOWED="NO"'
fi

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
xcodebuild $SIGNOPTS-project beidmw.xcodeproj -target "beidpkcs11" -configuration Release clean build
popd

pushd "../../../cardcomm/ctkToken"
xcodebuild $SIGNOPTS -project "BEIDToken.xcodeproj" -scheme "BEIDTokenApp" -configuration Release clean archive
popd

#-----------------------------------------
# create the pkg files
#-----------------------------------------
echo "creating the beid package..."
sudo ./create_export_package.sh

echo "[Info ] Done..."
