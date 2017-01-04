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
!addplugindir "NSIS_Plugins\beidread\Release"
  ;Name and file
  Name "Belgium eID-QuickInstaller ${EIDMW_VERSION}"
  OutFile "Belgium eID-QuickInstaller ${EIDMW_VERSION}.exe"
  VIProductVersion "${EIDMW_VERSION}"
  VIAddVersionKey "FileVersion" "${EIDMW_VERSION}"
  ;NSIS complains that Fileversion was not set for English, though it should have been done by the line above
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${EIDMW_VERSION}"
VIAddVersionKey "CompanyName" "Belgian Government"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2015"
VIAddVersionKey "FileDescription" "Belgium eID MiddleWare"

 
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Belgium Identity Card"
	;InstallDir "$LOCALAPPDATA\Belgium Identity Card"
	
  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin
	;admin;user
	;TargetMinimalOS 5.0
	;installer will run on Win2000 and newer
  
  ;XPStyle on
	WindowIcon on
	Icon Setup.ico
	caption "Belgium eID-QuickInstaller ${EIDMW_VERSION_SHORT}"
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
	Var Font_CardData
	Var FileToCopy
	Var LogFile
	Var MsiResponse

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
Page custom nsdWelcome nsdWelcomeLeave
Page instfiles "" show_instfiles ""
Page custom nsdDone nsdDoneLeave
Page custom nsdConnectReader nsdConnectReaderLeave
Page custom nsdInsertCard nsdInsertCardLeave
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
			${Break}
			${Default}
				Call ErrorHandler_msiexec
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
				;3010 is 'success, but reboot requiered'
			${Break}
			${Default}
				Call ErrorHandler_msiexec
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
Function ErrorHandler_msiexec
  MessageBox MB_ICONSTOP "$(ls_errorinstallmsi) $LogFile"
    Abort
FunctionEnd

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
File /oname=$PLUGINSDIR\"insert_card.bmp" "insert_card.bmp"
File /oname=$PLUGINSDIR\"connect_reader.bmp" "connect_reader.bmp"


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
	
	${NSD_CreateLabel} 0 0 100% 16u "$(ls_welcome)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Times New Roman" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "$(ls_bitmapwelcome)"
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\welcome.bmp" $Background_Image_Handle 

	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_install)"
	
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
FunctionEnd

Function nsdDone
	;File "welcome.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}

	;${NSD_CreateTextMultiline} 
	${NSD_CreateLabel} 0 0 100% 16u "$(ls_complete)" ; $\r$\n if you'd like to test reading your eidcard, press next"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "$(ls_bitmapwelcome)"
	Pop $Background_Image
  ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\welcome.bmp" $Background_Image_Handle 

	;GetDlgItem $NextButton $nsdDoneDialog 1 ; next=1, cancel=2, back=3
	GetDlgItem $Button $HWNDPARENT 1 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_test)"
	GetDlgItem $Button $HWNDPARENT 2 ; next=1, cancel=2, back=3
	SendMessage $Button ${WM_SETTEXT} 0 "STR:$(ls_finish)"
	;EnableWindow $NextButton 1 ;enable the previous button
	;SetCtlColors $NextButton 0xFF0000 0x00FF00
		
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
	
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
	
	${NSD_CreateLabel} 0 0 100% 16u "$(ls_pleaseconnect)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "$(ls_bitmapconnectreader)"
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\connect_reader.bmp" $Background_Image_Handle 
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function  nsdConnectReaderLeave
	beid::GetReaderCount 0
	Pop $retval
	${If} $retval <> '0'
		MessageBox MB_OK "$(ls_errorreadersearch) $\r$\n $(ls_error) = $retval"
		Abort
	${EndIf}
  Pop $readercount
	${If} $readercount > 0
		;MessageBox MB_OK "$$readercount is $readercount"
	${Else}
		MessageBox MB_OK "$(ls_noreaderfound)"
		Abort
	${EndIf}
FunctionEnd

Function nsdInsertCard 
	;File "insert_card.bmp"
	nsDialogs::Create 1018
	Pop $nsdCustomDialog
	${If} $nsdCustomDialog == error
		Abort
	${EndIf}
	${NSD_CreateLabel} 0 0 100% 16u "$(ls_pleaseinsertcard)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "$(ls_bitmapinsertcard)"
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "$PLUGINSDIR\insert_card.bmp" $Background_Image_Handle 
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function nsdInsertCardLeave
	beid::GetReaderCount 1	
	Pop $retval
	${If} $retval <> '0'
		MessageBox MB_OK "$(ls_errorreadingcard)"
		Abort
	${EndIf}
  Pop $readercount
	${If} $readercount > 0
		;MessageBox MB_OK "number of beidcards found is $readercount"
	${Else}
		MessageBox MB_OK "$(ls_nocardfound)"
		Abort
	${EndIf}
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
	Goto nsdCardDataDone
	${EndIf}
	
	;MessageBox MB_OK "$$retval is 0"
	Pop $municipality
	Pop $zip
	Pop $street
	Pop $lastname
	Pop $firstletterthirdname
	Pop $firstname

	${NSD_CreateLabel} 0 0 100% 16u "$(ls_cardread)"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	;SendMessage $Label ${WM_SETTEXT} 0 "STR:Card Read"

	CreateFont $Font_CardData "Times New Roman" "14" "500" ;/UNDERLINE
	${NSD_CreateLabel} 0 28u 18% 14u "$(ls_name)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${if} $firstletterthirdname == ""
		${NSD_CreateLabel} 20% 28u 85% 14u "$firstname $lastname"
	${Else}
		${NSD_CreateLabel} 20% 28u 85% 14u "$firstname $firstletterthirdname $lastname"
	${EndIf}
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 0 42u 18% 14u "$(ls_address)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 42u 85% 14u "$street"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 56u 85% 14u "$zip $municipality"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	;pop the others off the stack

	nsdCardDataDone:
	nsDialogs::Show
FunctionEnd
