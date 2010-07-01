#!/bin/sh
LANG=$1
DIRNAME=`dirname $0`

launchctl load /Library/LaunchDaemons/org.opensc.pcscd.autostart

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

	sleep 10
fi

exit 0
