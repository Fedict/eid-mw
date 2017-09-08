#!/bin/bash

set -e

#set SIGN_BUILD=1 in the environment to sign the .pkg files:
# SIGN_BUILD=1 ./create_package.sh
#or
# SIGN_BUILD=1 ./make-mac.sh
SIGN_BUILD=${SIGN_BUILD:-0}

#get the release number
source "$(pwd)/../../../scripts/mac/set_eidmw_version.sh"

#installer name defines
#release dir, where all the beidbuild files to be released will be placed
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
#PLISTMERGER_DIR="$ROOT_DIR/Library/Belgium Identity Card/plistMerger"
BEIDCARD_DIR="$ROOT_DIR/Library/Belgium Identity Card"
#xpi plugin dir, where the xpi plugin will be placed
#XPI_PLUGIN_DIR="$ROOT_DIR/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}"
#tokend dir, where the BEID.tokend will be placed
TOKEND_DIR="$ROOT_DIR/Library/Security/tokend"

#eIDMiddleware app path
EIDMIDDLEWAREAPP_PATH="$(pwd)/../../../plugins_tools/aboutmw/OSX/eID Middleware/Release/eID Middleware.app"


#viewer installer name defines
#release dir, where all the beidbuild files to be released will be placed
RELEASE_VIEWER_DIR="$(pwd)/release_viewer"
#root dir, for files that are to be installed by the pkg
#ROOT_VIEWER_DIR="$RELEASE_VIEWER_DIR/root"

EIDVIEWER_TMPL_DIR="$(pwd)/../../eid-viewer/mac/"

#BEIDToken installer name defines
#release dir, where all the beidbuild files to be released will be placed
RELEASE_BEIDToken_DIR="$(pwd)/release_BEIDToken"
#root dir, for files that are to be installed by the pkg
ROOT_BEIDTOKEN_DIR="$RELEASE_BEIDToken_DIR/root"

#BEIDToken inst dir, where our BEIDToken app will be installed
BEIDTOKEN_INST_DIR="$ROOT_BEIDTOKEN_DIR/Library/Belgium Identity Card"

#BEIDToken path
BEIDTOKEN_PATH="$(pwd)/../../../cardcomm/ctktoken/Release/BEIDTokenApp.app"

#eIDViewer.plist path
BEIDTOKEN_PLIST_PATH="$(pwd)/BEIDToken.plist"


#base name of the package
REL_NAME="eID-Quickinstaller"
REL_NAME_DIAG="beid_diagnostic"
#version number of the package
#REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
#REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')
#REL_VERSION="$4.1.10"

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


#####################################################################
echo "********** prepare beidbuild.pkg **********"

#create installer dirs
mkdir -p "$PKCS11_INST_DIR"
mkdir -p "$LICENSES_DIR"
mkdir -p "$TOKEND_DIR"
#mkdir -p "$XPI_PLUGIN_DIR"
mkdir -p "$RESOURCES_DIR"
mkdir -p "$INSTALL_SCRIPTS_DIR"
#mkdir -p "$PLISTMERGER_DIR"

#copy all files that should be part of the installer:
cp ../../../Release/libbeidpkcs11.$REL_VERSION.dylib $PKCS11_INST_DIR
#copy pkcs11 bundle
cp -R ./Packages/beid-pkcs11.bundle $PKCS11_INST_DIR
#make relative symblic link from bundle to the dylib
mkdir -p "$PKCS11_INST_DIR/beid-pkcs11.bundle/Contents/MacOS/"
ln -s ../../../libbeidpkcs11.$REL_VERSION.dylib "$PKCS11_INST_DIR/beid-pkcs11.bundle/Contents/MacOS/libbeidpkcs11.dylib"


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


