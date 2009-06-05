# Microsoft Developer Studio Project File - Name="eidlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=eidlib - Win32 DebugLog
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "eidlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eidlib.mak" CFG="eidlib - Win32 DebugLog"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eidlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "eidlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "eidlib - Win32 DebugLog" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "eidlib - Win32 ReleaseLog" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "eidlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\eid\pkcs11\opensc\src\include" /I "$(QTDIR)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 comctl32.lib Rpcrt4.lib ws2_32.lib beidlibopensc.lib beidssleay32.lib beidlibeay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /base:"0x61000000" /dll /machine:I386 /nodefaultlib:"libcmt" /out:"Release/beidlib.dll"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidlib.dll c:\dll	copy $(OutDir)\beidlib.lib c:\Programming\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "eidlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\eid\pkcs11\opensc\src\include" /I "$(QTDIR)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_USE_STATIC_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib Rpcrt4.lib ws2_32.lib beidlibopensc.lib beidssleay32.lib beidlibeay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /base:"0x61000000" /dll /debug /machine:I386 /nodefaultlib:"libcmt" /out:"Debug/beidlibd.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidlibd.dll c:\dll	copy $(OutDir)\beidlibd.lib c:\Programming\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "eidlib - Win32 DebugLog"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "eidlib___Win32_DebugLog"
# PROP BASE Intermediate_Dir "eidlib___Win32_DebugLog"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLog"
# PROP Intermediate_Dir "DebugLog"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\eid\pkcs11\opensc\src\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_USE_STATIC_LIB" /D "__LOGGING_CXX" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\eid\pkcs11\opensc\src\include" /I "$(QTDIR)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "__LOGGING_CXX" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswd.lib comctl32.lib Rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib belpic.lib eid_ssleay32.lib eid_libeay32.lib /nologo /base:"0x61000000" /dll /debug /machine:I386 /nodefaultlib:"libcmt" /out:"Debug/eidlibd.dll" /pdbtype:sept
# ADD LINK32 wxmswd.lib comctl32.lib Rpcrt4.lib ws2_32.lib belpic.lib beidssleay32.lib beidlibeay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /base:"0x61000000" /dll /debug /machine:I386 /nodefaultlib:"libcmt" /out:"DebugLog/eidlibd.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\DebugLog
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidlibd.dll c:\dll	copy $(OutDir)\beidlibd.lib c:\Programming\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "eidlib - Win32 ReleaseLog"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "eidlib___Win32_ReleaseLog"
# PROP BASE Intermediate_Dir "eidlib___Win32_ReleaseLog"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseLog"
# PROP Intermediate_Dir "ReleaseLog"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\eid\pkcs11\opensc\src\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\eid\pkcs11\opensc\src\include" /I "$(QTDIR)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EIDLIB_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "__LOGGING_CXX" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw.lib comctl32.lib Rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib belpic.lib eid_ssleay32.lib eid_libeay32.lib /nologo /base:"0x61000000" /dll /machine:I386 /nodefaultlib:"libcmt"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 comctl32.lib Rpcrt4.lib ws2_32.lib beidlibopensc.lib beidssleay32.lib beidlibeay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /base:"0x61000000" /dll /machine:I386 /nodefaultlib:"libcmt"
# Begin Special Build Tool
OutDir=.\ReleaseLog
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidlib.dll c:\dll	copy $(OutDir)\beidlib.lib c:\Programming\Lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "eidlib - Win32 Release"
# Name "eidlib - Win32 Debug"
# Name "eidlib - Win32 DebugLog"
# Name "eidlib - Win32 ReleaseLog"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\APDU.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\BEIDApp.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\bytearray.cpp
# End Source File
# Begin Source File

SOURCE=.\CardReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Certif.cpp
# End Source File
# Begin Source File

SOURCE=.\CertifManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandAPDU.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\config.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\datafile.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\downloadfile.cpp
# End Source File
# Begin Source File

SOURCE=.\eidlib.cpp
# End Source File
# Begin Source File

SOURCE=.\eidlib.rc
# End Source File
# Begin Source File

SOURCE=.\OpenSCReader.cpp
# End Source File
# Begin Source File

SOURCE=.\processapi.cpp
# End Source File
# Begin Source File

SOURCE=.\ResponseAPDU.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLV.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLVBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Verify.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\VirtualReader.cpp
# End Source File
# Begin Source File

SOURCE=.\wxwalldlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\APDU.h
# End Source File
# Begin Source File

SOURCE=.\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\BEIDApp.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\beidconst.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\bytearray.h
# End Source File
# Begin Source File

SOURCE=.\CardReader.h
# End Source File
# Begin Source File

SOURCE=.\Certif.h
# End Source File
# Begin Source File

SOURCE=.\CertifManager.h
# End Source File
# Begin Source File

SOURCE=.\CommandAPDU.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\Config.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\datafile.h
# End Source File
# Begin Source File

SOURCE=.\eidlib.h
# End Source File
# Begin Source File

SOURCE=.\OpenSCReader.h
# End Source File
# Begin Source File

SOURCE=.\processapi.h
# End Source File
# Begin Source File

SOURCE=.\ResponseAPDU.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLV.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLVBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Verify.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\VirtualReader.h
# End Source File
# Begin Source File

SOURCE=.\wxwalldlg.h

!IF  "$(CFG)" == "eidlib - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing wxwalldlg.h...
InputDir=.
InputPath=.\wxwalldlg.h
InputName=wxwalldlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing wxwalldlg.h...
InputDir=.
InputPath=.\wxwalldlg.h
InputName=wxwalldlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 DebugLog"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing wxwalldlg.h...
InputDir=.
InputPath=.\wxwalldlg.h
InputName=wxwalldlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 ReleaseLog"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing wxwalldlg.h...
InputDir=.
InputPath=.\wxwalldlg.h
InputName=wxwalldlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\walldialog.ui

!IF  "$(CFG)" == "eidlib - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing walldialog.ui...
InputDir=.
InputPath=.\walldialog.ui
InputName=walldialog

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing walldialog.ui...
InputDir=.
InputPath=.\walldialog.ui
InputName=walldialog

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 DebugLog"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing walldialog.ui...
InputDir=.
InputPath=.\walldialog.ui
InputName=walldialog

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "eidlib - Win32 ReleaseLog"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing walldialog.ui...
InputDir=.
InputPath=.\walldialog.ui
InputName=walldialog

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_walldialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_wxwalldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\walldialog.cpp
# End Source File
# Begin Source File

SOURCE=.\walldialog.h
# End Source File
# End Group
# End Target
# End Project
