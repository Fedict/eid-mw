#!/bin/bash

if [ "$(id -u)" != "0" ]
then
   echo "[Error] This script must be run as root" 1>&2
   exit 1
fi

echo "[Info ] Verifying installed packages..."
#-----------------------------------------
# Check the linux distribution and the necessary installed packages
#-----------------------------------------
if [ -e "/etc/fedora-release" ]
then
	PACKAGES=(pcsc-lite-devel
		 gcc
		 gcc-c++
		 make
		 openssl-devel
		 xerces-c27-devel-2.7.0
		 swig
		 pcsc-lite
		 doxygen
		 qt3-devel
		 compat-wxGTK26-devel-2.6.4
		 ccid
		 )
	for pkg in ${PACKAGES[@]}
	do
		echo "[Info ] Installing the package ${pkg}..."
		yum install ${pkg}
	done

		LINKS=( /usr/bin/wx-config )

		for lnk in ${LINKS[@]}
		do
			echo "[Info ] Checking link: $lnk"
			if [ -e $lnk ]
			then
				echo "[Info ] ok, link $lnk exists"
			else
				ln -s /usr/bin/wx-2.6-config $lnk
				echo "[Info ] The link $lnk is created."
			fi
		done
else
	echo "[Error] Unsupported Linux distribution."
	echo "[Error] Done..."
	exit -1
fi

MINQT4VER=4.5.0
pkg=qmake

echo "[Info ] Checking Qt4"
QT4=`which $pkg 2> /dev/null | wc -l`
if [ $QT4 -eq 0 ]
then
	echo "[Error] qmake can not be found."
	echo "[Error] Set the PATH environment variable or download/install Qt4 and try again."
	exit -1
else
	#-----------------------------------------
	# parse the version string assumed to be in the format: XXX.YYY.ZZZ
	#-----------------------------------------
	QT4=`which qmake`
	THEPATH=${QT4%/*}
	QT4VERSION=`${THEPATH}/${pkg} -v 2>&1`
	VERSIONSTR=`expr match "${QT4VERSION}" '.*\([1-9]\+\.[0-9]\+\.[0-9]\+\)'`
	if [[ "${VERSIONSTR}" = "${MINQT4VER}" ]]
	then
		echo "[Info ] ${pkg} version ${VERSIONSTR} found at ${THEPATH}"
	else if [[ "${VERSIONSTR}" > "${MINQT4VER}" ]]
		then
			echo "[Warn ] ${pkg} version ${VERSIONSTR} found at ${THEPATH}"
			echo "[Warn ] The Qt4 version used is ideally ${MINQT4VER}"
			echo "[Warn ] If needed, stop the process and install Qt4 ${MINQT4VER}"
		else
			if [ -e "/etc/alternatives/qmake" ]
			then
				rm /etc/alternatives/qmake
			fi

			QT4=`which /usr/local/Trolltech/*/bin/qmake`
			ln -s ${QT4} /etc/alternatives/qmake

			if [ -e "/etc/alternatives/moc" ]
			then
				rm /etc/alternatives/moc
			fi

			QT4=`which /usr/local/Trolltech/*/bin/moc`
			ln -s ${QT4} /etc/alternatives/moc

			if [ -e "/etc/alternatives/uic" ]
			then
				rm /etc/alternatives/uic
			fi

			QT4=`which /usr/local/Trolltech/*/bin/uic`
			ln -s ${QT4} /etc/alternatives/uic
		fi
	fi
fi

