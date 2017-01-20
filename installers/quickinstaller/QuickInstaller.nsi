;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include "eIDTranslations.nsh"
!include WinMessages.nsh
!include "eidmw_version.nsh"
!include WinVer.nsh
!include "buttons.nsh"

;!include nsDialogs_createTextMultiline.nsh
;!include MUI2.nsh

;--------------------------------
;General

;Function runFinish
;    MessageBox MB_OK "runFinish called"
;	Abort
;FunctionEnd

  ;defines
!define LOGFILE ""
!addplugindir "NSIS_Plugins\beidread\Release"
  ;Name and file
  Name "Belgium eID-QuickInstaller ${EIDMW_VERSION}"
  OutFile "Belgium eID-QuickInstaller ${EIDMW_VERSION}.exe"
  VIProductVersion "${EIDMW_VERSION}"
  VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
  ;NSIS complains that Fileversion was not set for English, though it should have been done by the line above
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2017"
VIAddVersionKey "FileDescription" "Belgium eID MiddleWare"

 
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Belgium Identity Card"
	;InstallDir "$LOCALAPPDATA\Belgium Identity Card"
	
  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin
	;admin;user
	;TargetMinimalOS 5.0
	;installer will run on Win2000 and newer
  
  XPStyle on
;	WindowIcon on
	Icon beID.ico
	caption "Belgium eID-QuickInstaller ${EIDMW_VERSION_SHORT}"
	caption $(ls_caption)
	;SubCaption 
	
	Var versionMajor
	Var versionMinor
	Var retval
	Var readercount
  Var municipality
  Var zip
	Var street
  Var lastname
	Var firstletterthirdname
  Var firstname
	
	Var nsdCustomDialog
  Var Label
	Var Background_Image
	Var Background_Image_Handle
	Var Button
	Var Font_Title
	Var Font_Info
	Var Font_CardData
	Var FileToCopy
	Var LogFile
	Var MsiResponse
	Var InstallFailed
	Var ReaderFailed
	Var FindCardFailed

;--------------------------------
	;Interface Settings
	;SilentInstall silent
BrandingText " "
;"Fedict"
InstallColors /windows
;SetBrandingImage [/IMGID=item_id_in_dialog] [/RESIZETOFIT] path_to_bitmap_file.bmp
CompletedText "$(ls_completedtext)"


;InstProgressFlags smooth
;do not show installation details
ShowInstDetails nevershow
ShowUninstDetails nevershow

;  !define MUI_HEADERIMAGE
 ; !define MUI_HEADERIMAGE_BITMAP "bannrbmp.bmp" ; optional
  ;!define MUI_ABORTWARNING
;--------------------------------
;Pages

;Page license
Page custom nsdWelcome nsdWelcomeLeave

;!insertmacro MUI_PAGE_INSTFILES
;!define MUI_PAGE_CUSTOMFUNCTION_PRE dirPre
;!define MUI_FINISHPAGE_RUN_FUNCTION "runFinish"
;!insertmacro MUI_PAGE_FINISH
;!define MUI_FINISHPAGE_RUN_FUNCTION 
Page instfiles "" show_instfiles ""
Page custom nsdInstallCheck nsdInstallCheckLeave
Page custom nsdDone nsdDoneLeave
Page custom nsdConnectReader nsdConnectReaderLeave
Page custom nsdReaderCheck nsdReaderCheckLeave
Page custom nsdInsertCard nsdInsertCardLeave
Page custom nsdCardCheck nsdCardCheckLeave
Page custom nsdCardData


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

