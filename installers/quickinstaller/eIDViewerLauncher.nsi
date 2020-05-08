;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include WinMessages.nsh
!include "eidmw_version.nsh"
!include WinVer.nsh
!include "buttons.nsh"


;--------------------------------
;General
;inner dialog size in units in classic nsdialogs: 266 * 130
;in modern UI: 300 * 140


;defines
!addplugindir "NSIS_Plugins\beidread\Release"
;Name and file
Name "Belgium eID Viewer ${EIDMW_VERSION}"
OutFile "Belgium eID Viewer Launcher ${EIDMW_VERSION}.exe"
VIProductVersion "${EIDMW_VERSION}"
VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) ${EIDMW_YEAR}"
VIAddVersionKey "FileDescription" "Belgium eID Viewer"

;Default installation folder
InstallDir "$DESKTOP"
	
;Request application privileges for Windows Vista
RequestExecutionLevel user  
XPStyle on
Icon beID.ico

caption "Belgium eID Viewer"
	
Var Button
Var FileToCopy
Var Response
Var InstallFailed

;--------------------------------
	;Interface Settings
BrandingText " "
InstProgressFlags smooth
;do not show installation details
ShowInstDetails nevershow
ShowUninstDetails nevershow

Function InstShow
SetCtlColors $HWNDPARENT 0 0xFFFFFF ; parent background white, black text

GetDlgItem $0 $HWNDPARENT 1028 ;1028 is the bar between the default buttons
SetCtlColors $0 0 0xFFFFFF ; background white, black text

FindWindow $0 "#32770" "" $HWNDPARENT ; get the inner dialog
SetCtlColors $0 0 0xFFFFFF ; make the background white, black text
GetDlgItem $1 $0 1006 ; get the item handle of detail text
SetCtlColors $1 0 0xFFFFFF ; make that item's background white, black text
FunctionEnd

;--------------------------------
;Pages

Page instfiles "" show_instfiles ""


;--------------------------------
;Languages
; its all in "eIDTranslations.nsh"
;--------------------------------
!include "eIDTranslations.nsh"
  
MiscButtonText $(ls_back) $(ls_next) $(ls_cancel) $(ls_close)
  
;--------------------------------
;Installer Sections

Section "Belgium Eid Viewer" BeidViewer
	HideWindow
	SetOutPath "$INSTDIR"
		
	${If} ${RunningX64}
		ClearErrors
		StrCpy $FileToCopy "$INSTDIR\eIDViewer.exe"
		File "..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\x64\Release\eIDViewer.exe"
		StrCpy $FileToCopy "$INSTDIR\eIDViewerBackend.dll"
		File "..\..\plugins_tools\eid-viewer\Windows\eIDViewerBackend\VS_2017\Binaries\x64_Release\eIDViewerBackend.dll"
		StrCpy $FileToCopy "$INSTDIR\beid_ff_pkcs11.dll"
		File "..\..\cardcomm\pkcs11\VS_2017\Binaries\x64_PKCS11_FF_Release\beid_ff_pkcs11.dll"
	${Else}				
		ClearErrors
		StrCpy $FileToCopy "$INSTDIR\eIDViewer.exe"
		File "..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewer.exe"
		StrCpy $FileToCopy "$INSTDIR\eIDViewerBackend.dll"
		File "..\..\plugins_tools\eid-viewer\Windows\eIDViewerBackend\VS_2017\Binaries\Win32_Release\eIDViewerBackend.dll"
		StrCpy $FileToCopy "$INSTDIR\beid_ff_pkcs11.dll"
		File "..\..\cardcomm\pkcs11\VS_2017\Binaries\Win32_PKCS11_FF_Release\beid_ff_pkcs11.dll"
	${EndIf}
				
	IfErrors 0 +2
		Call ErrorHandler_file
	ClearErrors
	
	ExecWait '"$INSTDIR\eIDViewer.exe"' $Response
	${Switch} $Response
		${Case} 0
		${Case} 3010 
			;3010 is 'success, but reboot requiered'
			;set 1 for testing, 0 otherwise
			StrCpy $InstallFailed 0
		${Break}
		${Default}
			StrCpy $InstallFailed $Response
	${EndSwitch}
	
	Delete "$INSTDIR\eIDViewer.exe"
	Delete "$INSTDIR\eIDViewerBackend.dll"
	Delete "$INSTDIR\beid_ff_pkcs11.dll"

SectionEnd

Function ErrorHandler_file
  MessageBox MB_ICONSTOP "$(ls_errorcopyfile) $FileToCopy"
    Abort
FunctionEnd

;--------------------------------
;Installer Functions

Function .onInit

InitPluginsDir

FunctionEnd

Function .onInstSuccess
FunctionEnd

Function show_instfiles
	SetAutoClose true
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_next)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_cancel)"

	Call InstShow
	${buttonVisible} "Back" 0
FunctionEnd








