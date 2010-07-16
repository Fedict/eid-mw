;---------------------------------------------------------
; warning experimental ; absolute paths to my homedir etc..
; -frank
;---------------------------------------------------------

!include "MUI2.nsh"
!include "x64.nsh"
Name "Belgian eID Middleware"
OutFile "beid-mw-3.99.M2.exe"

InstallDir "$SYSDIR"
InstallDirRegKey HKCU "Software\beid-middleware" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings
  !define MUI_ABORTWARNING
  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES
!define MUI_ICON "eid-mw-install.ico"
!define MUI_UNICON "eid-mw-uninstall.ico"


;--------------------------------
;Language Selection Dialog Settings
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\beid-middleware" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages
  !insertmacro MUI_PAGE_LICENSE "COPYING"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
  !insertmacro MUI_LANGUAGE "Dutch"   ;first language is the default language
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "English" 


;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------

;Installer Sections

Section "beID Core" SecCoreLibs
  SectionIn RO
  SetOutPath "$INSTDIR"
  File /home/frank/dist/win32/bin/libbeidcommon.dll
  File /home/frank/dist/win32/bin/libbeiddialogs.dll
  File /home/frank/dist/win32/bin/libbeidcardlayer.dll
  ;Store installation folder
  WriteRegStr HKCU "Software\beid-corelibs" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section "minidriver" SecMiniDriver
  SetOutPath "$INSTDIR"
  File /home/frank/dist/win32/bin/libbeidpkcs11.dll
  ;Store installation folder
  WriteRegStr HKCU "Software\beid-minidriver" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd


Section "pkcs#11 Module" SecPkcs11
  SetOutPath "$INSTDIR"
  File /home/frank/dist/win32/bin/libbeidpkcs11.dll
  ;Store installation folder
  WriteRegStr HKCU "Software\beid-pkcs11" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section "SIS Card Modules" SecSisCard
  SetOutPath "$INSTDIR"
  File /home/frank/dist/win32/bin/libcardpluginbeid.dll
  File /home/frank/dist/win32/bin/libcardpluginsis_acr38u.dll
  File /home/frank/dist/win32/bin/libcardpluginsis.dll
  ;Store installation folder
  WriteRegStr HKCU "Software\beid-sis" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section Test
	SetOutPath $SYSDIR
	; 32-bit install:
	${EnableX64FSRedirection}
	File /home/frank/dist/win32/bin/libbeidmdrv32.dll  # extracts to C:\Windows\SysWOW64 
	; 64-bit install:
	${If} ${RunningX64}
		MessageBox MB_OK "Running on X64"
		${DisableX64FSRedirection}
		File /home/frank/dist/win64/bin/libbeidmdrv64.dll  # extracts to C:\Windows\System32 
	${EndIf}
SectionEnd

;--------------------------------
;Installer Functions

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------
;Descriptions

LangString DESC_SecCoreLibs ${LANG_ENGLISH} "Core Libraries (required for other modules)"
LangString DESC_SecCoreLibs ${LANG_DUTCH}   "Basissoftware (vereist voor de andere modules)"
LangString DESC_SecCoreLibs ${LANG_FRENCH}  "Logiciel de base (prerequis pour les autre modules)"
LangString DESC_SecCoreLibs ${LANG_GERMAN}  "Basis Software (erfordert für andere Module)"

LangString DESC_SecPkcs11   ${LANG_ENGLISH} "PKCS#11 Module (e.g. for using TaxOnWeb in Firefox)"
LangString DESC_SecPkcs11   ${LANG_DUTCH}   "PKCS#11 Module (b.v. om TaxOnWeb in Firefox te gebruiken)"
LangString DESC_SecPkcs11   ${LANG_FRENCH}  "Module PKCS#11 (pour utiliser TaxOnWeb avec Firefox, par exemple.)"
LangString DESC_SecPkcs11   ${LANG_GERMAN}  "PKCS#11 Modul (z.b. für TaxOnWeb auf Firefox.)"

LangString DESC_SecSisCard   ${LANG_ENGLISH} "SIS Card Modules (allows reading SIS cards on particular card readers)"
LangString DESC_SecSisCard   ${LANG_DUTCH}   "SIS-kaart modules (laat toe met bepaalde kaartlezers SIS-kaarten uit te lezen)"
LangString DESC_SecSisCard   ${LANG_FRENCH}  "Module carte SIS (permet de lire les cartes SIS avec certains lecteurs de cartes)"
LangString DESC_SecSisCard   ${LANG_GERMAN}  "SIS-Karte Modul (ermöglicht bestimmten Kartenlezer SIS-Karten zu lesen)" 

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCoreLibs} $(DESC_SecCoreLibs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPkcs11}   $(DESC_SecPkcs11)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSisCard}  $(DESC_SecSisCard)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

 
;--------------------------------
;Uninstaller Section

Section "Uninstall"
  Delete "$INSTDIR\Uninstall.exe"
  DeleteRegKey /ifempty HKCU "Software\beid-middleware"
SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd
