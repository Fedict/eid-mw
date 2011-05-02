#!/bin/sh
DIRNAME=`dirname $0`

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

if [[ $ver == *10.5.0* || $ver == *10.5.1* || $ver == *10.5.2* || $ver == *10.5.3* || $ver == *10.5.4* || $ver == *10.5.5* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.5.mpkg -target /
        /usr/sbin/installer -pkg $DIRNAME/libusb.pkg -target /
        /usr/sbin/installer -pkg $DIRNAME/ifd_ccid.pkg -target /
    else if [[ $ver == *10.5* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/acr38driver_installer_10.5.mpkg -target /
		launchctl unload -w /Library/LaunchDaemons/org.opensc.pcscd.autostart /
		rm -f /Library/LaunchDaemons/org.opensc.pcscd.autostart /
		/Library/OpenSC/bin/sc-securityd.py active /
        else if [[ $ver == *10.6* ]]; then
            /usr/sbin/installer -pkg $DIRNAME/acr38driver_installer_10.6.mpkg -target /
            /usr/sbin/installer -pkg $DIRNAME/acsccid_installer_10.6.mpkg -target /
        fi
	fi
fi
