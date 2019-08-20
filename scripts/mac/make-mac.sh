#!/bin/bash

set -e

SIGN_BUILD=${SIGN_BUILD:-1}

if [ -z "$MAC_BUILD_CONFIG" ]
then
	MAC_BUILD_CONFIG=Release
	echo "running Release Build"
fi

if [ $SIGN_BUILD -ne 0 ]
then
	SIGNOPTS='CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED="NO" CODE_SIGN_ENTITLEMENTS="" CODE_SIGNING_ALLOWED="NO"'
fi

echo "MAC_BUILD_CONFIG is $MAC_BUILD_CONFIG"

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
echo "cleaning former project..."
xcodebuild $SIGNOPTS -project beidmw.xcodeproj -target beidpkcs11 -configuration $MAC_BUILD_CONFIG clean
xcodebuild $SIGNOPTS -project beidmw.xcodeproj -target beidpkcs11 -configuration $MAC_BUILD_CONFIG
popd

pushd "../../../cardcomm/ctkToken"
xcodebuild $SIGNOPTS -project "BEIDToken.xcodeproj" -configuration $MAC_BUILD_CONFIG clean
xcodebuild $SIGNOPTS -project "BEIDToken.xcodeproj" -target "BEIDTokenApp" -configuration $MAC_BUILD_CONFIG
popd

#-----------------------------------------
# create the pkg files
#-----------------------------------------
echo "creating the beid package..."
sudo SIGN_BUILD=$SIGN_BUILD MAC_BUILD_CONFIG=$MAC_BUILD_CONFIG ./create_package.sh

echo "[Info ] Done..."