Section "Belgium Eid Crypto Modules" BeidCrypto
	SetOutPath "$INSTDIR"
	CreateDirectory "$INSTDIR\log"
	
	${WinVerGetMajor} $versionMajor
	${WinVerGetMinor} $versionMinor
	
	;${If} $versionMajor == 5
	;${AndIf} $versionMinor == 1
		;xp
		;install windows installer v4.5
	;	File "..\..\..\ThirdParty\windows-installer\WindowsXP-KB942288-v3-x86.exe"
	;	ExecWait "$INSTDIR\WindowsXP-KB942288-v3-x86.exe"
	;	Delete "$INSTDIR\WindowsXP-KB942288-v3-x86.exe"
	;${EndIf}
	
	${If} ${RunningX64}
		ClearErrors
		StrCpy $FileToCopy "$INSTDIR\BeidMW_64.msi"
		File "..\eid-mw\Windows\bin\BeidMW_64.msi"
		IfErrors 0 +2
			Call ErrorHandler_file
		ClearErrors
		;delete previous log
		StrCpy $LogFile "$INSTDIR\log\install_eidmw64_log.txt"
		;Delete "$LogFile"
		ExecWait 'msiexec /quiet /norestart /log "$LogFile" /i "$INSTDIR\BeidMW_64.msi"' $MsiResponse
		${Switch} $MsiResponse
			${Case} 0
			${Case} 3010 
				;3010 is 'success, but reboot requiered'
				;set 1 for testing, 0 otherwise
				StrCpy $InstallFailed 0
			${Break}
			${Default}
				StrCpy $InstallFailed $MsiResponse
				;Call ErrorHandler_msiexec
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
		${Switch} $MsiResponse
			${Case} 0
			${Case} 3010 
				StrCpy $InstallFailed 0
;set 1 for testing				
				;3010 is 'success, but reboot requiered'
			${Break}
			${Default}
				StrCpy $InstallFailed $MsiResponse
				;$InstallFailed=$MsiResponse
				;Call ErrorHandler_msiexec
		${EndSwitch}
		;IfErrors 0 +2
		;	Call ErrorHandler_msiexec
		;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x1
		Delete "$INSTDIR\BeidMW_32.msi"
  ${EndIf}
  
  File /r "ReaderDrivers"

${DisableX64FSRedirection}
	 
	${If} $versionMajor == 5
	${AndIf} $versionMinor == 1
		;xp
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\XP\ACR38U\a38usb.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\XP\OmniKey3121\cxbu0wdm.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\XP\SCR331\scr3xx.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\XP\VascoDP509\usbccid.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\XP\SPR532\Spr332.inf"'
	${EndIf}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 0
		;vista
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\VISTA\ACR38U\a38usb.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\VISTA\OmniKey3121\cxbu0wdm.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\VISTA\SCR331\scr3xx.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\VISTA\VascoDP509\usbccid.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\VISTA\SPR532\Spr332.inf"'
	${EndIf}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 1
		;windows 7
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a  "$INSTDIR\ReaderDrivers\WIN7\ACR38U\a38usb.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a  "$INSTDIR\ReaderDrivers\WIN7\OmniKey3121\cxbu0wdm.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a  "$INSTDIR\ReaderDrivers\WIN7\SCR331\scr3xx.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a  "$INSTDIR\ReaderDrivers\WIN7\VascoDP509\usbccid.inf"'
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\WIN7\SPR532\Spr332.inf"'
	${EndIf}
		
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 2 
		;windows 8
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\WIN8\ACR38U\a38usb.inf"'
	${EndIf}
	
	${If} $versionMajor == 6
	${AndIf} $versionMinor == 3 
		;windows 8.1
		nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\WIN8\ACR38U\a38usb.inf"'
	${EndIf}
	;driver to install on all OS's

	nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\ReaderDrivers\WINALL\APG8201Z\apg8201z.inf"'

${EnableX64FSRedirection}

  RMDir /r /REBOOTOK $INSTDIR\ReaderDrivers

SectionEnd

;--------------------------------
;Error Messages
;Function ErrorHandler_msiexec
 ; MessageBox MB_ICONSTOP "$(ls_errorinstallmsi) $LogFile"
  ;  Abort
;FunctionEnd

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
File /oname=$PLUGINSDIR\"done.bmp" "done.bmp"
File /oname=$PLUGINSDIR\"insert_card.bmp" "insert_card.bmp"
File /oname=$PLUGINSDIR\"connect_reader.bmp" "connect_reader.bmp"
File /oname=$PLUGINSDIR\"Failed.bmp" "failed.bmp"
;File /oname=$PLUGINSDIR\"bannrbmp.bmp" "bannrbmp.bmp"

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

Function nsdWelcome
	;File "welcome.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog ;popped from the stack to prevent stack corruption
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 25% 100% 16u "Welkom!"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 50% 100% 12u "Klik op 'Installeren' om de eID software te installeren"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Info "Arial" "9" "500" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Info 1
	SetCtlColors $Label 0x000000 transparent
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
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

	${buttonVisible} "Back" 0
