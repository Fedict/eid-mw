#!/bin/bash

set -e

if [ -z "$EIDMW_BUILD_CONFIG" ]
then
	EIDMW_BUILD_CONFIG=Release
	echo "running Release Build"
fi
#set EIDMW_SIGN_BUILD=0 in the environment to not sign the .pkg files:
# EIDMW_SIGN_BUILD=0 ./create_package.sh
#or
# EIDMW_SIGN_BUILD=0 ./make-mac.sh
EIDMW_SIGN_BUILD=${EIDMW_SIGN_BUILD:-1}
if [ -z "$EIDMW_SIGN_BUILD" ]
then
	EIDMW_SIGN_BUILD=1
	echo "will be signing Release Build"
fi


echo "EIDMW_BUILD_CONFIG is $EIDMW_BUILD_CONFIG"

# Go to the mac installers directory
cd $(dirname $0)/../../installers/eid-mw/mac

#-----------------------------------------
# make sure scripts are executable
#-----------------------------------------
chmod +x ./create_package.sh

#-----------------------------------------
# build the eID MW
#-----------------------------------------

pushd ../../..
echo "cleaning former project..."
xcodebuild -project beidmw.xcodeproj -target beidpkcs11 -configuration $EIDMW_BUILD_CONFIG clean
xcodebuild -project beidmw.xcodeproj -target beidpkcs11 -configuration $EIDMW_BUILD_CONFIG
popd

pushd "../../../cardcomm/ctkToken"
xcodebuild -project "BEIDToken.xcodeproj" -configuration $EIDMW_BUILD_CONFIG clean
xcodebuild -project "BEIDToken.xcodeproj" -target "BEIDTokenApp" -configuration $EIDMW_BUILD_CONFIG
popd

#-----------------------------------------
# create the pkg files
#-----------------------------------------
echo "creating the beid package..."
source ./create_package.sh

echo "[Info ] Done..."
