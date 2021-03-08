#!/bin/bash

#remove former builds
if [ -e "./eID-QuickInstaller.dmg" ] 
then 
rm "./eID-QuickInstaller.dmg"
fi

if [ -e "./eID-QuickInstaller-uncompressed.dmg" ] 
then 
rm "./eID-QuickInstaller-uncompressed.dmg"
fi

#Create an uncompressed image (used for new template)
hdiutil create -size 50m -type UDIF -fs HFS+ -volname eID-QuickInstaller "./eID-QuickInstaller-uncompressed.dmg"

#resize the template uncompressed image
#cp ./eID-QuickInstaller-uncompressed-template.dmg ./eID-QuickInstaller-uncompressed.dmg
#hdiutil resize -size 50m ./eID-QuickInstaller-uncompressed.dmg 

#Open the uncompressed dmg and Copy the bundle into it
hdiutil attach "./eID-QuickInstaller-uncompressed.dmg"
#chmod 755 "../../../../ThirdParty/eid-viewer/eID Viewer.app"

###cp ./DS_Store_test "/Volumes/eID-QuickInstaller/.DS_Store"
#remove stubs, then copy the build apps
#rm -R "/Volumes/eID-QuickInstaller/eID Viewer.app"
# cp -R "../../../plugins_tools/eid-viewer/OSX/eID Viewer/build/Release/eID Viewer.app" /Volumes/eID-QuickInstaller/
cp -R "../../../Release/eID Viewer.app" /Volumes/eID-QuickInstaller/
#rm -R "/Volumes/eID-QuickInstaller/eID Middleware.app"
# cp -R "../../../plugins_tools/aboutmw/OSX/eID Middleware/eID Middleware.app" /Volumes/eID-QuickInstaller/
cp -R "../../../plugins_tools/aboutmw/OSX/eID Middleware/Release/eID Middleware.app" /Volumes/eID-QuickInstaller/
cp -R "./DMG/Applications" "/Volumes/eID-QuickInstaller"
cp -R "./DMG/Applications " "/Volumes/eID-QuickInstaller"
cp -R "./DMG/DmgBackground.jpg" "/Volumes/eID-QuickInstaller"
cp -R "./DMG/DMG_DS_Store" "/Volumes/eID-QuickInstaller/.DS_Store"

#cp -f "./DMG/DmgBackground2.jpg" /Volumes/eID-QuickInstaller/
#hide background image
#SetFile -a V /Volumes/eID-QuickInstaller/DmgBackground.jpg
#shift+cmd+G

#open the folder when the volume is mounted
###bless --openfolder "/Volumes/eID-QuickInstaller"

###cp ./DS_Store_test "/Volumes/eID-QuickInstaller/.DS_Store"

###chmod -R -w "/Volumes/eID-QuickInstaller" 
#Close the uncompress dmg
hdiutil detach "/Volumes/eID-QuickInstaller"

#Convert to Read-Only/Compressed
hdiutil convert -format UDCO "./eID-QuickInstaller-uncompressed.dmg" -o "./eID-QuickInstaller.dmg"

##Insert the resource file
hdiutil unflatten "./eID-QuickInstaller.dmg"
##The SLAResources file contain resources fork data this is not supported by svn.
##So the sla.r file is kept in place of SLAResources
Rez -a "./DMG/sla.r" -o "./eID-QuickInstaller.dmg"
hdiutil flatten "./eID-QuickInstaller.dmg"

#mv "./eID-QuickInstaller.dmg" "./eID-QuickInstaller-$build.dmg"