FunctionEnd


Function nsdInstallCheck
    ${If} $InstallFailed == 0
        ;MessageBox MB_OK "Install ok, skipping error"
        Abort   
    ${EndIf}
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	;${NSD_CreateButton} 25% 25% 50% 25% "Find Solution"
	;${NSD_CreateButton} -20u -10u 20u 10u "Find Solution"
	;Pop $button
	;${NSD_OnClick} $button FindSolutionButton_click
	;EnableWindow $button 0 # start out disabled	
	
	;${NSD_CreateTextMultiline} 
	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_install_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 16u "FOUTMELDING $InstallFailed"
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
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Failed.bmp" $Background_Image_Handle 

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
	;File "welcome.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

;		${NSD_CreateButton} 25% 25% 50% 25% "eID Software testen"
;		Pop $button
;		${NSD_OnClick} $button button_click
		;EnableWindow $button 0 # start out disabled	
	
	;${NSD_CreateTextMultiline} 

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
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Done.bmp" $Background_Image_Handle 

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
;	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function  nsdDoneLeave
FunctionEnd

Function nsdConnectReader
	;File "connect_reader.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog

	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	
;	${NSD_CreateButton} 25% 25% 50% 25% "Volgende"
;	Pop $button
;	${NSD_OnClick} $button button_click
	
	${NSD_CreateLabel} 0 -20u 100% 18u "Sluit uw kaartlezer aan"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	

	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapconnectreader)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\connect_reader.bmp" $Background_Image_Handle 
	
	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function  nsdConnectReaderLeave
	beid::GetReaderCount 0
	Pop $retval
	
	${If} $retval <> '0'
		StrCpy $ReaderFailed $retval
		;MessageBox MB_OK "$(ls_errorreadersearch) $\r$\n $(ls_error) = $retval"
		;Abort
	${EndIf}
  Pop $readercount
	;${If} $readercount > 0
	${If} $readercount == 0
		StrCpy $ReaderFailed 0
		;MessageBox MB_OK "$$readercount is $readercount"
	${Else}
		;MessageBox MB_OK "$(ls_noreaderfound)"
		StrCpy $ReaderFailed "no_readers_found"
		;Abort
	${EndIf}
FunctionEnd

Function nsdReaderCheck
    ${If} $ReaderFailed == 0
        ;MessageBox MB_OK "Reader found, skipping reader error"
        Abort   
    ${EndIf}
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	;	${NSD_CreateButton} 25% 25% 50% 25% "Opnieuw"
	;	Pop $button
	;	${NSD_OnClick} $button RetryCardReader_click
		;EnableWindow $button 0 # start out disabled	
	
	;${NSD_CreateTextMultiline} 
	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_cardreader_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${NSD_CreateLabel} 0 60% 100% 16u "FOUTMELDING $ReaderFailed"
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
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
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
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_helpsite)"
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function nsdReaderCheckLeave
	call FindSolutionButton_click
FunctionEnd


Function nsdInsertCard 
	;File "insert_card.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	${NSD_CreateLabel} 0 -20u 100% 18u "$(ls_pleaseinsertcard)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	

	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapinsertcard)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\insert_card.bmp" $Background_Image_Handle 
	
	${buttonVisible} "Back" 1
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 1
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function nsdInsertCardLeave
	;GetReaderCount 1 in order to get the readers with an eID card inserted
	beid::GetReaderCount 1	
	Pop $retval
	
	;the instruction below is just for testing
	StrCpy $retval 004180
	
	${If} $retval <> '0'
		StrCpy $FindCardFailed $retval
		;MessageBox MB_OK "$(ls_errorreadingcard)"
		;Abort
	${EndIf}
	Pop $readercount
	${If} $readercount > 0
		StrCpy $FindCardFailed 0
		;MessageBox MB_OK "number of beidcards found is $readercount"
	${Else}
		StrCpy $FindCardFailed "no_readers_found"
		;MessageBox MB_OK "$(ls_nocardfound)"
		;Abort
	${EndIf}
FunctionEnd

