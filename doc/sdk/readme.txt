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
- Linux:
- Mac: OSX 10.5 (Leopard) Intel, OSX 10.6 (SnowLeopard)

Software requirements
=====================
The SDK must be used together with the eID 4.0 runtime.

Windows:
- MS Visual Studio 2010 express examples are included

Linux:
- default g++ should be installed

Mac OSX:
- xcodeproj c example is included


About the SDK
=============
This package contains all necessary files to build applications based on the eID 4.0
middleware. The following programming languages are supported:

- Windows: C, C++, C#
- Linux:   C, C++
- Mac OSX: C, C++

The SDK is available in the following formats:
- Windows: zip file
- Linux: zip file
- Mac OSX: zip file


SDK Installation
================
The eID MW 4.0 runtime should be installed before the SDK can be used,
as it contains our pkcs11 implementation, which is essentially the SDK.

To get the SDK, you just need the directory which contains this
readme.txt file. You can get it by cloning the git repository, or by
just downloading the zip file from github. It is preferable to use
latest release for the operating system you're using, rather than to use
the "master" branch (which may or may not be production-ready at any
point in time).

Contents of the SDK
===================
The package contains all the required files to use the eID 4.0 library.

Description (depending of the platform):
- [Belgium Identity Card SDK,beidsdk]/pkcs11/include/rsaref220	: eID C include files
- [Belgium Identity Card SDK,beidsdk]/doc/			: eID SDK documentation
- [Belgium Identity Card SDK,beidsdk]/examples/			: eID SDK examples

API Documentation
=================
The SDK 4.0 uses the pkcs11 v2.11 API. This API is explained in the following document:
“RSA Security Inc. Public-Key Cryptography Standards (PKCS)”, this document can be found
in the doc folder (pkcs-11v2-11r1.pdf).

A document that contains the list of names we gave to (search for) the eid carddata,
can also be found in the documents folder.

Samples
=======
The 'examples' subdirectory contains the source code for sample applications in different
programming languages (depending of the platform).

Test cards
==========
Before test cards can be used, set the cert_allow_testcard configuration option to '1'
as follows:

 - On Windows: create the following REG_DWORD and set its value to 1:
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

 - On Windows: create the following REG_DWORD and set its value to 1:
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
