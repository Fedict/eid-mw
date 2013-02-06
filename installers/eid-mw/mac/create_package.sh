#!/bin/bash

#installer name defines
#resources dir, where the plists are
RESOURCES_NO_SVN_DIR="./resources_no_svn"
RESOURCES_DIR="./resources"
#install scripts dir, where the install scripts are
INFO_DIR="./info"
#install scripts dir, where the install scripts are
INSTALL_SCRIPTS_DIR="./install_scripts"
INSTALL_SCRIPTS_NO_SVN_DIR="./install_scripts_no_svn"
#root dir, where all files to be packaged will be placed
ROOT_DIR="./root"
#inst dir, where our libs and binaries will be placed
INST_DIR=/usr/local
#licenses dir, where our licences will be placed
LICENSES_DIR="/Library/BelgiumIdentityCard/Licenses"

#dir where files will be placed temporarely, before being handled by the postinstall script
BELGIUM_DIR="/Library/BelgiumIdentityCard"

#base name of the package
REL_NAME="beid"
#version number of the package
REL_VERSION_TMP=$(cat ../../../common/src/beidversions.h | grep BEID_PRODUCT_VERSION)
REL_VERSION=$(expr "$REL_VERSION_TMP" : '.*\([0-9].[0-9].[0-9]\).*')

PKCS11_BUNDLE="beid-pkcs11.bundle"
BUILD_NR=$(svn info ../../ | grep Revision | sed s/"Revision: "/""/)
PKG_NAME="$REL_NAME.pkg"
PKG_MAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
#VOL_NAME="$REL_NAME_MacOSX_$REL_VERSION"
#DMG_NAME="$REL_NAME_$REL_VERSION.dmg"


XPI_PLUGIN_DIR="/Library/Application\ Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/belgiumeid@eid.belgium.be"


#directories used to create the installer


LIB_DIR="../../../lib"
BIN_DIR="../../../bin"


#destroy previously build package\clean_release: 
test -e $ROOT_DIR && rm -rdf $ROOT_DIR
test -e $PKG_NAME &&rm -rf $PKG_NAME
test -e $RESOURCES_NO_SVN_DIR &&rm -rf $RESOURCES_NO_SVN_DIR
test -e $INSTALL_SCRIPTS_NO_SVN_DIR &&rm -rf $INSTALL_SCRIPTS_NO_SVN_DIR

#create installer dirs
mkdir -p $ROOT_DIR/$INST_DIR/lib/siscardplugins
mkdir -p $ROOT_DIR/$INST_DIR/lib/beidqt
mkdir -p $ROOT_DIR/$INST_DIR/share/beid/certs
mkdir -p $ROOT_DIR/$LICENSES_DIR
mkdir -p $ROOT_DIR/usr/bin/

#copy all files that should be part of the installer:
#copy third party deliverables
#PATH_TO_QTCORE=$(otool -L ../../../bin/beiddialogsQTsrv | grep QtCore  | sed s/\(.*\)/""/ | tr -d '\t')
#PATH_TO_QTGUI=$(otool -L ../../../bin/beiddialogsQTsrv | grep QtGui  | sed s/\(.*\)/""/ | tr -d '\t')
PATH_TO_QTCORE=/Library/Frameworks/QtCore.framework/Versions/4/QtCore
PATH_TO_QTGUI=/Library/Frameworks/QtGui.framework/Versions/4/QtGui

cp "$PATH_TO_QTCORE" "$ROOT_DIR/$INST_DIR/lib/beidqt/QtCore"
cp "$PATH_TO_QTGUI" "$ROOT_DIR/$INST_DIR/lib/beidqt/QtGui"

#copy eidMW deliverables
cp $LIB_DIR/libsiscardplugin1__ACS__.dylib $ROOT_DIR/$INST_DIR/lib/siscardplugins
cp $LIB_DIR/libbeidpkcs11.$REL_VERSION.dylib $ROOT_DIR/$INST_DIR/lib/
cp $BIN_DIR/beiddialogsQTsrv $ROOT_DIR/usr/bin/

LATEST_XPI=$(readlink ../../../xpi/builds/belgiumeid-CURRENT.xpi)
XPI_PLUGIN=../../../xpi/builds/$LATEST_XPI
cp $XPI_PLUGIN $ROOT_DIR/$BELGIUM_DIR

