#!/bin/bash

set -e


#get the release number
source "$(pwd)/../../../scripts/mac/set_eidmw_version.sh"

#get the notarizer's account name
#create the bash file set_eidmw_username.sh to define:
#AC_USERNAME="dev.account@firm.be" 
source "$(pwd)/../../../scripts/mac/set_eidmw_username.sh"

#####################################################################
################## eIDMW installer name defines ###########
#installer name defines
#release dir, where all the beidbuild files to be released will be placed
EXPORT_DIR="$(pwd)/exports/export_eidmw"
#root dir, for files that are to be installed by the pkg
ROOT_DIR="$EXPORT_DIR/root"
#resources dir, for files that are to be kept inside the pkg
RESOURCES_DIR="$EXPORT_DIR/resources"
#install scripts dir, where the install scripts are that will be executed by the package
INSTALL_SCRIPTS_DIR="$EXPORT_DIR/install_scripts"

#pkcs11_inst dir, where our pkcs11 lib will be placed
PKCS11_INST_DIR="$ROOT_DIR/usr/local/lib"
MOZ_PKCS11_MANIFEST_DIR="$ROOT_DIR/Library/Application Support/Mozilla/PKCS11Modules"
#licenses dir, where our licences will be placed
LICENSES_DIR="$ROOT_DIR/Library/Belgium Identity Card/Licenses"
BEIDCARD_DIR="$ROOT_DIR/Library/Belgium Identity Card"
#####################################################################

#####################################################################
################## BEIDToken installer name defines ###########
#BEIDToken installer name defines
#release dir, where all the BEIDToken files to be released will be placed
EXPORT_BEIDTOKEN_DIR="$(pwd)/exports/export_BEIDToken"
#root dir, for files that are to be installed by the pkg
ROOT_BEIDTOKEN_DIR="$EXPORT_BEIDTOKEN_DIR/root"

#BEIDToken inst dir, where our BEIDToken app will be installed
BEIDTOKEN_INST_DIR="$ROOT_BEIDTOKEN_DIR/Applications"

BEIDTOKEN_PATH="$(pwd)/../../../export/BEIDToken.app"

#BEIDToken.plist path
BEIDTOKEN_PLIST_PATH="$(pwd)/BEIDToken.plist"

#install scripts dir, where the install scripts are that will be executed by the package
BEIDTOKEN_INSTALL_SCRIPTS_DIR="$EXPORT_BEIDTOKEN_DIR/install_scripts"
#####################################################################

#base name of the package
REL_NAME="eID-Quickinstaller"

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
if test -e "$EXPORT_DIR"; then
 rm -rdf "$EXPORT_DIR"
fi
#if test -e beidbuild.pkg; then
# rm beidbuild.pkg
#fi
#if test -e $PKG_NAME; then
# rm $PKG_NAME
#fi

#leave created dir there for now

#####################################################################
echo "********** prepare beidbuild.pkg **********"

#create installer dirs
mkdir -p "$PKCS11_INST_DIR"
mkdir -p "$LICENSES_DIR"
mkdir -p "$RESOURCES_DIR"
mkdir -p "$INSTALL_SCRIPTS_DIR"
mkdir -p "$MOZ_PKCS11_MANIFEST_DIR"

#copy all files that should be part of the installer:

codesign --timestamp --force -o runtime --sign "Developer ID Application" -v ../../../release/libbeidpkcs11.$REL_VERSION.dylib
cp ../../../release/libbeidpkcs11.$REL_VERSION.dylib $PKCS11_INST_DIR
#copy pkcs11 bundle
cp -R ./Packages/beid-pkcs11.bundle $PKCS11_INST_DIR
#make relative symbolic link from bundle to the dylib
mkdir -p "$PKCS11_INST_DIR/beid-pkcs11.bundle/Contents/MacOS/"
ln -s ../../../libbeidpkcs11.$REL_VERSION.dylib "$PKCS11_INST_DIR/beid-pkcs11.bundle/Contents/MacOS/libbeidpkcs11.dylib"

