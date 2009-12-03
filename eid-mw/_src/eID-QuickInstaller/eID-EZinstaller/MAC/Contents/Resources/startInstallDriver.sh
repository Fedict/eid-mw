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

launchctl unload /Library/LaunchDaemons/org.opensc.pcscd.autostart

#kill pcscd if running
IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
pcscdid=(${pcscdlist[0]})

if [[ "$pcscdid" != "" ]]
then
	kill -9 $pcscdid

	#Wait until pcscd is stopped
	COUNT=0
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid2=(${pcscdlist[0]})
	while  [[ "$pcscdid" == "$pcscdid2" &&  $COUNT -lt 20 ]]; do
		sleep 1
		COUNT=`expr $COUNT + 1`
		IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
		pcscdid2=(${pcscdlist[0]})
	done
fi







