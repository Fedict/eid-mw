;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include "eIDTranslations.nsh"
!include WinMessages.nsh
!include "eidmw_version.nsh"
!include WinVer.nsh

;!include nsDialogs_createTextMultiline.nsh
;!include MUI2.nsh

;--------------------------------
;General

  ;defines
!define LOGFILE ""
!define INSTALLER_NAME "certRegInstaller"

  ;Name and file
  Name "${INSTALLER_NAME} ${EIDMW_VERSION}"
  OutFile "${INSTALLER_NAME} ${EIDMW_VERSION}.exe"
  VIProductVersion "${EIDMW_VERSION}"
  VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
  ;NSIS complains that Fileversion was not set for English, though it should have been done by the line above
;VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) ${EIDMW_YEAR}"
VIAddVersionKey "FileDescription" "Belgium eID CertRegTool"

 
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Belgium Identity Card\CertReg"
	;InstallDir "$LOCALAPPDATA\Belgium Identity Card"
	
  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin
	;admin;user
	;TargetMinimalOS 5.0
	;installer will run on Win2000 and newer
  
  XPStyle on
	WindowIcon on
	Icon Setup.ico
	caption "${INSTALLER_NAME} ${EIDMW_VERSION_SHORT}"
	;SubCaption 
	
	Var Button
	Var FileToCopy

;--------------------------------
	;Interface Settings
	;SilentInstall silent
BrandingText " "
;"Fedict"
InstallColors /windows
;InstProgressFlags smooth

;--------------------------------
;Pages

Page license
Page instfiles "" show_instfiles ""

;--------------------------------
;Languages
; its all in "eIDTranslations.nsh"
;--------------------------------
;Reserve Files

  
;If you are using solid compression, files that are required before
;the actual installation should be stored first in the data block,
;because this will make your installer start faster.
  
;!insertmacro MUI_RESERVEFILE_LANGDLL

MiscButtonText $(ls_back) $(ls_next) $(ls_cancel) $(ls_close)
; MessageBox MB_OK "A translated message: $(message)"
LicenseLangString license ${LANG_ENGLISH} "..\..\doc\licenses\English\License_en.rtf"
LicenseLangString license ${LANG_GERMAN} "..\..\doc\licenses\German\License_de.rtf"
LicenseLangString license ${LANG_FRENCH} "..\..\doc\licenses\French\License_fr.rtf"
LicenseLangString license ${LANG_DUTCH} "..\..\doc\licenses\Dutch\License_nl.rtf"

LicenseData $(license)
;LicenseText "text" "button_text"

;--------------------------------
;Installer Sections

Section "Belgium Eid Certifiacte Registration Tool" BeidCertReg
	SetOutPath "$INSTDIR"
	
	ClearErrors
	StrCpy $FileToCopy "$INSTDIR\CertReg\certreg.exe"
	File "..\..\plugins_tools\certreg\Release\certreg.exe"
	IfErrors 0 +2
		Call ErrorHandler_file
SectionEnd

;--------------------------------
;Error Messages
Function ErrorHandler_file
  MessageBox MB_ICONSTOP "$(ls_errorcopyfile) $FileToCopy"
    Abort
FunctionEnd


;--------------------------------
;Installer Functions

Function .onInit
;images used by this installer
InitPluginsDir
;extract the bitmaps to the temporary pluginsdir (which will be auto removed after installation)
File /oname=$PLUGINSDIR\"welcome.bmp" "welcome.bmp"


;for testing different languages
	;Push ${LANG_GERMAN}
	;Pop $LANGUAGE
	;Language selection dialog

;	Push ${LANG_ENGLISH}
;	Push English
;	Push ${LANG_DUTCH}
;	Push Dutch
;	Push ${LANG_FRENCH}
;	Push French
;	Push ${LANG_GERMAN}
;	Push German

;	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
;	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

;	Pop $LANGUAGE
;	StrCmp $LANGUAGE "cancel" 0 +2
;		Abort


FunctionEnd

Function .onInstSuccess
FunctionEnd

Function show_instfiles
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_next)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_cancel)"
FunctionEnd
