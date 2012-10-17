#!/bin/bash

# java 1.4.2 is not supported any more since oct 2008
MINJAVAVER=1.5
MACHINETYPE=i386
MWVER=4.0.4

#########################################
# we must be running on intel
#########################################
echo "[Info ] Verifying machine type..."
MACHINE=`uname -p`
if [ "$MACHINE" != "i386" ]
then
	echo "[Error] Incorrect machine type. Must be $MACHINETYPE."
	exit -1
else
	echo "[Info ] Machine type: $MACHINE."
fi

#########################################
# we must be on a 10.5 or later
#########################################
echo "[Info ] Verifying OSX version..."
IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

if [[ $ver == *10.5* || $ver == *10.6* ]]
then
    echo "[Info ] OSX version: $ver[1]."
else  
	echo "[Error] Incorrect OS version: $ver. Must be $OSXVER."
	exit -1	
fi


#########################################
# check gcc compiler installed
#########################################
GCC=`which gcc 2> /dev/null | wc -l`
echo "[Info ] Verifying gcc compiler..."
if [[ $GCC -eq 0 ]]
then
	echo "[Error] gcc compiler not found. Please verify or install."
	exit -1

else
	GCCVER=`gcc --version`
	echo "[Info ] Installed. gcc version: $GCCVER"
fi

#########################################
# check g++ compiler installed
#########################################
echo "[Info ] Verifying g++ compiler..."
GPP=`which g++ 2> /dev/null | wc -l`
if [ $GPP -eq 0  ]
then
	echo "[Error] g++ compiler not found. Please verify or install."
	exit -1
else
	GPPVER=`g++ --version`
	echo "[Info ] Installed. g++ version: $GPPVER"
fi

#########################################
# check xerces installed 
#########################################
XERCESCLIB=libxerces-c-3.1.dylib
XERCESCPATH=../_src/ThirdParty/Xerces/Xerces-3.1.0-mac
echo "[Info ] Verifying xerces libraries..."
if [ ! -e $XERCESCPATH/lib/$XERCESCLIB ]
then
	echo "[Error] xerces library not found at $XERCESCPATH/lib. Please verify or install."
	exit -1
else
	echo "[Info ] Installed"
fi

echo "[Info ] Verifying xerces include..."
XERCESCINC=SAXParser.hpp
if [ ! -e $XERCESCPATH/include/xercesc/parsers/$XERCESCINC ]
then
	echo "[Error] xerces include file not found at $XERCESPATH//include/xercesc/parsers/. Please verify or install."
	exit -1
else
	echo "[Info ] Installed"
fi


#-----------------------------------------
# create a pkg and dmg file
# The makefile requires
#-----------------------------------------
cd ../_src/eidmw/_Builds
sudo make -f Makefile_mac_bwc release QTBASEDIR=$QTBASEDIR UNIVERSAL=${UNIVERSAL}
cd -

BUILDNR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`

#-----------------------------------------
# move the beid_bwc.pkg to the quickinstaller and keep a copy locally
#-----------------------------------------
sudo rm -rf beid_bwc-$BUILDNR.pkg

sudo cp -r /release_build/beid_bwc.pkg .
sudo mv beid_bwc.pkg beid_bwc-$BUILDNR.pkg

#Detach and delete old dmg
hdiutil detach /Volumes/beid_bwc-$MWVER

if [ -e beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg ]
then
	rm beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg
fi
if [ -e beid_bwc-$MWVER-$BUILDNR.dmg ]
then
	rm beid_bwc-$MWVER-$BUILDNR.dmg
fi

#Create an uncompressed image
hdiutil create -size 50m -type UDIF -fs HFS+ -volname  beid_bwc-$MWVER beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg

#Open the uncompressed dmg and Copy the bundle into it
hdiutil attach beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg
sudo cp -R -f beid_bwc-$BUILDNR.pkg /Volumes/beid_bwc-$MWVER/

#Close the uncompress dmg
hdiutil detach /Volumes/beid_bwc-$MWVER

#Convert to Read-Only/Compressed
hdiutil convert -format UDCO beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg -o beid_bwc-$MWVER-$BUILDNR.dmg
rm beid_bwc-$MWVER-$BUILDNR-uncompressed.dmg