cp -R ./resources/* $RESOURCES_DIR


#LATEST_XPI=$(readlink ../../../plugins_tools/xpi/builds/belgiumeid-CURRENT.xpi)
#XPI_PLUGIN=../../../plugins_tools/xpi/signed-build/belgiumeid-signed
#cp -R $XPI_PLUGIN "$XPI_PLUGIN_DIR/belgiumeid@eid.belgium.be"

cp -R ../../../cardcomm/tokend/BEID_Lion.tokend "$TOKEND_DIR/BEID.tokend"

cp "$(pwd)/../../../scripts/mac/set_eidmw_version.sh" "$INSTALL_SCRIPTS_DIR"
cp -R ./install_scripts/* "$INSTALL_SCRIPTS_DIR"

#copy distribution file
cp ./Distribution.txt "$RELEASE_DIR"

#copy drivers
cp -R ./drivers/* "$RELEASE_DIR"

#copy eid middleware app
cp -R "$EIDMIDDLEWAREAPP_PATH"  "$BEIDCARD_DIR"

#####################################################################
echo "********** prepare BEIDTokenApp.pkg **********"

#cleanup
#test -e "$RELEASE_BEIDTOKEN_DIR" && rm -rdf "$RELEASE_BEIDTOKEN_DIR"
#test -e BEIDToken.pkg && rm BEIDToken.pkg

#create installer dirs
mkdir -p "$BEIDTOKEN_INST_DIR"

#copy eid token app
cp -R "$BEIDTOKEN_PATH"  "$BEIDTOKEN_INST_DIR"

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
#pkgbuild --analyze --root "$ROOT_DIR" beidbuild.plist

pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild.pkg

pkgbuild --root "$ROOT_BEIDTOKEN_DIR" --component-plist "$BEIDTOKEN_PLIST_PATH" --identifier be.eid.BEIDtoken.app --version $REL_VERSION --install-location / BEIDTokenApp.pkg

productbuild --distribution "$RELEASE_DIR/Distribution.txt" --resources "$RESOURCES_DIR" $PKG_NAME

#####################################################################

if [ $SIGN_BUILD -eq 1 ];then
  productsign --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
  hdiutil create -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME

  productsign --sign "Developer ID Installer" "beidbuild.pkg" "beidbuild-signed.pkg"
  hdiutil create -srcfolder "beidbuild-signed.pkg" -volname "beidbuild${REL_VERSION}" "beidbuild${REL_VERSION}.dmg"

  #productsign --sign "Developer ID Application" "eID Viewer.app" "eID Viewer.app-signed.app"
  productsign --sign "Developer ID Installer" "BEIDTokenApp.pkg" "BEIDToken-signedApp.pkg"
  hdiutil create -srcfolder "BEIDTokenApp-signed.pkg" -volname "BEIDTokenApp${REL_VERSION}" "BEIDTokenApp${REL_VERSION}.dmg"
  exit 1
else
  hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
  hdiutil create -srcfolder "beidbuild.pkg" -volname "beidbuild${REL_VERSION}" "beidbuild${REL_VERSION}.dmg"
  #hdiutil create -srcfolder "eidviewer.pkg" -volname "eidviewer${REL_VERSION}" "eidviewer${REL_VERSION}.dmg"
  hdiutil create -srcfolder "BEIDTokenApp.pkg" -volname "BEIDTokenApp${REL_VERSION}" "BEIDTokenApp${REL_VERSION}.dmg"
fi


#echo "********** generate $PKG_NAME_DIAG and $DMG_NAME_DIAG **********"
#
#pkgbuild --component "$EIDMIDDLEWAREAPP_PATH" --identifier be.eid.middleware.app --version $REL_VERSION --install-location /Applications/ $PKG_NAME_DIAG
#
#if [ $SIGN_BUILD -eq 1 ];then
#  productsign --sign "Developer ID Installer" $PKG_NAME_DIAG $PKGSIGNED_NAME_DIAG
#  hdiutil create -srcfolder $PKGSIGNED_NAME_DIAG -volname "${VOL_NAME_DIAG}" $DMG_NAME_DIAG
#else
#  hdiutil create -srcfolder $PKG_NAME_DIAG -volname "${VOL_NAME_DIAG}" $DMG_NAME_DIAG
#fi

popd
