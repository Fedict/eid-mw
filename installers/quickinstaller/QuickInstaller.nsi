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
Name "Belgium eID-QuickInstaller ${EIDMW_VERSION}"
OutFile "Belgium eID-QuickInstaller ${EIDMW_VERSION}.exe"
VIProductVersion "${EIDMW_VERSION}"
VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) ${EIDMW_YEAR}"
VIAddVersionKey "FileDescription" "Belgium eID MiddleWare"

;Default installation folder
InstallDir "$PROGRAMFILES\Belgium Identity Card"
	
;Request application privileges for Windows Vista
RequestExecutionLevel admin  
XPStyle on
Icon beID.ico

caption $(ls_caption)

	Var versionMajor
	Var versionMinor
	Var retval
	Var readercount
	Var lastname
	Var firstletterthirdname
	Var firstname
	
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
	;Var TestLogFile
	Var TempFile
	Var firstLine
	Var MsiResponse
	Var InstallFailed
	Var ReaderFailed
	Var FindCardFailed
	Var FAQ_url

;--------------------------------
	;Interface Settings
BrandingText " "
InstProgressFlags smooth
;do not show installation details
;ShowInstDetails nevershow
;ShowUninstDetails nevershow

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
Page custom nsdConnectReader nsdConnectReaderLeave
Page custom nsdReaderCheck nsdReaderCheckLeave
Page custom nsdInsertCard nsdInsertCardLeave
Page custom nsdCardCheck nsdCardCheckLeave
Page custom nsdCardData nsdCardDataLeave

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

Section "Belgium Eid Crypto Modules" BeidCrypto
	SetOutPath "$INSTDIR"
	CreateDirectory "$INSTDIR\log"
	
	${WinVerGetMajor} $versionMajor
	${WinVerGetMinor} $versionMinor
	
	StrCpy $FAQ_url "https://eid.belgium.be/"
	
	${If} ${RunningX64}
		ClearErrors
		StrCpy $FileToCopy "$INSTDIR\BeidMW_64.msi"
		File "..\eid-mw\Windows\bin\BeidMW_64.msi"
		IfErrors 0 +2
			Call ErrorHandler_file
		ClearErrors
		
		StrCpy $LogFile "$INSTDIR\log\install_eidmw64_log.txt"
		StrCpy $TempFile "$INSTDIR\log\1612_count.txt"
		;delete previous log
		;Delete "$LogFile"
		ExecWait 'msiexec /quiet /norestart /log "$LogFile" /i "$INSTDIR\BeidMW_64.msi"' $MsiResponse
		;for testing
		;StrCpy $MsiResponse 1603
		;StrCpy $TestLogFile "$INSTDIR\log\install_eidmw64_error_1612_log.txt"
		${Switch} $MsiResponse
			${Case} 1603
			;general failure, parse through the log file to find the root cause
			;check if error 1612 occured
				;for testing
				;ExecWait 'cmd.exe /C FIND "1612" "$TestLogFile" | FIND /C "error code 1612" > "$TempFile"' $retval
				ExecWait 'cmd.exe /C FIND "1612" "$LogFile" | FIND /C "error code 1612" > "$TempFile"' $retval
				!insertmacro GetFirstLineOfFile $TempFile $firstLine
				DetailPrint "MSI error 1612, count = $firstLine"
				StrCmp "$firstLine" "" +2 0	
				StrCmp "$firstLine" "0" 0 MSI_1612_Error_64			
			${Break}
			${Case} 1612
			MSI_1612_Error_64:
				DetailPrint "$(ls_errorinstallmsi_1612) $\r$\n $(ls_error) = $MsiResponse"
				;Refer to the FAQ where the user can find a manuel to manually repair the registry, or to run a MS tool that does the cleanup
				StrCpy $FAQ_url "$(ls_errorinstallmsi_1612_FAQurl)"
			;The installation source for this product is not available. Verify that the source exists and that you can access it.
			;often caused by registry not cleaned when cleanup tools remove previously installed msi files
			${Break}
			${Case} 1622
			;install log failure, try to install without logging
				ExecWait 'msiexec /quiet /norestart /i "$INSTDIR\BeidMW_64.msi"' $MsiResponse			
			${Break}
			${Default}	
				DetailPrint "MsiResponse = $MsiResponse"
			${Break}				
		${EndSwitch}
		;IfErrors 0 +2
		;	Call ErrorHandler_msiexec
		
		;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto64" 0x1
		Delete "$INSTDIR\BeidMW_64.msi"
	${Else}	
		ClearErrors
		StrCpy $FileToCopy "$INSTDIR\BeidMW_32.msi"
		File "..\eid-mw\Windows\bin\BeidMW_32.msi"
		IfErrors 0 +2
			Call ErrorHandler_file
		ClearErrors
		;delete previous log
		StrCpy $LogFile "$INSTDIR\log\install_eidmw32_log.txt"
		;Delete "$LogFile"
		ExecWait 'msiexec /quiet /norestart /log "$LogFile" /i "$INSTDIR\BeidMW_32.msi"' $MsiResponse
		
		;try to recover from some errors
		${Switch} $MsiResponse
			${Case} 1603
			;general failure, parse through the log file to find the root cause
			;check if error 1612 occured
				ExecWait 'cmd.exe /C FIND "1612" "$LogFile" | FIND /C "error code 1612" > "$TempFile"' $retval
				!insertmacro GetFirstLineOfFile $TempFile $firstLine
				DetailPrint "MSI error 1612, count = $firstLine"
				StrCmp "$firstLine" "" +2 0	
				StrCmp "$firstLine" "0" 0 MSI_1612_Error_32			
			${Break}
			${Case} 1612
			MSI_1612_Error_32:
				DetailPrint "$(ls_errorinstallmsi_1612) $\r$\n $(ls_error) = $MsiResponse"
				;Refer to the FAQ where the user can find a manuel to manually repair the registry, or to run a MS tool that does the cleanup
				StrCpy $FAQ_url "$(ls_errorinstallmsi_1612_FAQurl)"
			;The installation source for this product is not available. Verify that the source exists and that you can access it.
			;often caused by registry not cleaned when cleanup tools remove previously installed msi files
			${Break}				
			${Case} 1622
			;install log failure, try to install without logging
				ExecWait 'msiexec /quiet /norestart /i "$INSTDIR\BeidMW_32.msi"' $MsiResponse			
			${Break}
			${Default}	
				DetailPrint "MsiResponse = $MsiResponse"
			${Break}	
		${EndSwitch}
		;IfErrors 0 +2
		;	Call ErrorHandler_msiexec
		;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x1
		Delete "$INSTDIR\BeidMW_32.msi"
	${EndIf}
	
	;check if msi install went ok (initially or after correction)
 	${Switch} $MsiResponse
		${Case} 0
		${Case} 3010 
			StrCpy $InstallFailed 0		
			;3010 is 'success, but reboot requiered'
		${Break}		
		${Case} 1618
			;the installer is already running in another instance	
			MessageBox MB_OK "$(ls_errorinstallmsi_1618) $\r$\n $(ls_error) = $MsiResponse"	
			StrCpy $InstallFailed $MsiResponse
		${Break}			
		${Default}
			StrCpy $InstallFailed $MsiResponse
			;Call ErrorHandler_msiexec
	${EndSwitch}	
  
  File /r "Drivers"

