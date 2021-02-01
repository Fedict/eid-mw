!include "..\\..\\installers\\quickinstaller\\eidmw_version.nsh"
!include "..\\..\\installers\\quickinstaller\\eIDTranslations.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh

;Name and file
Name "Firefox Plugin Installer ${EIDMW_VERSION}"
OutFile "Belgium eID Firefox Plugin Installer ${EIDMW_VERSION}.exe"
VIProductVersion "${EIDMW_VERSION}"
VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
  ;NSIS complains that Fileversion was not set for English, though it should have been done by the line above
;VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2015"
VIAddVersionKey "FileDescription" "Belgium eID Firefox Addon Installer"


XPStyle on
WindowIcon on
Icon "..\\..\\installers\\quickinstaller\\Setup.ico"
caption "Belgium eID Firefox Addon Installer ${EIDMW_VERSION_SHORT}"
	
BrandingText " "
;"Fedict"
InstallColors /windows

Page instfiles "" show_instfiles ""

ShowInstDetails nevershow

Var Button
Var FFResponse

Function .onInit
FunctionEnd

Function show_instfiles
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_next)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_cancel)"
FunctionEnd

Section "Launch Firefox" FFStart
	IfFileExists "$PROGRAMFILES\Mozilla Firefox\Firefox.exe" 0 +3
		Exec '"$PROGRAMFILES\Mozilla Firefox\Firefox.exe" "$(ls_firefoxeidpluginpage)"'
		Quit
	MessageBox MB_OK "Firefox not found, but you can find the plugin at $(ls_firefoxeidpluginpage)"
SectionEnd
