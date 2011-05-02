#!/bin/sh

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

if [[ $ver == *10.5.0* || $ver == *10.5.1* || $ver == *10.5.2* || $ver == *10.5.3* || $ver == *10.5.4* || $ver == *10.5.5* || $ver == *10.6* ]]
then
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

    Launchctl load /Library/LaunchDaemons/org.opensc.pcscd.autostart

    sleep 3
fi

#start pcscd if not running
DAEMON=`ps -A -c -o pid,command | grep pcscd`
if [[ "$DAEMON" == "" ]]
then
	/usr/sbin/pcscd -f &

	#Wait until pcscd is started
	COUNT=0
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid=(${pcscdlist[0]})
	while  [[ "$pcscdid" == "" &&  $COUNT -lt 20 ]]; do
		sleep 1
		COUNT=`expr $COUNT + 1`
		IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
		pcscdid=(${pcscdlist[0]})
	done
fi

exit 0