${DisableX64FSRedirection}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 0
		;vista
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN8\ACR38U\a38usb.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\OmniKey3121\cxbu0wdm.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\SCR331\scr3xx.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\VascoDP509\usbccid.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\SPR532\Spr332.inf"'
	${EndIf}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 1
		;windows 7
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN8\ACR38U\a38usb.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\OmniKey3121\cxbu0wdm.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\SCR331\scr3xx.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\VascoDP509\usbccid.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN7\SPR532\Spr332.inf"'
		nsExec::ExecToLog '"$SYSDIR\Robocopy.exe" "$INSTDIR\Drivers\XP-WIN8\beidmdrv" "$INSTDIR\Drivers\WINALL\beidmdrv" beidmdrv.cat'
	${EndIf}
		
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 2
		;windows 8
		nsExec::ExecToLog '"$SYSDIR\Robocopy.exe" "$INSTDIR\Drivers\XP-WIN8\beidmdrv" "$INSTDIR\Drivers\WINALL\beidmdrv" beidmdrv.cat'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN8\ACR38U\a38usb.inf"'
	${EndIf}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 3
		;windows 8.1
		nsExec::ExecToLog '"$SYSDIR\Robocopy.exe" "$INSTDIR\Drivers\XP-WIN8\beidmdrv" "$INSTDIR\Drivers\WINALL\beidmdrv" beidmdrv.cat'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\XP-WIN8\ACR38U\a38usb.inf"'
	${EndIf}
	;driver to install on all OS's

	nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\WINALL\APG8201Z\apg8201z.inf"'
	
	;install the eid minidriver
	nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\Drivers\WINALL\beidmdrv\beidmdrv.inf"'

${EnableX64FSRedirection}

  RMDir /r /REBOOTOK $INSTDIR\Drivers

SectionEnd


