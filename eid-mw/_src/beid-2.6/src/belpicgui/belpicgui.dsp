# Microsoft Developer Studio Project File - Name="belpicgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=belpicgui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "belpicgui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "belpicgui.mak" CFG="belpicgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "belpicgui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "belpicgui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "belpicgui - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BELPICGUI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCGUI_DLL" /D "SCGUI_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_USE_STATIC_LIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x813 /d "NDEBUG"
# ADD RSC /l 0x813 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /machine:I386 /out:"Release/beidgui.dll"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidgui.dll c:\dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BELPICGUI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCGUI_DLL" /D "SCGUI_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_USE_STATIC_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x813 /d "_DEBUG"
# ADD RSC /l 0x813 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mt334.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /debug /machine:I386 /out:"Debug/beidgui.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\beidgui.dll c:\dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "belpicgui - Win32 Release"
# Name "belpicgui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\askmessagedialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\authenticationdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\belpicgui.cpp
# End Source File
# Begin Source File

SOURCE=.\belpicgui.rc
# End Source File
# Begin Source File

SOURCE=.\changepindialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\images.tmp

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\images.tmp
InputName=images

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed ileidgui -f "$(InputDir)\$(InputName).tmp" -o qmake_image_collection.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\images.tmp
InputName=images

"qmake_image_collection.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic -embed ileidgui -f "$(InputDir)\$(InputName).tmp" -o qmake_image_collection.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\insertcarddialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PINDialog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\askmessagedialogimpl.h

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing askmessagedialogimpl.h...
InputDir=.
InputPath=.\askmessagedialogimpl.h
InputName=askmessagedialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing askmessagedialogimpl.h...
InputDir=.
InputPath=.\askmessagedialogimpl.h
InputName=askmessagedialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\authenticationdialogimpl.h

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing authenticationdialogimpl.h...
InputDir=.
InputPath=.\authenticationdialogimpl.h
InputName=authenticationdialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing authenticationdialogimpl.h...
InputDir=.
InputPath=.\authenticationdialogimpl.h
InputName=authenticationdialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\belpicgui.h
# End Source File
# Begin Source File

SOURCE=.\changepindialogimpl.h

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing changepindialogimpl.h...
InputDir=.
InputPath=.\changepindialogimpl.h
InputName=changepindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing changepindialogimpl.h...
InputDir=.
InputPath=.\changepindialogimpl.h
InputName=changepindialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\insertcarddialogimpl.h

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing insertcarddialogimpl.h...
InputDir=.
InputPath=.\insertcarddialogimpl.h
InputName=insertcarddialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing insertcarddialogimpl.h...
InputDir=.
InputPath=.\insertcarddialogimpl.h
InputName=insertcarddialogimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MessageDialog.h
# End Source File
# Begin Source File

SOURCE=.\PINDialog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\images\cardreader-id_64.bmp"
# End Source File
# Begin Source File

SOURCE=.\images\idwindow.ico
# End Source File
# Begin Source File

SOURCE=.\images\pincode_128.bmp
# End Source File
# Begin Source File

SOURCE=.\images\signature_128.bmp
# End Source File
# End Group
# Begin Group "Forms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\askmessagedialog.ui

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# Begin Custom Build - UICing askmessagedialog.ui...
InputDir=.
InputPath=.\askmessagedialog.ui
InputName=askmessagedialog

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

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# Begin Custom Build - UICing askmessagedialog.ui...
InputDir=.
InputPath=.\askmessagedialog.ui
InputName=askmessagedialog

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
# Begin Source File

SOURCE=.\authenticationdialog.ui

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# Begin Custom Build - UICing authenticationdialog.ui...
InputDir=.
InputPath=.\authenticationdialog.ui
InputName=authenticationdialog

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

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# Begin Custom Build - UICing authenticationdialog.ui...
InputDir=.
InputPath=.\authenticationdialog.ui
InputName=authenticationdialog

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
# Begin Source File

SOURCE=.\changepindialog.ui

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# Begin Custom Build - UICing changepindialog.ui...
InputDir=.
InputPath=.\changepindialog.ui
InputName=changepindialog

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

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# Begin Custom Build - UICing changepindialog.ui...
InputDir=.
InputPath=.\changepindialog.ui
InputName=changepindialog

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
# Begin Source File

SOURCE=.\insertcarddialog.ui

!IF  "$(CFG)" == "belpicgui - Win32 Release"

# Begin Custom Build - UICing insertcarddialog.ui...
InputDir=.
InputPath=.\insertcarddialog.ui
InputName=insertcarddialog

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

!ELSEIF  "$(CFG)" == "belpicgui - Win32 Debug"

# Begin Custom Build - UICing insertcarddialog.ui...
InputDir=.
InputPath=.\insertcarddialog.ui
InputName=insertcarddialog

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

SOURCE=.\askmessagedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\askmessagedialog.h
# End Source File
# Begin Source File

SOURCE=.\authenticationdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\authenticationdialog.h
# End Source File
# Begin Source File

SOURCE=.\changepindialog.cpp
# End Source File
# Begin Source File

SOURCE=.\changepindialog.h
# End Source File
# Begin Source File

SOURCE=.\insertcarddialog.cpp
# End Source File
# Begin Source File

SOURCE=.\insertcarddialog.h
# End Source File
# Begin Source File

SOURCE=.\moc_askmessagedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_askmessagedialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_authenticationdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_authenticationdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_changepindialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_changepindialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_insertcarddialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_insertcarddialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\qmake_image_collection.cpp
# End Source File
# End Group
# End Target
# End Project
