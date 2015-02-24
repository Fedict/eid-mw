#!/bin/bash

set -e
set -x

#installer name defines
#resources dir, where the plists are
RESOURCES_DIR="./resources"
#install scripts dir, where the install scripts are
INFO_DIR="./info"
#install scripts dir, where the install scripts are
INSTALL_SCRIPTS_DIR="./install_scripts"
#root dir, where all files to be packaged will be placed
ROOT_DIR="$(pwd)/root"
#inst dir, where our libs and binaries will be placed
INST_DIR=/usr/local
#licenses dir, where our licences will be placed
LICENSES_DIR="/Library/BelgiumIdentityCard/Licenses"

#dir where files will be placed temporarely, before being handled by the postinstall script
BELGIUM_DIR="/Library/BelgiumIdentityCard"

#base name of the package
REL_NAME="beid"
#version number of the package
#REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
#REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')
REL_VERSION="4.1.3"

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(svn info ../../ | grep Revision | sed s/"Revision: "/""/)
PKG_NAME="$REL_NAME.pkg"
PKG_MAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
VOL_NAME="${REL_NAME} OSX ${REL_VERSION}"
DMG_NAME="${REL_NAME}_${REL_VERSION}.dmg"


XPI_PLUGIN_DIR="/Library/Application\ Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/belgiumeid@eid.belgium.be"


#directories used to create the installer


LIB_DIR="../../../output/lib/Release"
BIN_DIR="../../../output/bin/Release"

cleanup() {
	test -e $ROOT_DIR && rm -rdf $ROOT_DIR
	chown -R $SUDO_USER $PKG_NAME
	chown $SUDO_USER $DMG_NAME
}

trap cleanup EXIT


#create installer dirs
mkdir -p $ROOT_DIR/$INST_DIR/lib/siscardplugins
mkdir -p $ROOT_DIR/$INST_DIR/lib/beidqt
mkdir -p $ROOT_DIR/$INST_DIR/share/beid/certs
mkdir -p $ROOT_DIR/$LICENSES_DIR
mkdir -p $ROOT_DIR/usr/bin/

#copy all files that should be part of the installer:
#copy third party deliverables
cp $LIB_DIR/libbeidpkcs11.$REL_VERSION.dylib $ROOT_DIR/$INST_DIR/lib/


LATEST_XPI=$(readlink ../../../plugins_tools/xpi/builds/belgiumeid-CURRENT.xpi)
XPI_PLUGIN=../../../plugins_tools/xpi/builds/$LATEST_XPI
cp $XPI_PLUGIN $ROOT_DIR/$BELGIUM_DIR

cp -r ../../../cardcomm/tokend/BEID.tokend $ROOT_DIR/$BELGIUM_DIR
cp -r ../../../cardcomm/tokend/BEID_Lion.tokend $ROOT_DIR/$BELGIUM_DIR

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

#copy certificates
cp ../../../installers/certificates/beid-cert-belgiumrca2.der $ROOT_DIR/$INST_DIR/share/beid/certs/

#copy pkcs11 bundle
#cp -r ../../../misc/mac/pkcs11.bundle $ROOT_DIR/$INST_DIR/lib/$PKCS11_BUNDLE
	 
## Make interpretes spaces as separator. So we specify 2 names:
## the one that will be installed and the one to which this
## name will be renamed into in the postflight script at the end
## of the installation.


#####################################################################
################################################################
POSTFLIGHT=$REL_DIR/resources/postflight
PREFLIGHT=$REL_DIR/resources/preflight
INSTALLATIONCHECK=$REL_DIR/resources/InstallationCheck
INSTALLATIONCHECKSTRINGS=$REL_DIR/resources/InstallationCheck.strings


echo "********** manipulate mw_installer **********"
#	  sed -e "s;REPL_REL_VERSION;$REL_VERSION;g" \
#	  sed -e "s;REL_NAME;$REL_NAME;g" -e "s;PKCS11LIB_NAME_dylib;$PKCS11LIB_NAME_dylib;g" \
#	  	   ./resources/pkcs11.bundle/Contents/Info.plist > \
#	  	   $ROOT_DIR/$INST_DIR/lib/$PKCS11_BUNDLE/Contents/Info.plist

echo "********** generate $PKG_NAME and $DMG_NAME **********"

#chmod g+w $ROOT_DIR/$INST_DIR
#chmod g+w $ROOT_DIR/$INST_DIR/lib
#chmod a-x $ROOT_DIR/$INST_DIR/etc/beid.conf
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/Info.plist
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/PkgInfo
chgrp    wheel  $ROOT_DIR/usr
chgrp    wheel  $ROOT_DIR/usr/bin
chgrp    wheel  $ROOT_DIR/usr/local
chgrp    wheel  $ROOT_DIR/usr/local/lib
chgrp -R admin  $ROOT_DIR/$BELGIUM_DIR/BEID.tokend
chgrp -R admin  $ROOT_DIR/$BELGIUM_DIR/BEID_Lion.tokend

$PKG_MAKER -r $ROOT_DIR -o $PKG_NAME -f $INFO_DIR/Info.plist \
	-e $RESOURCES_DIR -s $INSTALL_SCRIPTS_DIR -n REL_VERSION

hdiutil create -srcfolder $PKG_NAME -volname "${VOL_NAME}" $DMG_NAME