Function ErrorHandler_file
IfSilent +2 0 
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
File /oname=$PLUGINSDIR\"done.bmp" "done.bmp"
File /oname=$PLUGINSDIR\"insert_card.bmp" "insert_card.bmp"
File /oname=$PLUGINSDIR\"connect_reader.bmp" "connect_reader.bmp"
File /oname=$PLUGINSDIR\"Failed.bmp" "failed.bmp"
File /oname=$PLUGINSDIR\"White.bmp" "white.bmp"
;File /oname=$PLUGINSDIR\"bannrbmp.bmp" "bannrbmp.bmp"

;for testing different languages
	;Push ${LANG_GERMAN}
	;Pop $LANGUAGE
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
	;File "welcome.bmp"
	;skip the windows when silent
	IfSilent 0 +2 
        Abort   
	
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
	
	${NSD_CreateLabel} 0 45% 100% 12u "$(ls_welcome_info)"
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

Function RelGotoPage
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"

  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function GotoNextPage
    StrCpy $R9 "1"
    Call RelGotoPage
FunctionEnd

Function GotoPrevPage
    StrCpy $R9 "-1"
    Call RelGotoPage
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
	
	
	;abort the installer when silent and error occured
	IfSilent 0 +3
		SetErrorLevel 3
        Quit   

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
	
	${NSD_CreateLabel} 0 70% 100% 36u "$(ls_install_failed_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 

	;first page after install page doesn't get its inner dialog color correct (not even when refreshed),
	;so we add a white background
;${NSD_CreateBitmap} 0 0 266u 124u ""
;	Pop $Background_Image2
 ;   ${NSD_SetStretchedImage} $Background_Image2 "$PLUGINSDIR\White.bmp" $Background_Image_Handle2 
	
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
	call FindSolutionButton_click
FunctionEnd

Function nsdDone
	
	;skip the window when silent
	IfSilent 0 +2 
        Abort   
	
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
	Call InstShow

;FindWindow $0 "#32770" "" $HWNDPARENT ; get the inner dialog
;SetCtlColors $0 0 0
;;  ShowWindow $0 ${SW_HIDE}
;  ShowWindow $0 ${SW_SHOW}
	

	
	${NSD_CreateLabel} 0 40% 100% 18u "$(ls_complete)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 36u "$(ls_complete_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
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
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_test)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	;EnableWindow $NextButton 1 ;enable the previous button
	;SetCtlColors $NextButton 0xFF0000 0x00FF00
	
	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1	
		
	;GetDlgItem $0 $HWNDPARENT 1 ;move the next button
    ;System::Call 'user32::MoveWindow(i $0, i 100, i 50, i 200, i 30, i 1)'
			
	nsDialogs::Show

	${NSD_FreeImage} $Background_Image_Handle
	${NSD_FreeImage} $Background_Image_Handle2
	
FunctionEnd

Function  nsdDoneLeave
FunctionEnd

Function nsdConnectReader
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	;File "connect_reader.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog

	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
	Call InstShow
	
	${NSD_CreateLabel} 0 -40u 100% 40u "$(ls_pleaseconnect)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "14" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	

	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\connect_reader.bmp" $Background_Image_Handle 
	
	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function  nsdConnectReaderLeave
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	beid::GetReaderCount 0
	Pop $retval
	
	${If} $retval <> '0'
		StrCpy $ReaderFailed $retval
		;MessageBox MB_OK "$(ls_errorreadersearch) $\r$\n $(ls_error) = $retval"
		;Abort
	${EndIf}
  Pop $readercount
	${If} $readercount > 0
		StrCpy $ReaderFailed 0
		;MessageBox MB_OK "$$readercount is $readercount"
	${Else}
		;MessageBox MB_OK "$(ls_noreaderfound)"
		StrCpy $ReaderFailed "$(ls_noreaderfound)"
		;Abort
	${EndIf}
FunctionEnd

Function nsdReaderCheck
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
    ${If} $ReaderFailed == 0
        ;MessageBox MB_OK "Reader found, skipping reader error"
        Abort   
    ${EndIf}
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		MessageBox MB_OK "nsdCustomDialog error"
		Abort
	${EndIf}

	Call InstShow
	
	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_cardreader_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 16u "$(ls_error) $ReaderFailed"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x008080 transparent
	
	${NSD_CreateLabel} 0 70% 100% 36u "$(ls_cardreader_failed_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 

	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_helpsite)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	GetDlgItem $Button $HWNDPARENT 3 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_retry)"

	${buttonVisible} "Back" 1
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function nsdReaderCheckLeave
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	call FindSolutionButton_click
FunctionEnd


