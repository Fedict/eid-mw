#!/bin/bash
#-----------------------------------------
# this script will build the eID MW project on linux
# it will call all necessary scripts to build and will log
# the build process to a file build_eidmw_<distro>_<buildnr>.log
#-----------------------------------------
DISTRO=none
DIR_CURR=`pwd`
PROJ_NAME=beid-2.6
DIR_PROJ=~${DIR_CURR}

LOCALBUILD=0

until [ -z "$1" ]
do
	if [ $1 == "local" ]
	then
		LOCALBUILD=$1
		echo "[Info] Local build beid-2.6."
	else if [ $1 == "help" ]
	then
		echo "$0 [local] "
		echo "local: build the eID MW on the local machine."
		exit -1
	fi
	fi
	shift
done

if [ -e "/etc/fedora-release" ]
then
	DISTRO=fedora
else
if [ -e "/etc/SuSE-release" ]
then
	DISTRO=suse
else
if [ -e "/etc/debian_version" ]
then
	DISTRO=debian
else
	echo "[Error] Unsupported Linux distribution."
	echo "[Error] Done..."
	exit -1
fi
fi
fi

#if [ $LOCALBUILD != "local" ]
#then
#	./getsvnrevision.pl
#fi
#
#if [ ! -e "svn_revision" ]
#then
#	echo "[Error] File svn_revision not found"
#	exit -1
#fi
#
#BUILD_NR=`cat svn_revision`
#LOGFILE=build_${PROJ_NAME}_${DISTRO}_${BUILD_NR}.log

source ${DIR_CURR}/build_environment_${DISTRO}.source
chmod +x configure
./configure
#make 2>&1 | tee ${LOGFILE}
make 2>&1
