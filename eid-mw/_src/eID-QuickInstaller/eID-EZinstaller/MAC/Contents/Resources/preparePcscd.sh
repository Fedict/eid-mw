#!/bin/sh

#echo the script's pid
echo $$

IFS=$'\n' ver=($(sw_vers -productVersion))

if [[ $ver == *10.5* || $ver == *10.6* ]]
then
    for pcscdid in `ps -A -c -o pid,command | grep pcscd | grep -v grep | awk '{print $1}'` ; do
        #kill pcscd nicely if running
  
        if [[ "$pcscdid" != "" ]]
        then
            kill -2 $pcscdid
        
            psactive=$(ps -p $pcscdid | grep pcscd )
            COUNT=0
            pcscdid2=(${psactive[0]})
        
            #Wait until pcscd is stopped
            while  [[ "$pcscdid" == "$pcscdid2" &&  $COUNT -lt 3 ]]; do
                sleep 1
                COUNT=`expr $COUNT + 1`
                psactive=$(ps -p $pcscdid | grep pcscd)
                pcscdid2=(${psactive[0]})
            done
        
            #force kill pcscd if still running
            if  [[ "$pcscdid" == "$pcscdid2" ]]
            then
                kill -9 $pcscdid
        
                psactive=$(ps -p $pcscdid | grep pcscd )
                COUNT=0
                pcscdid2=(${psactive[0]})
        
                #Wait until pcscd is stopped
                while  [[ "$pcscdid" == "$pcscdid2" &&  $COUNT -lt 5 ]]; do
                    sleep 1
                    COUNT=`expr $COUNT + 1`
                    psactive=$(ps -p $pcscdid | grep pcscd)
                    pcscdid2=(${psactive[0]})
                done
            fi
        fi
    done


	#Wait up to 10 seconds until pcscd is started
	COUNT=0
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid=(${pcscdlist[0]})
	while  [[ "$pcscdid" == "" &&  $COUNT -lt 10 ]]; do
		sleep 1
		COUNT=`expr $COUNT + 1`
		IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
		pcscdid=(${pcscdlist[0]})
	done
	if [[ 10 -lt $COUNT ]]
	then
        	/usr/sbin/pcscd -f &
    	    	sleep 3
    	fi

else
	#try to start pcscd
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid=(${pcscdlist[0]})
	if  [[ "$pcscdid" == "" ]]
    	then
        	arch -32 /usr/sbin/pcscd -f
    	fi
    
fi
exit 0
