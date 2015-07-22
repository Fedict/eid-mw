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


#pkcs11_inst dir, where our pkcs11 lib will be placed
PKCS11_INST_DIR="$ROOT_DIR/usr/local/lib"
#licenses dir, where our licences will be placed
LICENSES_DIR="$ROOT_DIR/Library/Belgium Identity Card/Licenses"
#xpi plugin dir, where the xpi plugin will be placed
XPI_PLUGIN_DIR="$ROOT_DIR/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/belgiumeid@eid.belgium.be"
#tokend dir, where the BEID.tokend will be placed
TOKEND_DIR="$ROOT_DIR/Library/Security/tokend"

#base name of the package
REL_NAME="beid"
#version number of the package
#REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
#REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')
REL_VERSION="4.1.4"

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
VOL_NAME="${REL_NAME} OSX ${REL_VERSION}"
DMG_NAME="${REL_NAME}_${REL_VERSION}.dmg"





#cleanup previous build

#cleanup() {
test -e "$RELEASE_DIR" && rm -rdf "$RELEASE_DIR"
test -e beidbuild.pkg && rm beidbuild.pkg
test -e $PKG_NAME && rm $PKG_NAME
#}

#leave created dir there for now
#trap cleanup EXIT


#create installer dirs
mkdir -p "$PKCS11_INST_DIR"
mkdir -p "$LICENSES_DIR"
mkdir -p "$TOKEND_DIR"
mkdir -p "$XPI_PLUGIN_DIR"
mkdir -p "$RESOURCES_DIR"
mkdir -p "$INSTALL_SCRIPTS_DIR"

#copy all files that should be part of the installer:
cp ../../../Release/libbeidpkcs11.$REL_VERSION.dylib $PKCS11_INST_DIR
#copy pkcs11 bundle
#cp -r ../../../misc/mac/pkcs11.bundle $RESOURCES_DIR
#$ROOT_DIR/$INST_DIR/lib/$PKCS11_BUNDLE

#copy licenses
cp ../../../doc/licenses/Dutch/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_NL.txt" ; \
cp ../../../doc/licenses/English/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_EN.txt" ; \
cp ../../../doc/licenses/French/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_FR.txt" ; \
cp ../../../doc/licenses/German/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_DE.txt" ; \
cp ../../../doc/licenses/THIRDPARTY-LICENSES-Mac.txt "$LICENSES_DIR/"


cp -r ./resources/* $RESOURCES_DIR

#copy certificates to scripts folder, as they are only used during install (to trust them)
cp ../../../installers/certificates/beid-cert-belgiumrca2.der "$INSTALL_SCRIPTS_DIR"
cp ../../../installers/certificates/beid-cert-belgiumrca3.der "$INSTALL_SCRIPTS_DIR"


LATEST_XPI=$(readlink ../../../plugins_tools/xpi/builds/belgiumeid-CURRENT.xpi)
XPI_PLUGIN=../../../plugins_tools/xpi/builds/$LATEST_XPI
cp $XPI_PLUGIN "$XPI_PLUGIN_DIR"

cp -r ../../../cardcomm/tokend/BEID_Lion.tokend "$TOKEND_DIR/BEID.tokend"

cp -r ./install_scripts/* "$INSTALL_SCRIPTS_DIR"
	 
#copy distribution file
cp ./Distribution.txt "$RELEASE_DIR"

#####################################################################

echo "********** generate $PKG_NAME and $DMG_NAME **********"

#chmod g+w $ROOT_DIR/$INST_DIR
#chmod g+w $ROOT_DIR/$INST_DIR/lib
#chmod a-x $ROOT_DIR/$INST_DIR/etc/beid.conf
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/Info.plist
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/PkgInfo
chgrp    wheel  "$ROOT_DIR/usr"
chgrp    wheel  "$ROOT_DIR/usr/local"
chgrp    wheel  "$ROOT_DIR/usr/local/lib"
chgrp -R admin  "$TOKEND_DIR/BEID.tokend"

#build the packages in the release dir
pushd $RELEASE_DIR
pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild.pkg

productbuild --distribution "$RELEASE_DIR/Distribution.txt" --resources "$RESOURCES_DIR" $PKG_NAME

hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
popd
