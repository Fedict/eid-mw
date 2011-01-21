;NSIS Modern User Interface
;Multilingual Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI and 64bit checks

  !include "MUI2.nsh"
  !include "x64.nsh"

;--------------------------------
;General

  ;defines
  !define VERSION "4.0.0"
  LicenseLangString license ${LANG_ENGLISH} "..\..\misc\licenses_files\English\License_en.rtf"
  LicenseLangString license ${LANG_GERMAN} "..\..\misc\licenses_files\German\License_de.rtf"
  LicenseLangString license ${LANG_FRENCH} "..\..\misc\licenses_files\French\License_fr.rtf"
  LicenseLangString license ${LANG_DUTCH} "..\..\misc\licenses_files\Dutch\License_nl.rtf"

  ;Name and file
  Name "eID-QuickInstaller ${VERSION}"
  OutFile "eID-QuickInstaller ${VERSION}.exe"

  ;Default installation folder
  
  InstallDir "$PROGRAMFILES\Belgium Identity Card"

  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\BEID\Installer\InstallDir" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin
  ;TargetMinimalOS 5.0

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  ;!define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  ;!define MUI_LANGDLL_REGISTRY_KEY "Software\Beid Middleware NSIS" 
  ;!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE $(license)
  !insertmacro MUI_PAGE_COMPONENTS
  ;!insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Dutch"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

  
  LicenseLangString license ${LANG_ENGLISH} "..\..\misc\licenses_files\English\License_en.rtf"
  LicenseLangString license ${LANG_GERMAN} "..\..\misc\licenses_files\German\License_de.rtf"
  LicenseLangString license ${LANG_FRENCH} "..\..\misc\licenses_files\French\License_fr.rtf"
  LicenseLangString license ${LANG_DUTCH} "..\..\misc\licenses_files\Dutch\License_nl.rtf"
;--------------------------------
;Installer Sections
Section "Belgium Eid Viewer" BeidViewer

  SetOutPath "$INSTDIR"
  
  File "..\eid-viewer\Windows\bin\BeidViewer.msi"
  ExecWait 'msiexec /quiet /norestart /l* "$INSTDIR\log\install_eidviewer_log.txt" /i "$INSTDIR\BeidViewer.msi"'
  WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidViewer" 0x1
  Delete "$INSTDIR\BeidViewer.msi"
SectionEnd

Section "Belgium Eid Crypto Modules" BeidCrypto
  SetOutPath "$INSTDIR"
  ${If} ${RunningX64}
     ;MessageBox MB_OK "running on x64"
	 File "..\eid-mw\Windows\bin\BeidMW_64.msi"
	 ExecWait 'msiexec /quiet /norestart /l* "$INSTDIR\log\install_eidmw64_log.txt" /i "$INSTDIR\BeidMW_64.msi"'
	 WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto64" 0x1
	 Delete "$INSTDIR\BeidMW_64.msi"
  ${Else}
	WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x1
	File "..\eid-mw\Windows\bin\BeidMW_32.msi"
	ExecWait 'msiexec /quiet /promptrestart /l* "$INSTDIR\log\install_eidmw32_log.txt" /i "$INSTDIR\BeidMW_32.msi"'
	Delete "$INSTDIR\BeidMW_32.msi"
  ${EndIf}

SectionEnd

;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd


;--------------------------------
;Descriptions

	LangString BeidCryptoMessage ${LANG_ENGLISH} "This module is required for using the eID card as an electronic identity card in Internet Explorer, Firefox, etc. or for creating digital signatures in Microsoft Office, OpenOffice.org, Microsoft Outlook, Thunderbird, etc."
 LangString BeidCryptoMessage ${LANG_FRENCH} "Ces modules permettent d'utiliser la carte eID comme preuve d'identité dans Internet Explorer, Firefox, etc. ou pour créer des signatures numériques dans Microsoft Office, OpenOffice.org, Microsoft Outlook, Thunderbird, etc."
 LangString BeidCryptoMessage ${LANG_GERMAN} "Diese Module sind für die Benutzung der eID-Karte als elektronischen Personalausweis in Internet Explorer, Firefox, usw. oder zum Erstellen von digitalen Unterschriften in Microsoft Office, OpenOffice.org, Microsoft Outlook, Thunderbird usw. notwendig."
 LangString BeidCryptoMessage ${LANG_DUTCH} "Deze modules zijn noodzakelijk voor het gebruik van de eID kaart als elektronisch identiteitsbewijs in Internet Explorer, Firefox, etc. of voor het maken van digitale handtekeningen in Microsoft Office, OpenOffice.org, Microsoft Outlook, Thunderbird, etc."

 	LangString BeidViewerMessage ${LANG_ENGLISH} "Read the content of an e-ID card"
 LangString BeidViewerMessage ${LANG_FRENCH} "Lire le contenu de la carte e-ID"
 LangString BeidViewerMessage ${LANG_GERMAN} "Lesen Sie den Inhalt einer e-ID Karte"
 LangString BeidViewerMessage ${LANG_DUTCH} "Uitlezen van de e-ID"
 
  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${BeidCrypto} $(BeidCryptoMessage) ;"The crypto modules, used by applications to access the Belgium Eid Card"
	!insertmacro MUI_DESCRIPTION_TEXT ${BeidViewer} $(BeidViewerMessage) ;"An application that allows to view the contents and change the PIN of a Belgium Eid Card"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd