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






#####################################################################
################## BEIDUninstaller name defines ###########
#BEIDUninstaller name defines
#release dir, where normally files to be released will be placed
RELEASE_BEIDUNINSTALL_DIR="$(pwd)/release_BEIDUninstall"
#root dir, for files that are to be installed by the pkg
ROOT_BEIDUNINSTALL_DIR="$RELEASE_BEIDUNINSTALL_DIR/root"

#uninstall scripts dir, where the install scripts are that will be executed by the package
BEIDUNINSTALL_SCRIPTS_DIR="$RELEASE_BEIDUNINSTALL_DIR/install_scripts"
#licenses dir, where the licenses are that will be showed by the package
BEIDUNINSTALL_LICENSES_DIR="$RELEASE_BEIDUNINSTALL_DIR/licenses"
#resources dir, for files that are to be kept inside the pkg
BEIDUNINSTALL_RESOURCES_DIR="$RELEASE_BEIDUNINSTALL_DIR/resources"
#####################################################################


#base name of the package
REL_NAME="eID-Quick-Uninstaller"
#version number of the package

BUILD_NR=$(git rev-list --count HEAD)
PKG_NAME="$REL_NAME.pkg"
PKGSIGNED_NAME="${REL_NAME}-signed.pkg"
VOL_NAME="${REL_NAME}-${REL_VERSION}"
DMG_NAME="${REL_NAME}-${REL_VERSION}.dmg"


#cleanup previous build

if test -e "$RELEASE_BEIDUNINSTALL_DIR"; then
 rm -rdf "$RELEASE_BEIDUNINSTALL_DIR"
fi
if test -e $PKG_NAME; then
 rm $PKG_NAME
fi


#####################################################################
echo "********** prepare beiduninstall.pkg **********"

#create uninstaller dirs
mkdir -p "$BEIDUNINSTALL_SCRIPTS_DIR"
mkdir -p "$BEIDUNINSTALL_LICENSES_DIR"
mkdir -p "$BEIDUNINSTALL_RESOURCES_DIR"
mkdir -p "$ROOT_BEIDUNINSTALL_DIR"


#copy all files that should be part of the installer:

#copy licenses
cp ../../../doc/licenses/Dutch/eID-toolkit_licensingtermsconditions.txt \
	"$BEIDUNINSTALL_LICENSES_DIR/license_NL.txt" ; \
cp ../../../doc/licenses/English/eID-toolkit_licensingtermsconditions.txt \
	"$BEIDUNINSTALL_LICENSES_DIR/license_EN.txt" ; \
cp ../../../doc/licenses/French/eID-toolkit_licensingtermsconditions.txt \
	"$BEIDUNINSTALL_LICENSES_DIR/license_FR.txt" ; \
cp ../../../doc/licenses/German/eID-toolkit_licensingtermsconditions.txt \
	"$BEIDUNINSTALL_LICENSES_DIR/license_DE.txt" ; \

#copy resources
cp -R ./uninstall_resources/* $BEIDUNINSTALL_RESOURCES_DIR

#copy uninstall scripts
cp -R ./uninstall_scripts/* "$BEIDUNINSTALL_SCRIPTS_DIR"

#copy distribution file
cp Distribution_Uninstall.txt "$RELEASE_BEIDUNINSTALL_DIR/Distribution_Uninstall.txt"

echo "********** generate $PKG_NAME and $DMG_NAME **********"


#build the packages in the release dir
pushd $RELEASE_BEIDUNINSTALL_DIR

pkgbuild --root "$ROOT_BEIDUNINSTALL_DIR" --scripts "$BEIDUNINSTALL_SCRIPTS_DIR" --identifier be.eid.uninstall --version $REL_VERSION --install-location / BEIDUninstall.pkg

productbuild --distribution "$RELEASE_BEIDUNINSTALL_DIR/Distribution_Uninstall.txt" --resources "$BEIDUNINSTALL_RESOURCES_DIR" $PKG_NAME

#####################################################################

if [ $SIGN_BUILD -eq 1 ];then
  productsign --sign "Developer ID Installer" $PKG_NAME $PKGSIGNED_NAME
  hdiutil create -srcfolder $PKGSIGNED_NAME -volname "${VOL_NAME}" $DMG_NAME
  exit 1
else
  hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
fi


popd