# Create PKCS#11 manifest file for Mozilla
cat > "$MOZ_PKCS11_MANIFEST_DIR/beidpkcs11.json" <<EOF
{
  "name": "beidpkcs11",
  "description": "Belgium eID PKCS#11 Module",
  "type": "pkcs11",
  "path": "/usr/local/lib/beid-pkcs11.bundle/Contents/MacOS/libbeidpkcs11.dylib",
  "allowed_extensions": ["belgiumeid@eid.belgium.be"]
}
EOF

#copy licenses
cp ../../../doc/licenses/Dutch/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_NL.txt" ; \
cp ../../../doc/licenses/English/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_EN.txt" ; \
cp ../../../doc/licenses/French/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_FR.txt" ; \
cp ../../../doc/licenses/German/eID-toolkit_licensingtermsconditions.txt \
	"$LICENSES_DIR/license_DE.txt" ; \

cp -R ./resources/* $RESOURCES_DIR

cp "$(pwd)/../../../scripts/mac/set_eidmw_version.sh" "$INSTALL_SCRIPTS_DIR"
cp -R ./install_scripts/* "$INSTALL_SCRIPTS_DIR"

#copy distribution file
cp ./Distribution_export.txt "$EXPORT_DIR"

#####################################################################
echo "********** prepare BEIDToken.pkg **********"

#cleanup
if test -e "$EXPORT_BEIDTOKEN_DIR"; then
 rm -rdf "$EXPORT_BEIDTOKEN_DIR"
fi

#create installer dirs
mkdir -p "$BEIDTOKEN_INST_DIR"
mkdir -p "$BEIDTOKEN_INSTALL_SCRIPTS_DIR"

#copy install scripts
cp -R ./install_scripts_BEIDToken/* "$BEIDTOKEN_INSTALL_SCRIPTS_DIR"

#copy eid token app
cp -R "$BEIDTOKEN_PATH"  "$BEIDTOKEN_INST_DIR"/BEIDToken.app

#####################################################################

echo "********** generate $PKG_NAME and $DMG_NAME **********"
#chgrp    wheel  "$ROOT_DIR/usr"
#chgrp    wheel  "$ROOT_DIR/usr/local"
#chgrp    wheel  "$ROOT_DIR/usr/local/lib"

#build the packages in the export dir
pushd $EXPORT_DIR

pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild.pkg

pkgbuild --root "$ROOT_BEIDTOKEN_DIR" --scripts "$BEIDTOKEN_INSTALL_SCRIPTS_DIR" --component-plist "$BEIDTOKEN_PLIST_PATH" --identifier be.eid.BEIDtoken.app --version $REL_VERSION --install-location / BEIDToken.pkg

productbuild --distribution "$EXPORT_DIR/Distribution_export.txt" --resources "$RESOURCES_DIR" $PKG_NAME

#####################################################################
#Using HFS+ as fs, as OS X 10.11 (El Capitan) does not yet support APFS
#Using UDIF bzip2-compressed disk image for notarization
#####################################################################

productsign --timestamp --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
hdiutil create -fs "HFS+" -format UDBZ -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME

#notarize the quick installer
/usr/bin/xcrun altool --notarize-app --primary-bundle-id "be.eid.QuickInstaller.dmg" --username "AC_USERNAME" --password "@keychain:altool" --file "$DMG_NAME"
xcrun altool --notarization-history 0 -u "$AC_USERNAME" -p "@keychain:altool"


#staple the notarization package to the DMG.
/usr/bin/xcrun stapler staple -v "$DMG_NAME"

#  productsign --timestamp --sign "Developer ID Installer" "beidbuild.pkg" "beidbuild-signed.pkg"
#  hdiutil create -fs "HFS+" -format UDBZ -srcfolder "beidbuild-signed.pkg" -volname "beidbuild${REL_VERSION}" "beidbuild${REL_VERSION}.dmg"

#  productsign --timestamp --sign "Developer ID Installer" "BEIDToken.pkg" "BEIDToken-signed.pkg"
#  hdiutil create -fs "HFS+" -format UDBZ -srcfolder "BEIDToken-signed.pkg" -volname "BEIDToken${REL_VERSION}" "BEIDToken${REL_VERSION}.dmg"

  exit 1


popd
