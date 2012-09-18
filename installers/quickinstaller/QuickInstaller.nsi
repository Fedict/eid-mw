;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include "eIDTranslations.nsh"
!include WinMessages.nsh
;!include nsDialogs_createTextMultiline.nsh
;!include MUI2.nsh

;--------------------------------
;General

  ;defines
!define VERSION "4.1.0.7256"
!define VERSION_SHORT "4.1.0"
!define LOGFILE ""

  ;Name and file
  Name "Belgium eID-QuickInstaller ${VERSION}"
  OutFile "Belgium eID-QuickInstaller ${VERSION}.exe"
VIProductVersion "${VERSION}"
VIAddVersionKey "FileVersion" "${VERSION}"
  
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
	caption "Belgium eID-QuickInstaller ${VERSION_SHORT}"
	;SubCaption 
	
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
LicenseLangString license ${LANG_ENGLISH} "..\..\misc\licenses_files\English\License_en.rtf"
LicenseLangString license ${LANG_GERMAN} "..\..\misc\licenses_files\German\License_de.rtf"
LicenseLangString license ${LANG_FRENCH} "..\..\misc\licenses_files\French\License_fr.rtf"
LicenseLangString license ${LANG_DUTCH} "..\..\misc\licenses_files\Dutch\License_nl.rtf"

LicenseData $(license)
;LicenseText "text" "button_text"

;--------------------------------
;Installer Sections

Section "Belgium Eid Crypto Modules" BeidCrypto
	SetOutPath "$INSTDIR"
	CreateDirectory "$INSTDIR\log"
  ${If} ${RunningX64}
   ;MessageBox MB_OK "running on x64"
	 File "..\..\Windows\bin\BeidMW_64.msi"
	 ExecWait 'msiexec /quiet /norestart /l* "$APPDATA\log\install_eidmw64_log.txt" /i "$INSTDIR\BeidMW_64.msi"'
	 ;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto64" 0x1
	 Delete "$INSTDIR\BeidMW_64.msi"
  ${Else}
	;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x1
	File "..\..\Windows\bin\BeidMW_32.msi"	
	ExecWait 'msiexec /quiet /norestart /l* "$APPDATA\log\install_eidmw32_log.txt" /i "$INSTDIR\BeidMW_32.msi"'
;	$0
; /l* "$APPDATA\install_eidmw32_log.txt"
;	${if} $0 <> 0
;		DetailPrint "BeidMW_32.msi returned $0"
;		MessageBox MB_OK "An error occured while trying to install the eID Middleware $\n A logfile can be found at : $APPDATA\log\install_eidmw32_log.txt"
;	${endif}
	Delete "$INSTDIR\BeidMW_32.msi"
  ${EndIf}

SectionEnd

;--------------------------------
;Installer Functions

Function .onInit

;for testing different languages
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
    ${NSD_SetStretchedImage} $Background_Image "welcome.bmp" $Background_Image_Handle 

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
  ${NSD_SetStretchedImage} $Background_Image "welcome.bmp" $Background_Image_Handle 

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
    ${NSD_SetStretchedImage} $Background_Image "connect_reader.bmp" $Background_Image_Handle 
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function  nsdConnectReaderLeave
	beid::GetReaderCount 0
	Pop $retval
	${If} $retval <> '0'
		MessageBox MB_OK "$(ls_errorreadersearch)"
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
    ${NSD_SetStretchedImage} $Background_Image "insert_card.bmp" $Background_Image_Handle 
	
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
	
	${If} $retval == '0'
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
	${NSD_CreateLabel} 0 28u 18% 10u "$(ls_name)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 28u 85% 10u "$firstname $firstletterthirdname $lastname"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 0 42u 18% 10u "$(ls_address)"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 42u 85% 10u "$street"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 56u 85% 10u "$zip $municipality"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	;pop the others off the stack
${Else}
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
${EndIf}
	
	nsDialogs::Show
FunctionEnd
