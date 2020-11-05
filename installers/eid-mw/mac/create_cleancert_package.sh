#!/bin/bash

set -e


MAC_BUILD_CONFIG=Release


#get the release number
source "$(pwd)/../../../scripts/mac/set_eidmw_version.sh"


#####################################################################
################## BEIDCLEANCERTSer name defines ###########
#BEIDCLEANCERTSer name defines
#release dir, where normally files to be released will be placed
RELEASE_BEIDCLEANCERTS_DIR="$(pwd)/release_BEIDCLEANCERTS"
#root dir, for files that are to be installed by the pkg
ROOT_BEIDCLEANCERTS_DIR="$RELEASE_BEIDCLEANCERTS_DIR/root"

#certclean scripts dir, where the install scripts are that will be executed by the package
BEIDCLEANCERTS_SCRIPTS_DIR="$RELEASE_BEIDCLEANCERTS_DIR/install_scripts"
#licenses dir, where the licenses are that will be showed by the package
BEIDCLEANCERTS_LICENSES_DIR="$RELEASE_BEIDCLEANCERTS_DIR/licenses"
#resources dir, for files that are to be kept inside the pkg
BEIDCLEANCERTS_RESOURCES_DIR="$RELEASE_BEIDCLEANCERTS_DIR/resources"
#####################################################################


#base name of the package
REL_NAME="eID-Quickinstaller"
PKCS11_BUNDLE="beid-pkcs11.bundle"
#version number of the package
BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
PKGSIGNED_NAME="${REL_NAME}-signed.pkg"
VOL_NAME="${REL_NAME}-${REL_VERSION}"
DMG_NAME="${REL_NAME}-${REL_VERSION}.dmg"
DMG_BACKUP_NAME="${REL_NAME}-${REL_VERSION}-backup.dmg"


#####################################################################
echo "********** prepare BEIDCLEANCERTS.pkg **********"

#create certcleaner dirs
mkdir -p "$BEIDCLEANCERTS_SCRIPTS_DIR"
mkdir -p "$BEIDCLEANCERTS_RESOURCES_DIR"
mkdir -p "$ROOT_BEIDCLEANCERTS_DIR"


#copy all files that should be part of the installer:

#copy licenses
cp -R ./resources/* $BEIDCLEANCERTS_RESOURCES_DIR

#overwrite the readme files
cp -R ./certclean_resources/* $BEIDCLEANCERTS_RESOURCES_DIR

#copy certclean scripts
cp -R ./certclean_scripts/* "$BEIDCLEANCERTS_SCRIPTS_DIR"

echo "********** generate $PKG_NAME and $DMG_NAME **********"



Echo "********** building and signing eID-certcleaner.pkg **********"
#build the packages in the release dir
pushd $RELEASE_BEIDCLEANCERTS_DIR

pkgbuild --root "$ROOT_BEIDCLEANCERTS_DIR" --scripts "$BEIDCLEANCERTS_SCRIPTS_DIR" --identifier be.eid.certclean --version $REL_VERSION --install-location / eID-certcleaner.pkg

productsign --timestamp --sign "Developer ID Installer" eID-certcleaner.pkg certclean.pkg


  #####################################################################
  #Using HFS+ as fs, as OS X 10.11 (El Capitan) does not yet support APFS
  #Using UDIF bzip2-compressed disk image for notarization
  #####################################################################

  Echo "********** signing the package with Developer ID Installer **********"
  productsign --timestamp --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
  hdiutil create -fs "HFS+" -format UDBZ -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME

  Echo "********** signing the disk image with Developer ID Application **********"
  codesign --timestamp --force -o runtime --sign "Developer ID Application" -v $DMG_NAME





  Echo "********** notarize the quick installer **********"
  /usr/bin/xcrun altool --notarize-app --primary-bundle-id "be.eid.QuickInstaller.dmg" --username "$AC_USERNAME" --password "@keychain:altool" --file "$DMG_NAME"


  #create a backup copy, in case the stapling goes wrong
  cp -R "$DMG_NAME"  "$DMG_BACKUP_NAME"

  Echo "********** waiting 200 sec **********"
  #wait 200 seconds to give the notarization service some time to make the ticket available online (otherwise stapling will fail)
  sleep 200

  Echo "********** check notarization history **********"
  /usr/bin/xcrun altool --notarization-history 0 -u "$AC_USERNAME" -p "@keychain:altool"


  #staple the notarization package to the DMG.
  /usr/bin/xcrun stapler staple -v "$DMG_NAME"

  #copy the stapled disk image to the Mac scripts folder
  #cp -R "$DMG_NAME" "$(pwd)/../../../../../scripts/mac/"

popd





