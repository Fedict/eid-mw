#!/bin/bash

set -e

#this script prepares and creates the installer packages for the eid middleware releases.
#it should be called from one of the shells that first build the related binaries and set some variables already.

#we define a default config (unsigned release) here, 
#in case (for testing the package making) a package will be made without rebuilding the binaries.
if [ -z "$EIDMW_BUILD_CONFIG" ]
then
	EIDMW_BUILD_CONFIG=Release
	EIDMW_SIGN_BUILD=0
fi
echo "EIDMW_BUILD_CONFIG is $EIDMW_BUILD_CONFIG"

#get the release number
source "$(pwd)/../../../scripts/mac/set_eidmw_version.sh"



if [ "$EIDMW_BUILD_CONFIG" = "Export" ]
then
	#when creating the installers packages that will be released to the public (export config),
	#we need to run tools that require a mac dev account and PW
	#the password need to be stored in the keychain, named "altool"
	#the account name should be stored in "set_eidmw_username.sh"

	#get the notarizer's account name
	#create the bash file set_eidmw_username.sh to define:
	#AC_USERNAME="dev.account@firm.be" 
	source "$(pwd)/../../../scripts/mac/set_eidmw_username.sh"
	#release dir, where all the beidbuild files to be released will be placed
	RELEASE_DIR="$(pwd)/exports/export_eidmw"
	#release dir, where all the BEIDToken files to be released will be placed
	RELEASE_BEIDTOKEN_DIR="$(pwd)/exports/export_BEIDToken"
	#BEIDToken.app path, where this script will find the exported BEIDToken.app
	BEIDTOKEN_PATH="$(pwd)/../../../export/BEIDToken.app"
else
	#release dir, where all the beidbuild files to be released will be placed
	RELEASE_DIR="$(pwd)/release"
	#release dir, where all the BEIDToken files to be released will be placed
	RELEASE_BEIDTOKEN_DIR="$(pwd)/release_BEIDToken"
	PKCS11_DYLIB_PATH="$(pwd)/../../../$EIDMW_BUILD_CONFIG/libbeidpkcs11.$REL_VERSION.dylib"
	#BEIDToken path
	if [ "$EIDMW_BUILD_CONFIG" = "Debug" ]
	then
		BEIDTOKEN_PATH="$(pwd)/../../../cardcomm/ctktoken/build/$EIDMW_BUILD_CONFIG/BEIDTokenApp.app"
	else
		BEIDTOKEN_PATH="$(pwd)/../../../cardcomm/ctktoken/build/$EIDMW_BUILD_CONFIG/BEIDToken.app"	
	fi
fi

################## eIDMW installer name defines ###########
#installer name defines
#release dir, where all the beidbuild files to be released will be placed
#RELEASE_DIR is defined above, depending on build configuration
#root dir, for files that are to be installed by the pkg
ROOT_DIR="$RELEASE_DIR/root"
#resources dir, for files that are to be kept inside the pkg
RESOURCES_DIR="$RELEASE_DIR/resources"
#install scripts dir, where the install scripts are that will be executed by the package
INSTALL_SCRIPTS_DIR="$RELEASE_DIR/install_scripts"

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
#RELEASE_BEIDTOKEN_DIR is defined above, depending on build configuration
#root dir, for files that are to be installed by the pkg
ROOT_BEIDTOKEN_DIR="$RELEASE_BEIDTOKEN_DIR/root"

#BEIDToken inst dir, where our BEIDToken app will be installed
BEIDTOKEN_INST_DIR="$ROOT_BEIDTOKEN_DIR/Applications"

#BEIDToken path
#BEIDTOKEN_PATH should be already defined
#BEIDToken.plist path
BEIDTOKEN_PLIST_PATH="$(pwd)/BEIDToken.plist"

#install scripts dir, where the install scripts are that will be executed by the package
BEIDTOKEN_INSTALL_SCRIPTS_DIR="$RELEASE_BEIDTOKEN_DIR/install_scripts"
#####################################################################

#base name of the package
REL_NAME="eID-Quickinstaller"

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
PKGSIGNED_NAME="${REL_NAME}-signed.pkg"
VOL_NAME="${REL_NAME}-${REL_VERSION}"
DMG_NAME="${REL_NAME}-${REL_VERSION}.dmg"
DMG_BACKUP_NAME="${REL_NAME}-${REL_VERSION}-backup.dmg"
PKG_NAME_DIAG="$REL_NAME_DIAG.pkg"
PKGSIGNED_NAME_DIAG="${REL_NAME_DIAG}-signed.pkg"
VOL_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}"
DMG_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}.dmg"

#cleanup previous build
if test -e "$RELEASE_DIR"; then
 sudo rm -rdf "$RELEASE_DIR"
fi

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
#in case of an export, also sign the pkcs11 library
#the other binaries should already been signed when archiving in xcode
if [ "$EIDMW_BUILD_CONFIG" = "Export" ]
then
	codesign --timestamp --force -o runtime --sign "Developer ID Application" -v ../../../release/libbeidpkcs11.$REL_VERSION.dylib
	cp ../../../release/libbeidpkcs11.$REL_VERSION.dylib $PKCS11_INST_DIR
else
	cp $PKCS11_DYLIB_PATH $PKCS11_INST_DIR
fi
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
cp ./Distribution_export.txt "$RELEASE_DIR"

#####################################################################
echo "********** prepare BEIDToken.pkg **********"

#cleanup
if test -e "$RELEASE_BEIDTOKEN_DIR"; then
 sudo rm -rdf "$RELEASE_BEIDTOKEN_DIR"
fi

#create installer dirs
mkdir -p "$BEIDTOKEN_INST_DIR"
mkdir -p "$BEIDTOKEN_INSTALL_SCRIPTS_DIR"

#copy install scripts
cp -R ./install_scripts_BEIDToken/* "$BEIDTOKEN_INSTALL_SCRIPTS_DIR"

#copy eid token app
cp -R "$BEIDTOKEN_PATH"  "$BEIDTOKEN_INST_DIR"/BEIDToken.app





#####################################################################
#The preparations for creating the packages have been completed
#(i.e. placing all files in the wanted directory structure)
#Now we'll build and sign the packages

#For the DMG:
#Using HFS+ as fs, as OS X 10.11 (El Capitan) does not yet support APFS
#Using UDIF bzip2-compressed disk image for notarization
#####################################################################
echo "********** building packages **********"

#build the packages in the RELEASE_DIR
pushd $RELEASE_DIR

if [ "$EIDMW_BUILD_CONFIG" = "Export" ]
then
  echo "********** building and signing beidbuild.pkg **********"
  pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild-unsigned.pkg
  productsign --timestamp --sign "Developer ID Installer" "beidbuild-unsigned.pkg" "beidbuild.pkg"

  echo "********** building and signing BEIDToken.pkg **********"
  pkgbuild --root "$ROOT_BEIDTOKEN_DIR" --scripts "$BEIDTOKEN_INSTALL_SCRIPTS_DIR" --component-plist "$BEIDTOKEN_PLIST_PATH" --identifier be.eid.BEIDtoken.app --version $REL_VERSION --install-location / BEIDToken-unsigned.pkg
  productsign --timestamp --sign "Developer ID Installer" "BEIDToken-unsigned.pkg" "BEIDToken.pkg"

  echo "********** building $PKG_NAME **********"
  productbuild --distribution "$RELEASE_DIR/Distribution_export.txt" --resources "$RESOURCES_DIR" $PKG_NAME

  #####################################################################
  #Using HFS+ as fs, as OS X 10.11 (El Capitan) does not yet support APFS
  #Using UDIF bzip2-compressed disk image for notarization
  #####################################################################

  echo "********** signing the package with Developer ID Installer **********"
  productsign --timestamp --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
  hdiutil create -fs "HFS+" -format UDBZ -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME

  echo "********** signing the disk image with Developer ID Application **********"
  codesign --timestamp --force -o runtime --sign "Developer ID Application" -v $DMG_NAME


  echo "********** notarize the quick installer **********"
  /usr/bin/xcrun altool --notarize-app --primary-bundle-id "be.eid.QuickInstaller.dmg" --username "$AC_USERNAME" --password "@keychain:altool" --file "$DMG_NAME"


  #create a backup copy, in case the stapling goes wrong
  cp -R "$DMG_NAME"  "$DMG_BACKUP_NAME"

  echo "********** waiting 20 sec **********"
  #wait 20 seconds to give the notarization service some time to make the ticket available online (otherwise stapling will fail)
  sleep 20

  echo "********** check notarization history **********"
  /usr/bin/xcrun altool --notarization-history 0 -u "$AC_USERNAME" -p "@keychain:altool"


  #staple the notarization package to the DMG.
  /usr/bin/xcrun stapler staple -v "$DMG_NAME"

  #copy the stapled disk image to the Mac scripts folder
  cp -R "$DMG_NAME" "$(pwd)/../../../../../scripts/mac/"

else

  echo "********** building beidbuild.pkg **********"
  pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild.pkg
  if [ $EIDMW_SIGN_BUILD -eq 1 ];then
    echo "********** signing beidbuild.pkg with Mac Developer **********"
    codesign --timestamp --force -o runtime --sign "Mac Developer" -v "beidbuild.pkg"
  fi

  echo "********** building BEIDToken.pkg **********"
  pkgbuild --root "$ROOT_BEIDTOKEN_DIR" --scripts "$BEIDTOKEN_INSTALL_SCRIPTS_DIR" --component-plist "$BEIDTOKEN_PLIST_PATH" --identifier be.eid.BEIDtoken.app --version $REL_VERSION --install-location / BEIDToken.pkg
  if [ $EIDMW_SIGN_BUILD -eq 1 ];then
    echo "********** signing BEIDToken.pkg with Mac Developer **********"
    codesign --timestamp --force -o runtime --sign "Mac Developer" -v "BEIDToken.pkg"
  fi

  echo "********** building $PKG_NAME **********"
  productbuild --distribution "$RELEASE_DIR/Distribution_export.txt" --resources "$RESOURCES_DIR" $PKG_NAME
  if [ $EIDMW_SIGN_BUILD -eq 1 ];then
    echo "********** signing $PKG_NAME with Mac Developer **********"
    codesign --timestamp --force -o runtime --sign "Mac Developer" -v $PKG_NAME
  fi

  echo "********** creating the installer dmg package with Mac Developer **********"
  hdiutil create -fs "HFS+" -format UDBZ -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
  if [ $EIDMW_SIGN_BUILD -eq 1 ];then
    echo "********** signing $PKG_NAME with Mac Developer **********"
    codesign --timestamp --force -o runtime --sign "Mac Developer" -v $DMG_NAME
  fi
fi

popd
