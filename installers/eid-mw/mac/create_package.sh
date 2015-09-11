#!/bin/bash

set -e
set -x

SIGN_BUILD=0
#set SIGN_BUILD=1 to sign the .pkg files

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
#plistmerger dir, where our plistmerger tool will be placed
PLISTMERGER_DIR="$ROOT_DIR/Library/Belgium Identity Card/plistMerger"
BEIDCARD_DIR="$ROOT_DIR/Library/Belgium Identity Card"
#xpi plugin dir, where the xpi plugin will be placed
#XPI_PLUGIN_DIR="$ROOT_DIR/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/belgiumeid@eid.belgium.be"
#tokend dir, where the BEID.tokend will be placed
TOKEND_DIR="$ROOT_DIR/Library/Security/tokend"

#eIDViewer path
EIDVIEWER_PATH="$(pwd)/../../../../ThirdParty/eid-viewer/eID Viewer.app"

#eIDMiddleware app path
EIDMIDDLEWAREAPP_PATH="$(pwd)/../../../plugins_tools/aboutmw/OSX/eID Middleware/Release/eID Middleware.app"

#base name of the package
REL_NAME="eID-Quickinstaller"
REL_NAME_DIAG="beid_diagnostic"
#version number of the package
#REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
#REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')
REL_VERSION="4.1.5"

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
PKGSIGNED_NAME="${REL_NAME}-signed.pkg"
VOL_NAME="${REL_NAME}-${REL_VERSION}"
DMG_NAME="${REL_NAME}-${REL_VERSION}.dmg"

PKG_NAME_DIAG="$REL_NAME_DIAG.pkg"
PKGSIGNED_NAME_DIAG="${REL_NAME_DIAG}-signed.pkg"
VOL_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}"
DMG_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}.dmg"

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
#mkdir -p "$XPI_PLUGIN_DIR"
mkdir -p "$RESOURCES_DIR"
mkdir -p "$INSTALL_SCRIPTS_DIR"
mkdir -p "$PLISTMERGER_DIR"

#copy all files that should be part of the installer:
cp ../../../Release/libbeidpkcs11.$REL_VERSION.dylib $PKCS11_INST_DIR
#copy pkcs11 bundle
cp -r ./Packages/beid-pkcs11.bundle $PKCS11_INST_DIR

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


#LATEST_XPI=$(readlink ../../../plugins_tools/xpi/builds/belgiumeid-CURRENT.xpi)
#XPI_PLUGIN=../../../plugins_tools/xpi/builds/$LATEST_XPI
#cp $XPI_PLUGIN "$XPI_PLUGIN_DIR"

cp -r ../../../cardcomm/tokend/BEID_Lion.tokend "$TOKEND_DIR/BEID.tokend"

cp -r ./install_scripts/* "$INSTALL_SCRIPTS_DIR"
	 
cp  ../../../plugins_tools/bin/Release/plistmerger "$PLISTMERGER_DIR"
cp  ../../../plugins_tools/plistMerger/Info.plist "$PLISTMERGER_DIR"

#copy distribution file
cp ./Distribution.txt "$RELEASE_DIR"

#copy drivers
cp -r ./drivers/* "$RELEASE_DIR"

#copy eid middleware app
cp -r "$EIDMIDDLEWAREAPP_PATH"  "$BEIDCARD_DIR"

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

pkgbuild --component "$EIDVIEWER_PATH" --identifier be.eid.viewer.app --version $REL_VERSION --install-location /Applications/ eidviewer.pkg

productbuild --distribution "$RELEASE_DIR/Distribution.txt" --resources "$RESOURCES_DIR" $PKG_NAME

if [ $SIGN_BUILD -eq 1 ];then
  productsign --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
  hdiutil create -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME
else
  hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
fi


echo "********** generate $PKG_NAME_DIAG and $DMG_NAME_DIAG **********"

#pkgbuild --component "$EIDMIDDLEWAREAPP_PATH" --identifier be.eid.middleware.app --version $REL_VERSION --install-location /Applications/ $PKG_NAME_DIAG

if [ $SIGN_BUILD -eq 1 ];then
  productsign --sign "Developer ID Installer" $PKG_NAME_DIAG $PKGSIGNED_NAME_DIAG
  hdiutil create -srcfolder $PKGSIGNED_NAME_DIAG -volname "${VOL_NAME_DIAG}" $DMG_NAME_DIAG
else
  hdiutil create -srcfolder $PKG_NAME_DIAG -volname "${VOL_NAME_DIAG}" $DMG_NAME_DIAG
fi

popd
