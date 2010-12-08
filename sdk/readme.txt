Belgium eID 4.0 middleware SDK
==============================

Contents
========
- Platform requirements
- Software requirements
- About the SDK
- SDK Installation
- Contents of the SDK
- API Documentation
- Samples
- Test cards
- Logging
- Known issues


Platform Requirements
======================
The Sdk is tested on the following platforms:
- Windows: XP SP3, Vista, Windows 7
- Linux: fedora 14, Ubuntu 10.10
- Mac: OSX 10.5 (Leopard) Intel, OSX 10.6 (SnowLeopard)

Software requirements
=====================
The SDK must be used together with the eID 4.0 runtime.

Windows:
- MS Visual Studio 2005 or higher should be installed

Linux:
- default g++ should be installed

Mac OSX:
- default g++ should be installed


About the SDK
=============
This package contains all necessary files to build applications based on the eID 4.0
middleware. The following programming languages are supported:

- Windows: C, 
- Linux:   C, 
- Mac OSX: C, 

The SDK is available in the following formats:
- Windows: MSI file
- Linux: tgz file
- Mac OSX: dmg file


SDK Installation
================
The eID MW 4.0 runtime should be installed before the SDK can be used.

- Windows: follow the instructions of the MSI installation
           the SDK is installed in: 
	C:\Documents and Settings\<user>\My Documents\Belgium Identity Card SDK 40
- Linux: extract the files from the tgz file to the directory of your choice
- Mac OSX: open dmg file and optionally copy the mounted directory

Contents of the SDK
===================
The package contains all the required files to use the eID 4.0 library.

Description (depending of the platform):
- [Belgium Identity Card SDK,beidsdk]/pkcs11/include/rsaref220	: eID C include files
- [Belgium Identity Card SDK,beidsdk]/doc/eidlib/C		: eID lib C API documentation

API Documentation
=================
HTML API documentation for the Middleware is available in the 'doc' subdirectory for the 
different languages (depending on the platform).

Samples
=======
The 'samples' subdirectory contains the source code for sample applications in different
programming languages (depending of the platform).

Test cards
==========
Before test cards can be used, set the cert_allow_testcard configuration option to '1'
as follows:

 - On Windows: create the following REG_DWORD and set it's value to 1:
     HKEY_CURRENT_USER\Software\BEID\certificatevalidation\cert_allow_testcard
 - On Linux: create/edit the file ~/.config/beid.conf to add the following:
     [certificatevalidation]
     cert_allow_testcard=1
 - On Mac OS X: create/edit the file ~/Library/Preferences/beid.conf to add the following:
     [certificatevalidation]
     cert_allow_testcard=1

Remark (Linux, Mac OS X): the section [certificatevalidation] may already exist.
In this case, just add the 'cert_allow_testcard' line.

Logging
=======
For extra logging, set the log_level configuration option to 'debug'
as follows:

 - On Windows: create the following REG_DWORD and set it's value to 1:
     HKEY_CURRENT_USER\Software\BEID\logging\log_level
 - On Linux: create/edit the file ~/.config/beid.conf to add the following:
     [logging]
     log_level=debug
 - On Mac OS X: create/edit the file ~/Library/Preferences/beid.conf to add the following:
     [logging]
     log_level=debug

The log files can be found:
 - On Windows:  <userhome>\Application Data\.BEID_x.log
 		with <userhome>: <disk>:\Documents and Settings\<username>\Application Data
 - On Linux, Mac OS X: ~/.BEID_x.log


Known issues
============


