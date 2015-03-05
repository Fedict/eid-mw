#!/bin/bash

#remove former builds
if [ -e "./eID-Installer.dmg" ] 
then 
rm "./eID-Installer.dmg"
fi

if [ -e "./eID-Installer-uncompressed.dmg" ] 
then 
rm "./eID-Installer-uncompressed.dmg"
fi

#Create an uncompressed image
hdiutil create -size 50m -type UDIF -fs HFS+ -volname eID-Installer "./eID-Installer-uncompressed.dmg"

#Open the uncompressed dmg and Copy the bundle into it
hdiutil attach "./eID-Installer-uncompressed.dmg"
chmod 755 "./eID Viewer.app"
cp -R -f "./beid.pkg" /Volumes/eID-Installer/
cp -R -f "./eID Viewer.app" /Volumes/eID-Installer/
ln -s /Applications /Volumes/eID-Installer/Applications
ln -s "/Volumes/eID-Installer/eID Viewer.app" /Volumes/eID-Installer/Test
cp -f "./DMG/DmgBackground.jpg" /Volumes/eID-Installer/
/Developer/Tools/SetFile -a V /Volumes/eID-Installer/DmgBackground.jpg
cp -f "./DMG/.VolumeIcon.icns" /Volumes/eID-Installer/
/Developer/Tools/SetFile -a C /Volumes/eID-Installer/
cp -f "./DMG/DMG_DS_Store" /Volumes/eID-Installer/.DS_Store

#Close the uncompress dmg
hdiutil detach "/Volumes/eID-Installer"

#Convert to Read-Only/Compressed
hdiutil convert -format UDCO "./eID-Installer-uncompressed.dmg" -o "./eID-Installer.dmg"

#Insert the resource file
hdiutil unflatten "./eID-Installer.dmg"
#The SLAResources file contain resources fork data this is not supported by svn.
#So the sla.r file is kept in place of SLAResources
/Developer/Tools/Rez -a "./DMG/sla.r" -o "./eID-Installer.dmg"
hdiutil flatten "./eID-Installer.dmg"

#mv "./eID-Installer.dmg" "./eID-Installer-$build.dmg"