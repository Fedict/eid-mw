# Microsoft Developer Studio Project File - Name="eidviewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=eidviewer - Win32 Debug_win9x
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "eidviewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eidviewer.mak" CFG="eidviewer - Win32 Debug_win9x"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eidviewer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "eidviewer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "eidviewer - Win32 Release_win9x" (based on "Win32 (x86) Console Application")
!MESSAGE "eidviewer - Win32 Debug_win9x" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "eidviewer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\eid\pkcs11\opensc\src\include" /I "." /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_WINDOWS" /D "__WXMSW__" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /FD /c
# ADD BASE RSC /l 0x813 /d "NDEBUG"
# ADD RSC /l 0x813 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 winmm.lib jpeg.lib wxmswu.lib beidlib.lib beidlibopensc.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib beidlibeay32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"Release/beidgui.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\eid\pkcs11\opensc\src\include" /I "." /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "_DEBUG" /D "_WINDOWS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /D "__WXMSW__" /D "__WXDEBUG__" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x813 /d "_DEBUG"
# ADD RSC /l 0x813 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmswud.lib jpegd.lib beidlibd.lib beidlibopensc.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib beidlibeay32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt" /out:"Debug/beidgui.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Release_win9x"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "eidviewer___Win32_Release_win9x"
# PROP BASE Intermediate_Dir "eidviewer___Win32_Release_win9x"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_win9x"
# PROP Intermediate_Dir "Release_win9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_STLP_USE_STATIC_LIB" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\eid\pkcs11\opensc\src\include" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WXMSW__" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /U "UNICODE" /U "_UNICODE" /U "wxUSE_UNICODE" /FR /FD /c
# SUBTRACT CPP /u
# ADD BASE RSC /l 0x813 /d "NDEBUG"
# ADD RSC /l 0x813 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib jpeg.lib wxmswu.lib beidlib.lib belpic.lib libeay32.lib crypt32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib jpeg.lib wxmsw.lib beidlib.lib beidlibopensc.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib beidlibeay32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"Release_win9x/beidgui_win9x.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Debug_win9x"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "eidviewer___Win32_Debug_win9x"
# PROP BASE Intermediate_Dir "eidviewer___Win32_Debug_win9x"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win32_Debug_win9x"
# PROP Intermediate_Dir "Win32_Debug_win9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D wxUSE_UNICODE=1 /D "_STLP_USE_STATIC_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\eid\pkcs11\opensc\src\include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WXDEBUG__" /D "_STLP_USE_STATIC_LIB" /D "_STLP_NEW_PLATFORM_SDK" /U "_UNICODE" /U "UNICODE" /U "wxUSE_UNICODE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x813 /d "_DEBUG"
# ADD RSC /l 0x813 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswud.lib beidlibd.lib belpic.lib libeay32.lib crypt32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib beidlibd.lib beidlibopensc.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib beidlibeay32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt" /out:"Win32_Debug_win9x/beidgui_win9x.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "eidviewer - Win32 Release"
# Name "eidviewer - Win32 Debug"
# Name "eidviewer - Win32 Release_win9x"
# Name "eidviewer - Win32 Debug_win9x"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\config\BelgianEIDConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\bytearray.cpp
# End Source File
# Begin Source File

SOURCE=.\Certif.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\eidviewer.rc
# End Source File
# Begin Source File

SOURCE=.\eidviewerApp.cpp

!IF  "$(CFG)" == "eidviewer - Win32 Release"

# ADD CPP /Yc"wx/wxprec.h"

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Debug"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Release_win9x"

# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"

!ELSEIF  "$(CFG)" == "eidviewer - Win32 Debug_win9x"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\eidviewerFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\eidviewerNotebook.cpp
# End Source File
# Begin Source File

SOURCE=.\eidviewerTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\config\KeyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Pin.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintOut.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintOutData.cpp
# End Source File
# Begin Source File

SOURCE=.\config\ScConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\config\SlotInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\verinfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\config\BelgianEIDConfig.h
# End Source File
# Begin Source File

SOURCE=.\Certif.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\eidviewerApp.h
# End Source File
# Begin Source File

SOURCE=.\eidviewerFrame.h
# End Source File
# Begin Source File

SOURCE=.\eidviewerNotebook.h
# End Source File
# Begin Source File

SOURCE=.\eidviewerTypes.h
# End Source File
# Begin Source File

SOURCE=.\config\KeyInfo.h
# End Source File
# Begin Source File

SOURCE=.\Pin.h
# End Source File
# Begin Source File

SOURCE=.\PrintOut.h
# End Source File
# Begin Source File

SOURCE=.\PrintOutData.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\config\ScConfig.h
# End Source File
# Begin Source File

SOURCE=.\config\SlotInfo.h
# End Source File
# Begin Source File

SOURCE=.\verinfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\res\eidviewer.ico
# End Source File
# Begin Source File

SOURCE=.\res\eidviewer.xpm
# End Source File
# Begin Source File

SOURCE=.\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\watch1.cur
# End Source File
# End Group
# End Target
# End Project
