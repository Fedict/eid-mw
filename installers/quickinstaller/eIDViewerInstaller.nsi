;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include WinMessages.nsh
!include "eidmw_version.nsh"
!include WinVer.nsh
!include "buttons.nsh"
!include "fileSearch.nsh"

;--------------------------------
;General
;inner dialog size in units in classic nsdialogs: 266 * 130
;in modern UI: 300 * 140


;defines
!addplugindir "NSIS_Plugins\beidread\Release"
;Name and file
Name "Belgium eID Viewer ${EIDMW_VERSION}"
OutFile "Belgium eID Viewer Installer ${EIDMW_VERSION}.exe"
VIProductVersion "${EIDMW_VERSION}"
VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) ${EIDMW_YEAR}"
VIAddVersionKey "FileDescription" "Belgium eID Viewer"

;Default installation folder
InstallDir "$PROGRAMFILES\Belgium Identity Card"
	
;Request application privileges for Windows Vista
RequestExecutionLevel admin  
XPStyle on
Icon beID.ico

caption $(ls_caption_viewer)
	
Var nsdCustomDialog
Var Label
Var Background_Image
Var Background_Image_Handle
Var Background_Image2
Var Background_Image_Handle2
Var Button
Var Font_Title
Var Font_Info
Var FileToCopy
Var LogFile
Var MsiResponse
Var InstallFailed
Var retval
Var TempFile
Var firstLine

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

Page custom nsdWelcome nsdWelcomeLeave
;Page license "" show_instfiles ""
Page instfiles "" show_instfiles ""
Page custom nsdInstallCheck nsdInstallCheckLeave
Page custom nsdDone nsdDoneLeave

;--------------------------------
;Languages
; its all in "eIDTranslations.nsh"
;--------------------------------
!include "eIDTranslations.nsh"
  
MiscButtonText $(ls_back) $(ls_next) $(ls_cancel) $(ls_close)
; MessageBox MB_OK "A translated message: $(message)"
;LicenseLangString license ${LANG_ENGLISH} "..\..\doc\licenses\English\License_en.rtf"
;LicenseLangString license ${LANG_GERMAN} "..\..\doc\licenses\German\License_de.rtf"
;LicenseLangString license ${LANG_FRENCH} "..\..\doc\licenses\French\License_fr.rtf"
;LicenseLangString license ${LANG_DUTCH} "..\..\doc\licenses\Dutch\License_nl.rtf"

;LicenseData $(license)
  
;--------------------------------
;Installer Sections

Section "Belgium Eid Viewer" BeidViewer
	SetOutPath "$INSTDIR"
	CreateDirectory "$INSTDIR\log"
		
	ClearErrors
	StrCpy $FileToCopy "$INSTDIR\BeidViewer.msi"
	File "..\eid-viewer\Windows\bin\BeidViewer.msi"
	IfErrors 0 +2
		Call ErrorHandler_file
	ClearErrors

	StrCpy $LogFile "$INSTDIR\log\install_eidviewer_log.txt"
	StrCpy $TempFile "$INSTDIR\log\viewer_error_count.txt"

	ExecWait 'msiexec /quiet /norestart /log "$LogFile" /i "$INSTDIR\BeidViewer.msi"' $MsiResponse
	${Switch} $MsiResponse
		${Case} 0
		${Case} 3010 
			;3010 is 'success, but reboot requiered'
			;set 1 for testing, 0 otherwise
			StrCpy $InstallFailed 0
		${Break}
		${Case} 1603
			;general failure, parse through the log file to find the root cause
			;1638: check if downgrade install was attempted and blocked
				ExecWait 'cmd.exe /C FIND "error 1638" "$LogFile" | FIND /C "error 1638" > "$TempFile"' $retval
				!insertmacro GetFirstLineOfFile $TempFile $firstLine
				DetailPrint "a newer version of the eID Viewer has been detected"
				StrCmp "$firstLine" "" +2 0	
				StrCmp "$firstLine" "0" 0 MSI_1638_Error
		${Break}
		${Case} 1638
			MSI_1638_Error:
				DetailPrint "$(ls_errorinstallmsi_viewer_1638) $\r$\n $(ls_error) = $MsiResponse"
				;Another version of this product is already installed. Installation of this version cannot continue.
				;To configure or remove the existing version of this product, use Add/Remove Programs in Control Panel.
				MessageBox MB_OK "$(ls_errorinstallmsi_viewer_1638)$\r$\n$\r$\n $(ls_error) = $MsiResponse"	
				StrCpy $InstallFailed 1638
		${Break}
		${Default}
			StrCpy $InstallFailed $MsiResponse
	${EndSwitch}

	Delete "$INSTDIR\BeidViewer.msi"
  
