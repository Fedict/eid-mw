# Microsoft Developer Studio Project File - Name="Belpic PCSC Service" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Belpic PCSC Service - Win32 DebugRemote
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Belpic PCSC Service.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Belpic PCSC Service.mak" CFG="Belpic PCSC Service - Win32 DebugRemote"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Belpic PCSC Service - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Belpic PCSC Service - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Belpic PCSC Service - Win32 DebugRemote" (based on "Win32 (x86) Application")
!MESSAGE "Belpic PCSC Service - Win32 ReleaseRemote" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Belpic PCSC Service - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT" /out:"Release/beidservicepcsc.exe"

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "_DEBUGLOG" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMTD" /nodefaultlib:"libcmt" /nodefaultlib:"msvcrt" /out:"Debug/beidservicepcsc.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 DebugRemote"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Belpic_PCSC_Service___Win32_DebugRemote"
# PROP BASE Intermediate_Dir "Belpic_PCSC_Service___Win32_DebugRemote"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Belpic_PCSC_Service___Win32_DebugRemote"
# PROP Intermediate_Dir "Belpic_PCSC_Service___Win32_DebugRemote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "_PRIVACY_REMOTE_SVC" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMTD" /nodefaultlib:"libcmt" /out:"Debug/beidservicepcsc.exe" /pdbtype:sept
# ADD LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMTD" /nodefaultlib:"libcmt" /out:"Debug/beidservicepcscr.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 ReleaseRemote"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Belpic_PCSC_Service___Win32_ReleaseRemote"
# PROP BASE Intermediate_Dir "Belpic_PCSC_Service___Win32_ReleaseRemote"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseRemote"
# PROP Intermediate_Dir "ReleaseRemote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "BELPIC_PIN_PAD" /D "_STLP_NEW_PLATFORM_SDK" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "_PRIVACY_REMOTE_SVC" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT" /out:"Release/beidservicepcsc.exe"
# ADD LINK32 comctl32.lib winscard.lib beidlibeay32.lib beidssleay32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winspool.lib winmm.lib wsock32.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT" /out:"ReleaseRemote/beidservicepcscr.exe"

!ENDIF 

# Begin Target

# Name "Belpic PCSC Service - Win32 Release"
# Name "Belpic PCSC Service - Win32 Debug"
# Name "Belpic PCSC Service - Win32 DebugRemote"
# Name "Belpic PCSC Service - Win32 ReleaseRemote"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ApplicationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplicationObject.cpp
# End Source File
# Begin Source File

SOURCE=".\Belpic PCSC Service.rc"
# End Source File
# Begin Source File

SOURCE=..\beidcommon\bytearray.cpp
# End Source File
# Begin Source File

SOURCE=.\CardChangeMonitor.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\config.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\datafile.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\maindialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\PCSCManager.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\pinpad.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\qtservice.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\qtservice_win.cpp
# End Source File
# Begin Source File

SOURCE=.\ReaderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ReaderState.cpp
# End Source File
# Begin Source File

SOURCE=.\SCardHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLV.cpp
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLVBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\TransactionStack.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ApplicationManager.h
# End Source File
# Begin Source File

SOURCE=.\ApplicationObject.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\bytearray.h
# End Source File
# Begin Source File

SOURCE=.\CardChangeMonitor.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\Config.h
# End Source File
# Begin Source File

SOURCE=.\maindialogimpl.h

!IF  "$(CFG)" == "Belpic PCSC Service - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing maindialogimpl.h...
InputDir=.
InputPath=.\maindialogimpl.h
InputName=maindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing maindialogimpl.h...
InputDir=.
InputPath=.\maindialogimpl.h
InputName=maindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 DebugRemote"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing maindialogimpl.h...
InputDir=.
InputPath=.\maindialogimpl.h
InputName=maindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 ReleaseRemote"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing maindialogimpl.h...
InputDir=.
InputPath=.\maindialogimpl.h
InputName=maindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PCSCManager.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\pinpad.h
# End Source File
# Begin Source File

SOURCE=.\ReaderObject.h
# End Source File
# Begin Source File

SOURCE=.\ReaderState.h
# End Source File
# Begin Source File

SOURCE=.\SCardHandler.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLV.h
# End Source File
# Begin Source File

SOURCE=..\beidcommon\TLVBuffer.h
# End Source File
# Begin Source File

SOURCE=.\TransactionStack.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\maindialog.cpp
# End Source File
# Begin Source File

SOURCE=.\maindialog.h
# End Source File
# Begin Source File

SOURCE=.\moc_maindialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_maindialogimpl.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\maindialog.ui

!IF  "$(CFG)" == "Belpic PCSC Service - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing maindialog.ui...
InputDir=.
InputPath=.\maindialog.ui
InputName=maindialog

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

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing maindialog.ui...
InputDir=.
InputPath=.\maindialog.ui
InputName=maindialog

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

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 DebugRemote"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing maindialog.ui...
InputDir=.
InputPath=.\maindialog.ui
InputName=maindialog

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

!ELSEIF  "$(CFG)" == "Belpic PCSC Service - Win32 ReleaseRemote"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UICing maindialog.ui...
InputDir=.
InputPath=.\maindialog.ui
InputName=maindialog

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
# End Target
# End Project
