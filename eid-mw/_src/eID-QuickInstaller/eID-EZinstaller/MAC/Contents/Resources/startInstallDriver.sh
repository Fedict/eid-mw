#!/bin/sh
DIRNAME=`dirname $0`

#echo the script's pid
echo $$

IFS=$'\n' ver=($(sw_vers -productVersion))

if [[ ($ver == *10.5*) && ($ver < "10.5.6") ]]; then
		/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.5.mpkg -target /
        /usr/sbin/installer -pkg $DIRNAME/libusb.pkg -target /
        /usr/sbin/installer -pkg $DIRNAME/ifd_ccid.pkg -target /
    else if [[ $ver == *10.5* || $ver == *10.6* ]]; then
            /usr/sbin/installer -pkg $DIRNAME/acr38driver_installer_1.7.11.mpkg -target /
            /usr/sbin/installer -pkg $DIRNAME/acsccid_installer_10.6.mpkg -target /
		else if [[ $ver == *10.7* || $ver == *10.8* ]]; then
			/usr/sbin/installer -pkg $DIRNAME/acr38driver_installer_1.7.11_10.7.mpkg -target /
			/usr/sbin/installer -pkg $DIRNAME/acsccid_installer_10.7.mpkg -target /
				else if [[ $ver == *10.9* ]]; then
					/usr/sbin/installer -pkg $DIRNAME/acsccid_installer_10.9.mpkg -target /
				fi
		fi
	fi
fi