SectionEnd

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
File /oname=$PLUGINSDIR\"failed.bmp" "Failed.bmp"
File /oname=$PLUGINSDIR\"done.bmp" "done.bmp"
File /oname=$PLUGINSDIR\"White.bmp" "white.bmp"
;File /oname=$PLUGINSDIR\"bannrbmp.bmp" "bannrbmp.bmp"

;for testing different languages
;	Push ${LANG_GERMAN}
;	Pop $LANGUAGE
	;Language selection dialog

;	Push ${LANG_ENGLISH}
;	Pop $LANGUAGE
;	Push ${LANG_DUTCH}
;	Pop $LANGUAGE
;	Push ${LANG_FRENCH}
;	Pop $LANGUAGE
;	Push ${LANG_GERMAN}
;	Pop $LANGUAGE

;	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
;	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

;	Pop $LANGUAGE
;	StrCmp $LANGUAGE "cancel" 0 +2
;		Abort


FunctionEnd

Function .onInstSuccess
FunctionEnd

Function nsdWelcome
	;skip the window when silent
	IfSilent 0 +2 
        Abort   

	File "welcome.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog ;popped from the stack to prevent stack corruption
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	Call InstShow

	
	${NSD_CreateLabel} 0 25% 100% 16u "$(ls_welcome_short)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 50% 100% 12u "$(ls_welcome_viewer_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\welcome.bmp" $Background_Image_Handle 
	SetCtlColors $Background_Image 0xFFFFFF transparent
	
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_install)"
	; hide the Back button
	${buttonVisible} "Back" 0
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function  nsdWelcomeLeave
FunctionEnd

Function show_instfiles
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_next)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_cancel)"

	Call InstShow
	${buttonVisible} "Back" 0
FunctionEnd


Function nsdInstallCheck

    ${If} $InstallFailed == 0
        ;MessageBox MB_OK "Install ok, skipping error"
        Abort   
    ${EndIf}

	;skip the window when silent
	IfSilent 0 +2 
        Abort   
	
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
	Call InstShow
	
	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_install_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 16u "$(ls_error) $InstallFailed"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x008080 transparent
	
	${NSD_CreateLabel} 0 70% 100% 36u "$(ls_install_viewer_failed_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent

	${NSD_CreateBitmap} 0% 0 100% 100% ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 	
	
	;first page after install page doesn't get its inner dialog color correct (not even when refreshed),
	;so we add a white background
	;${NSD_CreateBitmap} 0 0 266u 124u ""
	;Pop $Background_Image2
    ;${NSD_SetStretchedImage} $Background_Image2 "$PLUGINSDIR\White.bmp" $Background_Image_Handle2 
	
	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_helpsite)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"

	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
		
FunctionEnd

Function nsdInstallCheckLeave
	;skip the function when silent
	IfSilent 0 +2 
        Abort   
		
	call FindSolutionButton_click
FunctionEnd

Function nsdDone
	;skip the window when silent
	IfSilent 0 +2 
        Abort   

	;File "welcome.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
	Call InstShow
		
	${NSD_CreateLabel} 0 50% 100% 18u "$(ls_viewer_complete)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
;	${NSD_CreateLabel} 0 60% 100% 36u "$(ls_testcomplete_info)"
;	Pop $Label
;	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
;	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
;	SendMessage $Label ${WM_SETFont} $Font_Info 1
;	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Done.bmp" $Background_Image_Handle 

	;first page after install page doesn't get its inner dialog color correct (not even when refreshed),
	;so we add a white background
	${NSD_CreateBitmap} 0 124u 266u 130u ""
	Pop $Background_Image2
    ${NSD_SetStretchedImage} $Background_Image2 "$PLUGINSDIR\White.bmp" $Background_Image_Handle2 

	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	;GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	;SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"

	
	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 0	
				
	nsDialogs::Show

	${NSD_FreeImage} $Background_Image_Handle
	${NSD_FreeImage} $Background_Image_Handle2
	
FunctionEnd

Function  nsdDoneLeave
FunctionEnd

Function FindSolutionButton_click
    ExecShell "open" "https://eid.belgium.be/"
	;when keeping the nsis installer alive, it can permit the webbrowser to take the foreground.
	;should we quit in stead, the webbrowser will be openened in the background
	Abort
	;Quit
FunctionEnd