cp -r ../../../tokend/BEID.tokend $ROOT_DIR/$BELGIUM_DIR
cp -r ../../../tokend/BEID_Lion.tokend $ROOT_DIR/$BELGIUM_DIR

#copy licenses
cp ../../../misc/licenses_files/Dutch/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_NL.txt ; \
cp ../../../misc/licenses_files/English/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_EN.txt ; \
cp ../../../misc/licenses_files/French/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_FR.txt ; \
cp ../../../misc/licenses_files/German/eID-toolkit_licensingtermsconditions.txt \
	$ROOT_DIR/$LICENSES_DIR/license_DE.txt ; \
cp ../../../misc/licenses_files/THIRDPARTY-LICENSES-Mac.txt $ROOT_DIR/$LICENSES_DIR/

#copy certificates
cp ../../../misc/certs/beid-cert-belgiumrca2.der $ROOT_DIR/$INST_DIR/share/beid/certs/

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

	 echo "********** Changing install_names for use with the QT libs **********"
	 install_name_tool -change $PATH_TO_QTCORE \
		$INST_DIR/lib/beidqt/QtCore \
		$ROOT_DIR/usr/bin/beiddialogsQTsrv
	 install_name_tool -change $PATH_TO_QTGUI \
		$INST_DIR/lib/beidqt/QtGui \
		$ROOT_DIR/usr/bin/beiddialogsQTsrv
	 install_name_tool -change $PATH_TO_QTCORE \
		$INST_DIR/lib/beidqt/QtCore \
		$ROOT_DIR/$INST_DIR/lib/beidqt/QtGui

## make sure the .svn files don't get into the package
svn export --force $RESOURCES_DIR $RESOURCES_NO_SVN_DIR
svn export --force $INSTALL_SCRIPTS_DIR $INSTALL_SCRIPTS_NO_SVN_DIR


echo "********** manipulate mw_installer **********"
#	  sed -e "s;REPL_REL_VERSION;$REL_VERSION;g" \
#	  sed -e "s;REL_NAME;$REL_NAME;g" -e "s;PKCS11LIB_NAME_dylib;$PKCS11LIB_NAME_dylib;g" \
#	  	   ./resources/pkcs11.bundle/Contents/Info.plist > \
#	  	   $ROOT_DIR/$INST_DIR/lib/$PKCS11_BUNDLE/Contents/Info.plist

echo "********** generate $PKG_NAME and $DMG_NAME **********"

chmod g+w $ROOT_DIR/$INST_DIR
chmod g+w $ROOT_DIR/$INST_DIR/lib
chmod g+w $ROOT_DIR/$INST_DIR/lib/beidqt
#chmod g+w $ROOT_DIR/$INST_DIR/lib/beidqt/plugins
#chmod g+w $ROOT_DIR/$INST_DIR/lib/beidqt/plugins/imageformats
#chmod a-x $ROOT_DIR/$INST_DIR/etc/beid.conf
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/Info.plist
#chmod a-x $ROOT_DIR/$INST_DIR/lib/beid-pkcs11.bundle/Contents/PkgInfo
chgrp    wheel  $ROOT_DIR/usr
chgrp    wheel  $ROOT_DIR/usr/bin
chgrp    wheel  $ROOT_DIR/usr/local
chgrp    wheel  $ROOT_DIR/usr/local/lib
chgrp    wheel  $ROOT_DIR/usr/local/lib/siscardplugins
chgrp    wheel  $ROOT_DIR/usr/local/lib/beidqt
#chgrp    wheel  $ROOT_DIR/usr/local/lib/beidqt/plugins
chgrp -R admin  $ROOT_DIR/$BELGIUM_DIR/BEID.tokend
chgrp -R admin  $ROOT_DIR/$BELGIUM_DIR/BEID_Lion.tokend

$PKG_MAKER -r $ROOT_DIR -o $PKG_NAME -i $INFO_DIR/Info.plist \
	-e $RESOURCES_NO_SVN_DIR -s $INSTALL_SCRIPTS_NO_SVN_DIR -n REL_VERSION
		 
		 
#	hdiutil create -srcfolder $PKG_NAME -volname "$(VOL_NAME)" $DMG_NAME