Function nsdCardCheck
    ${If} $FindCardFailed <> '0'
        ;MessageBox MB_OK "Reader found, skipping reader error"
        Abort   
    ${EndIf}
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 40% 100% 20u "$(ls_test_failed)"
	Pop $Label
	SetCtlColors $Label 0x008080 transparent
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Arial" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	
	${If} $retval <> '0'
		${NSD_CreateLabel} 0 60% 100% 16u "FOUTMELDING R$retval"
	${Else}
		${NSD_CreateLabel} 0 60% 100% 16u "FOUTMELDING I$InstallFailed"
	${EndIf}	
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
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
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
	

	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
FunctionEnd

Function nsdCardCheckLeave
	call FindSolutionButton_click
FunctionEnd


Function nsdCardData
	nsDialogs::Create 1018
	Pop $nsdCustomDialog	
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
		
	beid::ReadCardData
	Pop $retval
	
	${If} $retval <> '0'
	  ;MessageBox MB_OK "$$retval is $retval"
	${NSD_CreateLabel} 0 0 18% 20% "$(ls_error)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 0 100% 20% "$retval"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 0 20% 100% 100% "$(ls_cardread)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	SendMessage $Label ${WM_SETTEXT} 0 "$(ls_testfailed)"
	;Goto nsdCardDataDone
	Call GotoPrevPage
	
	${EndIf}
	
	;MessageBox MB_OK "$$retval is 0"
	Pop $municipality
	Pop $zip
	Pop $street
	Pop $lastname
	Pop $firstletterthirdname
	Pop $firstname

	${NSD_CreateLabel} 0 40% 100% 36u "$(ls_testcomplete_pre) $firstname$(ls_testcomplete_post)"
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
	
	${NSD_CreateBitmap} 0 0 100% 100% "$(ls_bitmapwelcome)"
	Pop $Background_Image
	SetCtlColors $Background_Image 0xFFFFFF transparent
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\Done.bmp" $Background_Image_Handle 

	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_test)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	;EnableWindow $NextButton 1 ;enable the previous button
	;SetCtlColors $NextButton 0xFF0000 0x00FF00
	
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_close)"
	
	${buttonVisible} "Back" 0
	${buttonVisible} "Next" 1
	${buttonVisible} "Cancel" 0	
	
	
	
;	${NSD_CreateLabel} 0 0 100% 16u "$(ls_cardread)"
;	Pop $Label
;	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
;	SendMessage $Label ${WM_SETFont} $Font_Title 1
;	;SendMessage $Label ${WM_SETTEXT} 0 "STR:Card Read"
;
;	CreateFont $Font_CardData "Times New Roman" "14" "500" ;/UNDERLINE
;	${NSD_CreateLabel} 0 28u 18% 14u "$(ls_name)"
;	Pop $Label
;	SendMessage $Label ${WM_SETFont} $Font_CardData 1
;	${if} $firstletterthirdname == ""
;		${NSD_CreateLabel} 20% 28u 85% 14u "$firstname $lastname"
;	${Else}
;		${NSD_CreateLabel} 20% 28u 85% 14u "$firstname $firstletterthirdname $lastname"
;	${EndIf}
;	Pop $Label
;	SendMessage $Label ${WM_SETFont} $Font_CardData 1
;	${NSD_CreateLabel} 0 42u 18% 14u "$(ls_address)"
;	Pop $Label
;	SendMessage $Label ${WM_SETFont} $Font_CardData 1
;	${NSD_CreateLabel} 20% 42u 85% 14u "$street"
;	Pop $Label
;	SendMessage $Label ${WM_SETFont} $Font_CardData 1
;	${NSD_CreateLabel} 20% 56u 85% 14u "$zip $municipality"
;	Pop $Label
;	SendMessage $Label ${WM_SETFont} $Font_CardData 1
;	;pop the others off the stack
;

	
	nsdCardDataDone:
	nsDialogs::Show
FunctionEnd

Function FindSolutionButton_click
    ExecShell "open" "http://eid.belgium.be/"
	;when keeping the nsis installer alive, it can permit the webbrowser to take the foreground.
	;should we quit in stead, the webbrowser will be openened in the background
	Abort
	;Quit
FunctionEnd

Function RetryCardReader_click
    Call GotoPrevPage
FunctionEnd






