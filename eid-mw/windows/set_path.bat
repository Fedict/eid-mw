::==================================================
::Remove the comment and complete the necessary path
::==================================================

::===============
::LIBRARY and SDK
::===============

::Qt 3.3.4 (needed for the beidlib 2.6)
::=====================================
::@set BEID_DIR_QT_334=...

::Qt 4.5.0 exe (uic, moc and rcc)
::===============================
::@set BEID_DIR_QT_450_EXE=...

::Qt 4.5.0 include
::================
::@set BEID_DIR_QT_450_INCLUDE=...

::Qt 4.5.0 dynamic library
::========================
::@set BEID_DIR_QT_450_DYNAMIC=...

::Qt 4.5.0 build in static library (needed for the QuickInstaller and eDT)
::========================================================================
::@set BEID_DIR_QT_450_STATIC=...

::OpenSSL 0.9.8g
::==============
::@set BEID_DIR_OPENSSL_098G=...

::Xercess 3.1.0
::=============
::@set BEID_DIR_XERCES_310=...

::7zip
::=============
::@set BEID_DIR_7ZIP=...

::MS Platform SDK 2003 
::====================
::(To avoid conflict with OpenSSL this version 
:: does not define OCSP_RESPONSE in the file wincrypt.h)
::@set BEID_DIR_PLATFORMSDK_2003=...

::MS Platform SDK 2008
::====================
::(This version is used in the QuickInstaller 
:: which need the file newdev.h)
::@set BEID_DIR_PLATFORMSDK_2008=...

::=====
::TOOLS
::=====

::Visual Studio 2005
::====================
::@set BEID_DIR_VS_2005=...

::Doxygen (Designed for version 1.5.7)
::====================================
::@set BEID_DIR_DOXYGEN=...

::Swig (Designed for version 1.3.35)
::==================================
::@set BEID_DIR_SWIG=...

::Java JDK (Designed for version 1.4.2)
::=====================================
::@set BEID_DIR_JDK_142=...

::WiX (Designed for version 3.0.4415)
::=====================================
::WiX need to be installed. Then the environment variable %WIX% is available
::@set BEID_DIR_WIX=%WIX%

::Merge Module for MSI
::====================
::@set BEID_DIR_MSM=...
