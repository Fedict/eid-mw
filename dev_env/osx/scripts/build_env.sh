#!/bin/sh

################################################
# Configuration

macportsfile=MacPorts-1.9.2-10.6-SnowLeopard.dmg
macportsurl=http://dl.dropbox.com/u/2715381/buildbot/$macportsfile

# End configuration
################################################
mkdir /tmp/build_env && cd /tmp/build_env

################################################
# Install MacPorts
################################################
echo -- Installing MacPorts
echo     Download from $macportsurl
curl $macportsurl -O
macportsmountpoint=`hdiutil attach $macportsfile | tail -n 1 | cut -f 3`
echo     $macportsfile is mounted on $macportsmountpoint

echo     Install MacPorts
installer -pkg "$macportsmountpoint/MacPorts-1.9.2.pkg" -target /

echo     Detach MacPorts mount
hdiutil detach "$macportsmountpoint"

################################################
# Set PATH for MacPorts
################################################
export PATH=/opt/local/bin:/opt/local/sbin:$PATH

################################################
# Update MacPorts
################################################
echo -- Update MacPorts
port -v selfupdate

################################################
# Install wget port
################################################
echo -- Update MacPorts
port install wget