Function nsdInsertCard 
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	;File "insert_card.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
	Call InstShow
	${NSD_CreateLabel} 0 -40u 100% 40u "$(ls_pleaseinsertcard)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "14" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	

	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapinsertcard)"
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\insert_card.bmp" $Background_Image_Handle 
	
	${buttonVisible} "Back" 1
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function nsdInsertCardLeave
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	;GetReaderCount 1 in order to get the readers with an eID card inserted
	beid::GetReaderCount 1	
	Pop $retval
	
	;the instruction below is just for testing
	;StrCpy $retval 004180
	
	${If} $retval <> '0'
		StrCpy $FindCardFailed $retval
		;MessageBox MB_OK "$(ls_errorreadingcard)"
		;Abort
	${EndIf}
	Pop $readercount
	${If} $readercount > '0'
		StrCpy $FindCardFailed '0'
		;MessageBox MB_OK "number of beidcards found is $readercount"
	${Else}
		StrCpy $FindCardFailed "$(ls_nocardfound)"
		;MessageBox MB_OK "$(ls_nocardfound)"
		;Abort
	${EndIf}
FunctionEnd

Function nsdCardCheck
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
    ${If} $FindCardFailed == '0'
        ;MessageBox MB_OK "Card found, skipping card error"
        Abort   
    ${EndIf}
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	Call InstShow
	
	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_test_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 16u "$(ls_error) $FindCardFailed"	
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x008080 transparent
	
	${NSD_CreateLabel} 0 70% 100% 36u "$(ls_test_failed_info)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 266u 124u ""
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 

	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_helpsite)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"

	${buttonVisible} "Back" 1
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function nsdCardCheckLeave
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	call FindSolutionButton_click
FunctionEnd


Function nsdCardData
	;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	nsDialogs::Create 1018
	Pop $nsdCustomDialog	
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
		
	Call InstShow
		
	beid::ReadCardData
	Pop $retval
	
	;for testing
	;StrCpy $retval 004180
	
	${If} $retval == '0'
	
		Pop $lastname
		Pop $firstletterthirdname
		Pop $firstname
	
		;all went well, show a succes message on this final page
		${NSD_CreateLabel} 0 40% 100% 36u "$(ls_testcomplete_pre) $firstname $(ls_testcomplete_post)"
		Pop $Label
		SetCtlColors $Label 0x008080 transparent
		${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
		CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
		SendMessage $Label ${WM_SETFont} $Font_Title 1
		
		${NSD_CreateLabel} 0 80% 100% 36u "$(ls_testcomplete_info)"
		Pop $Label
		${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
		CreateFont $Font_Info "Arial" "9" "700" ;/UNDERLINE
		SendMessage $Label ${WM_SETFont} $Font_Info 1
		SetCtlColors $Label 0x000000 transparent
		
		${NSD_CreateBitmap} 0 0 266u 124u ""
		Pop $Background_Image
		${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Done.bmp" $Background_Image_Handle 
		
		GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
		SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_close)"
		GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
		SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
			
		${buttonVisible} "Back" 1
		${buttonVisible} "Next" 1
		${buttonVisible} "Cancel" 0	
	${Else} 

		${NSD_CreateLabel} 0 40% 100% 20u "$(ls_errorreadingcard)"
		Pop $Label
		SetCtlColors $Label 0x008080 transparent
		${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
		CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
		SendMessage $Label ${WM_SETFont} $Font_Title 1
		
		${NSD_CreateLabel} 0 60% 100% 16u "$(ls_error) R$retval"
		Pop $Label
		${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
		CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
		SendMessage $Label ${WM_SETFont} $Font_Info 1
		SetCtlColors $Label 0x008080 transparent
		
		${NSD_CreateLabel} 0 70% 100% 36u "$(ls_test_failed_info)"
		Pop $Label
		${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
		CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
		SendMessage $Label ${WM_SETFont} $Font_Info 1
		SetCtlColors $Label 0x000000 transparent
		
		${NSD_CreateBitmap} 0 0 266u 124u ""
		Pop $Background_Image
		${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 
	
		;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
		GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
		SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_helpsite)"
		GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
		SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	
		;enable the cancel(finish) button
		GetDlgItem $1 $HWNDPARENT 2 
		EnableWindow $1 1
	
		${buttonVisible} "Back" 1
		${buttonVisible} "Next" 1
		${buttonVisible} "Cancel" 1
	
	${EndIf}
		
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function nsdCardDataLeave
		;skip the window when silent
	IfSilent 0 +2 
        Abort  
		
	${If} $retval <> '0'
		call FindSolutionButton_click
	${EndIf}
FunctionEnd

Function FindSolutionButton_click
    ExecShell "open" "$FAQ_url"
	;when keeping the nsis installer alive, it can permit the webbrowser to take the foreground.
	;should we quit in stead, the webbrowser will be openened in the background
	Abort
	;Quit
FunctionEnd

Function RetryCardReader_click
    Call GotoPrevPage
FunctionEnd






