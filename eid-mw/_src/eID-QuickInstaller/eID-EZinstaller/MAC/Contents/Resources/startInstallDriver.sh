#!/bin/sh
DIRNAME=`dirname $0`

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

#ACR38
if [[ $ver == *10.4* ]]; then
	/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.4.mpkg -target /
else if [[ $ver == *10.5* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.5.mpkg -target /
	else if [[ $ver == *10.6* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.6.mpkg -target /
		fi
	fi
fi

#SPR532
if [[ $ver == *10.5* ]]; then
	/usr/sbin/installer -pkg $DIRNAME/libusb.pkg -target /
	/usr/sbin/installer -pkg $DIRNAME/ifd_ccid.pkg -target /
fi
