#!/bin/bash

set -e
set -x

#installer name defines
#release dir, where all files to be released will be placed
RELEASE_DIR="$(pwd)/release"
#root dir, for files that are to be installed by the pkg
ROOT_DIR="$RELEASE_DIR/root"
#resources dir, for files that are to be kept inside the pkg
RESOURCES_DIR="$RELEASE_DIR/resources"
#install scripts dir, where the install scripts are that will be executed by the package
INSTALL_SCRIPTS_DIR="$RELEASE_DIR/install_scripts"


#inst dir, where our libs and binaries will be placed
INST_DIR=/usr/local
#licenses dir, where our licences will be placed
LICENSES_DIR="/Library/BelgiumIdentityCard/Licenses"


#base name of the package
REL_NAME="beid"
#version number of the package
#REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
#REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')
REL_VERSION="4.1.3"

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
PKG_MAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
VOL_NAME="${REL_NAME} OSX ${REL_VERSION}"
DMG_NAME="${REL_NAME}_${REL_VERSION}.dmg"


XPI_PLUGIN_DIR="/Library/Application\ Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/belgiumeid@eid.belgium.be"


#cleanup previous build

cleanup() {
test -e $RELEASE_DIR && rm -rdf $RELEASE_DIR
}

trap cleanup EXIT


#create installer dirs
mkdir -p $ROOT_DIR/$INST_DIR/lib
mkdir -p $ROOT_DIR/$LICENSES_DIR
mkdir -p $RESOURCES_DIR
mkdir -p $INSTALL_SCRIPTS_DIR

#copy all files that should be part of the installer:
cp ../../../output/lib/Release/libbeidpkcs11.$REL_VERSION.dylib $ROOT_DIR/$INST_DIR/lib/
#copy pkcs11 bundle
#cp -r ../../../misc/mac/pkcs11.bundle $RESOURCES_DIR
#$ROOT_DIR/$INST_DIR/lib/$PKCS11_BUNDLE

#copy licenses
cp ../../../doc/licenses/Dutch/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_NL.txt ; \
cp ../../../doc/licenses/English/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_EN.txt ; \
cp ../../../doc/licenses/French/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_FR.txt ; \
cp ../../../doc/licenses/German/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_DE.txt ; \
cp ../../../doc/licenses/THIRDPARTY-LICENSES-Mac.txt $ROOT_DIR/$LICENSES_DIR/


cp -r ./resources $RESOURCES_DIR

LATEST_XPI=$(readlink ../../../plugins_tools/xpi/builds/belgiumeid-CURRENT.xpi)
XPI_PLUGIN=../../../plugins_tools/xpi/builds/$LATEST_XPI
cp $XPI_PLUGIN $RESOURCES_DIR

cp -r ../../../cardcomm/tokend/BEID_Lion.tokend $RESOURCES_DIR

#copy certificates
cp ../../../installers/certificates/beid-cert-belgiumrca2.der $RESOURCES_DIR
cp ../../../installers/certificates/beid-cert-belgiumrca3.der $RESOURCES_DIR

cp -r ./install_scripts $INSTALL_SCRIPTS_DIR
	 

#####################################################################
################################################################

echo "********** generate $PKG_NAME and $DMG_NAME **********"

#chmod g+w $ROOT_DIR/$INST_DIR
#chmod g+w $ROOT_DIR/$INST_DIR/lib
#chmod a-x $ROOT_DIR/$INST_DIR/etc/beid.conf
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/Info.plist
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/PkgInfo
chgrp    wheel  $ROOT_DIR/usr
chgrp    wheel  $ROOT_DIR/usr/local
chgrp    wheel  $ROOT_DIR/usr/local/lib
chgrp -R admin  $RESOURCES_DIR/BEID_Lion.tokend

pkgbuild --root $ROOT_DIR --scripts INSTALL_SCRIPTS_DIR --identifier eid.belgium.be --version $REL_VERSION --install-location / $PKG_NAME

#$PKG_MAKER -r $ROOT_DIR -o $PKG_NAME -f $INFO_DIR/Info.plist \
#	-e $RESOURCES_DIR -s $INSTALL_SCRIPTS_DIR -n REL_VERSION

hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
