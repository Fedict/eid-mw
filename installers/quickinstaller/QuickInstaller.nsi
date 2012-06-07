;--------------------------------
;Include nsdialogs and 64bit checks

!include "x64.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
;!include nsDialogs_createTextMultiline.nsh
;!include MUI2.nsh

;--------------------------------
;General

  ;defines
  !define VERSION "4.0.4.7256g"
	!define VERSION_SHORT "4.0.4"
	!define LOGFILE ""
;  LicenseLangString license ${LANG_ENGLISH} "..\..\misc\licenses_files\English\License_en.rtf"
;  LicenseLangString license ${LANG_GERMAN} "..\..\misc\licenses_files\German\License_de.rtf"
;  LicenseLangString license ${LANG_FRENCH} "..\..\misc\licenses_files\French\License_fr.rtf"
;  LicenseLangString license ${LANG_DUTCH} "..\..\misc\licenses_files\Dutch\License_nl.rtf"

  ;Name and file
  Name "Belgium eID-QuickInstaller ${VERSION}"
  OutFile "Belgium eID-QuickInstaller ${VERSION}.exe"

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

VIProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Belgium eID Middleware"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Software for reading the Belgium Identity Card"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Fedict"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright © 2012"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "eID QuickInstaller"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"


;--------------------------------
;Pages
Page custom nsdWelcome nsdWelcomeLeave
Page instfiles "" show_instfiles ""
Page custom nsdDone nsdDoneLeave
Page custom nsdConnectReader nsdConnectReaderLeave
Page custom nsdInsertCard nsdInsertCardLeave
Page custom nsdCardData

;--------------------------------
;Languages
; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Dutch.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  ;!insertmacro MUI_RESERVEFILE_LANGDLL

LangString ls_install ${LANG_ENGLISH} "Install"
LangString ls_install ${LANG_FRENCH} "Installez"
LangString ls_install ${LANG_DUTCH} "Installeer"

LangString ls_cancel ${LANG_ENGLISH} "Cancel"
LangString ls_cancel ${LANG_FRENCH} "Anulez"
LangString ls_cancel ${LANG_DUTCH} "Anuleer"

LangString ls_next ${LANG_ENGLISH} "Next"
LangString ls_next ${LANG_FRENCH} "Prochaine"
LangString ls_next ${LANG_DUTCH} "Volgende"

LangString ls_back ${LANG_ENGLISH} "Back"
LangString ls_back ${LANG_FRENCH} "Précédente"
LangString ls_back ${LANG_DUTCH} "Vorige"

LangString ls_close ${LANG_ENGLISH} "Close"
LangString ls_close ${LANG_FRENCH} "Fermer"
LangString ls_close ${LANG_DUTCH} "Sluiten"

LangString ls_finish ${LANG_ENGLISH} "Finish"
LangString ls_finish ${LANG_FRENCH} "OK"
LangString ls_finish ${LANG_DUTCH} "OK"

LangString ls_test ${LANG_ENGLISH} "Test"
LangString ls_test ${LANG_FRENCH} "Tester"
LangString ls_test ${LANG_DUTCH} "Testen"

MiscButtonText $(ls_back) $(ls_next) $(ls_cancel) $(ls_close)
; MessageBox MB_OK "A translated message: $(message)"
;  LicenseLangString license ${LANG_ENGLISH} "..\..\misc\licenses_files\English\License_en.rtf"
;  LicenseLangString license ${LANG_GERMAN} "..\..\misc\licenses_files\German\License_de.rtf"
;  LicenseLangString license ${LANG_FRENCH} "..\..\misc\licenses_files\French\License_fr.rtf"
;  LicenseLangString license ${LANG_DUTCH} "..\..\misc\licenses_files\Dutch\License_nl.rtf"
;--------------------------------
;Installer Sections
;Section "Belgium Eid Viewer" BeidViewer

  ;SetOutPath "$INSTDIR"
  
  ;File "..\eid-viewer\Windows\bin\BeidViewer.msi"
  ;ExecWait 'msiexec /quiet /norestart /l* "$INSTDIR\log\install_eidviewer_log.txt" /i "$INSTDIR\BeidViewer.msi"'
  ;WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidViewer" 0x1
  ;Delete "$INSTDIR\BeidViewer.msi"
;SectionEnd

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

	${NSD_CreateLabel} 0 0 100% 16u "Welcome to the Eid QuickInstaller"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	CreateFont $Font_Title "Times New Roman" "18" "700" ;/UNDERLINE
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u ""
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
	${NSD_CreateLabel} 0 0 100% 16u "Installation complete" ; $\r$\n if you'd like to test reading your eidcard, press next"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u ""
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
	
	${NSD_CreateLabel} 0 0 100% 16u "Please connect your cardreader"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "Type something here..."
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "connect_reader.bmp" $Background_Image_Handle 
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function  nsdConnectReaderLeave
	beid::GetReaderCount 0
	Pop $retval
	${If} $retval <> '0'
		MessageBox MB_OK "Error while searching for cardreaders"
		Abort
	${EndIf}
  Pop $readercount
	${If} $readercount > 0
		;MessageBox MB_OK "$$readercount is $readercount"
	${Else}
		MessageBox MB_OK "No cardreader was found"
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
	${NSD_CreateLabel} 0 0 100% 16u "Please insert your beid card"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1

	${NSD_CreateBitmap} 0 18u 100% -13u "Type something here..."
	Pop $Background_Image
    ${NSD_SetStretchedImage} $Background_Image "insert_card.bmp" $Background_Image_Handle 
	
	nsDialogs::Show
	${NSD_FreeImage} $Background_Image_Handle
FunctionEnd

Function nsdInsertCardLeave
	beid::GetReaderCount 1	
	Pop $retval
	${If} $retval <> '0'
		MessageBox MB_OK "Error while trying to read from card"
		Abort
	${EndIf}
  Pop $readercount
	${If} $readercount > 0
		;MessageBox MB_OK "number of beidcards found is $readercount"
	${Else}
		MessageBox MB_OK "No beidcard was found"
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

	${NSD_CreateLabel} 0 0 100% 16u "Card Read"
	Pop $Label
	${NSD_AddStyle} $Label ${SS_CENTER} ;center the text
	SendMessage $Label ${WM_SETFont} $Font_Title 1
	;SendMessage $Label ${WM_SETTEXT} 0 "STR:Card Read"

	CreateFont $Font_CardData "Times New Roman" "14" "500" ;/UNDERLINE
	${NSD_CreateLabel} 0 28u 18% 10u "Name:"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 28u 85% 10u "$firstname $firstletterthirdname $lastname"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 0 42u 18% 10u "Adress:"
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
	${NSD_CreateLabel} 0 0 18% 20% "Error:"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 20% 0 100% 20% "$retval"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	${NSD_CreateLabel} 0 20% 100% 100% "Card Read"
	Pop $Label
	SendMessage $Label ${WM_SETFont} $Font_CardData 1
	SendMessage $Label ${WM_SETTEXT} 0 "STR:Test failed,$\n$\ngo back and try again,$\nor reboot your pc and launch the eid viewer to continue testing"
		;if retval != 0, dont read out values, but print a 'restart' message
${EndIf}
	
	nsDialogs::Show
FunctionEnd
