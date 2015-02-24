#!/bin/bash

set -e



# Ensure we're in the right directory
cd $(dirname $0)/../../installers/eid-mw/mac

#########################################
# check gcc compiler installed
#########################################
GCC=`which gcc 2> /dev/null | wc -l`
echo "[Info ] Verifying gcc compiler..."
if [[ $GCC -eq 0 ]]
then
	echo "[Error] gcc compiler not found. Please verify or install."
	exit -1

else
	GCCVER=`gcc --version`
	echo "[Info ] Installed. gcc version: $GCCVER"
fi

#########################################
# check g++ compiler installed
#########################################
echo "[Info ] Verifying g++ compiler..."
GPP=`which g++ 2> /dev/null | wc -l`
if [ $GPP -eq 0  ]
then
	echo "[Error] g++ compiler not found. Please verify or install."
	exit -1
else
	GPPVER=`g++ --version`
	echo "[Info ] Installed. g++ version: $GPPVER"
fi

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


#-----------------------------------------
# Unpack the BEID_Lion.tokend and copy to bin
# this is the tokend version for Lion
#-----------------------------------------
pushd ../../../cardcomm/tokend 
if [ -d ./BEID_Lion.tokend ]
then
	sudo rm -rf ./BEID_Lion.tokend
fi
echo "Unpacking BEID_Lion.tokend..."
tar -xvf ./BEID_Lion.tokend.tar.gz

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
