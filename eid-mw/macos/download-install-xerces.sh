#!/bin/sh
if [[ ! -e "/usr/local/lib/libxerces-c.28.0.dylib" ]]
then
	mkdir -p ../_src/ThirdParty/Xerces/Xerces-2.8.0-mac/include
	mkdir -p ../_src/ThirdParty/Xerces/Xerces-2.8.0-mac/lib
	cd ../_src/ThirdParty/Xerces/Xerces-2.8.0-mac

	# Download xerces
	ftp http://apache.mogo.be/xerces/c/2/binaries/xerces-c_2_8_0-x86-macosx-gcc_4_0.tar.gz
	ftp http://apache.mogo.be/xerces/c/2/binaries/xerces-c_2_8_0-powerpc-macosx-gcc_4_0.tar.gz

	# Unzip tarballs
	tar -xzf xerces-c_2_8_0-x86-macosx-gcc_4_0.tar.gz
	tar -xzf xerces-c_2_8_0-powerpc-macosx-gcc_4_0.tar.gz

	# Make a universal binary
	lipo xerces-c_2_8_0-powerpc-macosx-gcc_4_0/lib/libxerces-c.28.0.dylib xerces-c_2_8_0-x86-macosx-gcc_4_0/lib/libxerces-c.28.0.dylib -output lib/libxerces-c.28.0.dylib -create

	# Install in /usr/local/lib
	sudo mkdir -p /usr/local/lib
	sudo cp lib/libxerces-c.28.0.dylib /usr/local/lib/
	sudo ln -s /usr/local/lib/libxerces-c.28.0.dylib /usr/local/lib/libxerces-c.28.dylib

	cp -R xerces-c_2_8_0-x86-macosx-gcc_4_0/include/* include
	cd -

fi