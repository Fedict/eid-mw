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

  ;Name and file
  Name "eID-QuickInstaller ${VERSION}"
  OutFile "eID-QuickInstaller ${VERSION}.exe"

  ;Default installation folder
  
  InstallDir "$PROGRAMFILES\Belgium Identity Card"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\BEID\Installer\InstallDir" ""

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

  !insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
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

;--------------------------------
;Installer Sections

Section "Belgium Eid Crypto Modules" BeidCrypto
  SetOutPath "$INSTDIR"

  WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x1
  File ".\BeidMW.msi"
  ExecWait 'msiexec /quiet /i "$INSTDIR\BeidMW.msi"'
  ${If} ${RunningX64}
     ;MessageBox MB_OK "running on x64"
	 File ".\BeidMW-64.msi"
	 ExecWait 'msiexec /quiet /i "$INSTDIR\BeidMW-64.msi"'
	 WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto64" 0x1
  ${EndIf}

  ;Store installation folder
  ;WriteRegStr HKCU "Software\UninstallBeidMiddleWare" $INSTDIR
  
  ;Create uninstaller
  ;WriteUninstaller "$INSTDIR\UninstallBeidMiddleWare.exe"

SectionEnd

Section "Belgium Eid Viewer" BeidViewer

  SetOutPath "$INSTDIR"
  
  File ".\BeidViewer.msi"
  ExecWait 'msiexec /quiet /i "$INSTDIR\BeidViewer.msi"'
  WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidViewer" 0x1
  ;ADD YOUR OWN FILES HERE...
  
  ;Store installation folder
  ;WriteRegStr HKCU "Software\UninstallBeidViewer" "" $INSTDIR
  
  ;Create uninstaller
  ;WriteUninstaller "$INSTDIR\UninstallBeidViewer.exe"

SectionEnd



;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

Function .onGUIEnd

  ;Store installation folder
  WriteRegStr HKCU "Software\BEID\Installer\UninstallBeid" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\UninstallBeid.exe"

FunctionEnd

;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC
	LangString BeidCryptoMessage ${LANG_ENGLISH} "English message"
 LangString BeidCryptoMessage ${LANG_FRENCH} "French message"
 LangString BeidCryptoMessage ${LANG_GERMAN} "German message"
 LangString BeidCryptoMessage ${LANG_DUTCH} "Dutch message"

 	LangString BeidViewerMessage ${LANG_ENGLISH} "English message"
 LangString BeidViewerMessage ${LANG_FRENCH} "French message"
 LangString BeidViewerMessage ${LANG_GERMAN} "German message"
 LangString BeidViewerMessage ${LANG_DUTCH} "Dutch message"
 
  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${BeidCrypto} $(BeidCryptoMessage) ;"The crypto modules, used by applications to access the Belgium Eid Card"
	!insertmacro MUI_DESCRIPTION_TEXT ${BeidViewer} $(BeidViewerMessage) ;"An application that allows to view the contents and change the PIN of a Belgium Eid Card"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  ReadRegDWORD $0 HKCU Software\BEID\Installer\Components BeidViewer
  IntCmp $0 0x0 noviewer
	ExecWait 'msiexec /uninstall "$INSTDIR\BeidViewer.msi"'
	WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidViewer" 0x0
	Delete "$INSTDIR\BeidViewer.msi"
  noviewer:
  
  ReadRegDWORD $0 HKCU Software\BEID\Installer\Components BeidCrypto32
  IntCmp $0 0x0 noCrypto32
	ExecWait 'msiexec /uninstall "$INSTDIR\BeidMW.msi"'
	WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto32" 0x0
	Delete "$INSTDIR\BeidMW.msi"
  noCrypto32:
  
  ReadRegDWORD $0 HKCU Software\BEID\Installer\Components BeidCrypto64
  IntCmp $0 0x0 noCrypto64
	ExecWait 'msiexec /uninstall "$INSTDIR\BeidMW-64.msi"'
	WriteRegDWORD HKCU "Software\BEID\Installer\Components" "BeidCrypto64" 0x0
	Delete "$INSTDIR\BeidMW-64.msi"
  noCrypto64:
  
  Delete "$INSTDIR\UninstallBeid.exe"
  RMDir "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\BEID\Installer\UninstallBeid"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd