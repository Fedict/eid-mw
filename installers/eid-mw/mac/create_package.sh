#!/bin/bash

set -e

if [ -z "$MAC_BUILD_CONFIG" ]
then
	MAC_BUILD_CONFIG=Release
fi

#set SIGN_BUILD=0 in the environment to not sign the .pkg files:
# SIGN_BUILD=0 ./create_package.sh
#or
# SIGN_BUILD=0 ./make-mac.sh
SIGN_BUILD=${SIGN_BUILD:-1}

#get the release number
source "$(pwd)/../../../scripts/mac/set_eidmw_version.sh"

####################################################################
######### specific release defines (for test builds) ###############
PKCS11_DYLIB_PATH="$(pwd)/../../../$MAC_BUILD_CONFIG/libbeidpkcs11.$REL_VERSION.dylib"
#BEIDToken path
if [ "$MAC_BUILD_CONFIG" = "Debug" ]
then
	BEIDTOKEN_PATH="$(pwd)/../../../cardcomm/ctktoken/build/$MAC_BUILD_CONFIG/BEIDTokenApp.app"
else
	BEIDTOKEN_PATH="$(pwd)/../../../cardcomm/ctktoken/build/$MAC_BUILD_CONFIG/BEIDToken.app"
fi
#####################################################################

#####################################################################
################## eIDMW installer name defines ###########
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
MOZ_PKCS11_MANIFEST_DIR="$ROOT_DIR/Library/Application Support/Mozilla/PKCS11Modules"
#licenses dir, where our licences will be placed
LICENSES_DIR="$ROOT_DIR/Library/Belgium Identity Card/Licenses"
BEIDCARD_DIR="$ROOT_DIR/Library/Belgium Identity Card"
#####################################################################

#####################################################################
################## BEIDToken installer name defines ###########
#BEIDToken installer name defines
#release dir, where all the BEIDToken files to be released will be placed
RELEASE_BEIDTOKEN_DIR="$(pwd)/release_BEIDToken"
#root dir, for files that are to be installed by the pkg
ROOT_BEIDTOKEN_DIR="$RELEASE_BEIDTOKEN_DIR/root"

#BEIDToken inst dir, where our BEIDToken app will be installed
BEIDTOKEN_INST_DIR="$ROOT_BEIDTOKEN_DIR/Applications"

#BEIDToken path
#BEIDTOKEN_PATH should be already defined by calling script

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

PKG_NAME_DIAG="$REL_NAME_DIAG.pkg"
PKGSIGNED_NAME_DIAG="${REL_NAME_DIAG}-signed.pkg"
VOL_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}"
DMG_NAME_DIAG="${REL_NAME_DIAG}-${REL_VERSION}.dmg"

#cleanup previous build

if test -e "$RELEASE_DIR"; then
 rm -rdf "$RELEASE_DIR"
fi
if test -e beidbuild.pkg; then
 rm beidbuild.pkg
fi
if test -e $PKG_NAME; then
 rm $PKG_NAME
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
cp $PKCS11_DYLIB_PATH $PKCS11_INST_DIR
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
 rm -rdf "$RELEASE_BEIDTOKEN_DIR"
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

chgrp    wheel  "$ROOT_DIR/usr"
chgrp    wheel  "$ROOT_DIR/usr/local"
chgrp    wheel  "$ROOT_DIR/usr/local/lib"

#build the packages in the release dir
pushd $RELEASE_DIR

pkgbuild --root "$ROOT_DIR" --scripts "$INSTALL_SCRIPTS_DIR" --identifier be.eid.middleware --version $REL_VERSION --install-location / beidbuild.pkg

pkgbuild --root "$ROOT_BEIDTOKEN_DIR" --scripts "$BEIDTOKEN_INSTALL_SCRIPTS_DIR" --component-plist "$BEIDTOKEN_PLIST_PATH" --identifier be.eid.BEIDtoken.app --version $REL_VERSION --install-location / BEIDToken.pkg

productbuild --distribution "$RELEASE_DIR/Distribution_export.txt" --resources "$RESOURCES_DIR" $PKG_NAME

#####################################################################
#Using HFS+ as fs, as OS X 10.11 (El Capitan) does not yet support APFS
#####################################################################

if [ $SIGN_BUILD -eq 1 ];then

  hdiutil create -fs "HFS+" -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME

# signing with Mac Developer: ambiguity on some systems where multiple Mac Developer accounts are present, so skip signing for now
#  codesign --timestamp --force -o runtime --sign "Mac Developer" -v $DMG_NAME

  hdiutil create -fs "HFS+" -srcfolder "beidbuild.pkg" -volname "beidbuild${REL_VERSION}" "beidbuild${REL_VERSION}.dmg"
#  codesign --timestamp --force -o runtime --sign "Mac Developer" -v "beidbuild${REL_VERSION}.dmg"

  hdiutil create -fs "HFS+" -srcfolder "BEIDToken.pkg" -volname "BEIDToken${REL_VERSION}" "BEIDToken${REL_VERSION}.dmg"
#  codesign --timestamp --force -o runtime --sign "Mac Developer" -v "BEIDToken ${REL_VERSION}.dmg"

  exit 1
else
  hdiutil create -fs "HFS+" -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
  hdiutil create -fs "HFS+" -srcfolder "beidbuild.pkg" -volname "beidbuild${REL_VERSION}" "beidbuild${REL_VERSION}.dmg"

  hdiutil create -fs "HFS+" -srcfolder "BEIDToken.pkg" -volname "BEIDToken${REL_VERSION}" "BEIDToken${REL_VERSION}.dmg"
fi

popd
