!include "MUI2.nsh"
!include "x64.nsh"
Name "Belgian eID Middleware"
OutFile "eid-mw.exe"

InstallDir "$SYSDIR"
InstallDirRegKey HKCU "Software\beid-middleware" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings
  !define MUI_ABORTWARNING
  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

  ; pretty icons
!define MUI_ICON "meta/eid-mw-install.ico"
!define MUI_UNICON "meta/eid-mw-uninstall.ico"

;--------------------------------
;Language Selection Dialog Settings
  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\beid-middleware" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages
  !insertmacro MUI_PAGE_LICENSE "meta/COPYING"
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


; Core Libraries ; needed for pkcs#11 and CSP
; -----------------------------------------------------------------------------------------
Section "beID Core" SecCoreLibs
  SectionIn RO
  SetOutPath "$INSTDIR"

  ; 32-bit install:
  ${EnableX64FSRedirection}
  File dist/win32/usr/local/bin/libgcc_s_sjlj-1.dll
  File dist/win32/usr/local/bin/libstdc++-6.dll
  File dist/win32/usr/local/bin/libbeidcommon.dll
  File dist/win32/usr/local/bin/libbeiddialogs.dll
  File dist/win32/usr/local/bin/libbeidcardlayer.dll

	${If} ${RunningX64}
		; 64-bit install:
		${DisableX64FSRedirection}
  		File dist/win64/usr/local/bin/w64gcc_s_sjlj-1.dll
  		File dist/win64/usr/local/bin/libstdc++-6.dll
  		File dist/win64/usr/local/bin/libbeidcommon.dll
  		File dist/win64/usr/local/bin/libbeiddialogs.dll
  		File dist/win64/usr/local/bin/libbeidcardlayer.dll
	${EndIf}

  ;Store installation folder
  WriteRegStr HKCU "Software\beid-corelibs" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd


; minidriver ; for IE and other proprietary apps on Windows Vista and Above
; -----------------------------------------------------------------------------------------
Section "minidriver" SecMiniDriver
	SetOutPath $INSTDIR

	; 32-bit install:
	${EnableX64FSRedirection}
	File /oname=beidmdrv.dll dist/win32/usr/local/bin/libbeidmdrv32.dll  # extracts to C:\Windows\SysWOW64 ; rename because registry reference

	${If} ${RunningX64}
		; 64-bit install:
		${DisableX64FSRedirection}
		File /oname=beidmdrv.dll dist/win64/usr/local/bin/libbeidmdrv64.dll # extracts to C:\Windows\System32 ; rename because registry reference
	${EndIf}

  ;Store installation folder
  WriteRegStr HKCU "Software\beid-minidriver" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

; CSP+Certificate Propagation Service ; for IE and other proprietary apps on Windows XP
; -----------------------------------------------------------------------------------------
Section "csp" SecCSPAndPropagationService
	SetOutPath $INSTDIR

	; 32-bit install:
	${EnableX64FSRedirection}
	File /oname=beidmdrv.dll dist/win32/usr/local/bin/libbeidmdrv32.dll  # extracts to C:\Windows\SysWOW64 ; rename because registry reference

	${If} ${RunningX64}
		; 64-bit install:
		${DisableX64FSRedirection}
		File /oname=beidmdrv.dll dist/win64/usr/local/bin/libbeidmdrv64.dll # extracts to C:\Windows\System32 ; rename because registry reference
	${EndIf}

  ;Store installation folder
  WriteRegStr HKCU "Software\beid-minidriver" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd


; pkcs11 module ; for Firefox and other standards-aware apps
; -----------------------------------------------------------------------------------------
Section "pkcs#11 Module" SecPkcs11
	SetOutPath $INSTDIR

	; 32-bit install:
	${EnableX64FSRedirection}
  	File dist/win32/usr/local/bin/libbeidpkcs11.dll

	${If} ${RunningX64}
		; 64-bit install:
		${DisableX64FSRedirection}
  		File dist/win64/usr/local/bin/libbeidpkcs11.dll
	${EndIf}

  ;Store installation folder
  WriteRegStr HKCU "Software\beid-pkcs11" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd


; SISCard ; for reading Belgian SIS cards on certain readers
; -----------------------------------------------------------------------------------------

Section "SIS Card Modules" SecSisCard
	SetOutPath $INSTDIR
	; 32-bit install:
	${EnableX64FSRedirection}
  	File dist/win32/usr/local/bin/libcardpluginbeid.dll
  	File dist/win32/usr/local/bin/libcardpluginsis_acr38u.dll
  	File dist/win32/usr/local/bin/libcardpluginsis.dll

	${If} ${RunningX64}
		; 64-bit install:
		${DisableX64FSRedirection}
  		File dist/win64/usr/local/bin/libcardpluginbeid.dll
  		File dist/win64/usr/local/bin/libcardpluginsis_acr38u.dll
  		File dist/win64/usr/local/bin/libcardpluginsis.dll
	${EndIf}

  ;Store installation folder
  WriteRegStr HKCU "Software\beid-sis" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd


;--------------------------------
;Installer Functions

Function .onInit
	!insertmacro MUI_LANGDLL_DISPLAY

	Push $R0
	Push $R1
	ClearErrors
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
	IfErrors 0 lbl_winnt
 
; we are not NT
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
	StrCpy $R1 $R0 1
	StrCmp $R1 '4' 0 lbl_error
	StrCpy $R1 $R0 3
	StrCmp $R1 '4.0' lbl_win32_95
	StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
lbl_win32_95:
	StrCpy $R0 '95'
	MessageBox MB_OK|MB_ICONQUESTION "Windows 95 is not supported by eid-mw" IDOK 
	Quit
    Goto lbl_done
 
lbl_win32_98:
	StrCpy $R0 '98'
	MessageBox MB_OK|MB_ICONQUESTION "Windows 98 is not supported by eid-mw" IDOK 
	Quit
    Goto lbl_done
 
lbl_win32_ME:
	StrCpy $R0 'ME'
	MessageBox MB_OK|MB_ICONQUESTION "Windows ME is not supported by eid-mw" IDOK 
	Quit
    Goto lbl_done
 
lbl_winnt:
	StrCpy $R1 $R0 1
	StrCmp $R1 '3' lbl_winnt_x
	StrCmp $R1 '4' lbl_winnt_x
	StrCpy $R1 $R0 3
	StrCmp $R1 '5.0' lbl_winnt_2000
	StrCmp $R1 '5.1' lbl_winnt_XP
	StrCmp $R1 '5.2' lbl_winnt_2003
	StrCmp $R1 '6.0' lbl_winnt_vista
	StrCmp $R1 '6.1' lbl_winnt_7 lbl_error
 
lbl_winnt_x:
	StrCpy $R0 "NT $R0" 6
	MessageBox MB_OK|MB_ICONQUESTION "Windows NT4 is not supported by eid-mw" IDOK 
	Quit
	Goto lbl_done
 
lbl_winnt_2000:
	Strcpy $R0 '2000'
	MessageBox MB_OK|MB_ICONQUESTION "Windows 2000 is not supported by eid-mw" IDOK 
	Quit
    Goto lbl_done
 
lbl_winnt_XP:
	Strcpy $R0 'XP'
	MessageBox MB_OK|MB_ICONQUESTION "Windows XP (-minidriver +CSP)" IDOK 
	SectionSetFlags ${SecMiniDriver} ${SF_RO}
	SectionSetFlags ${SecCSPAndPropagationService} ${SF_SELECTED}
    Goto lbl_done
 
lbl_winnt_2003:
	Strcpy $R0 '2003'
	MessageBox MB_OK|MB_ICONQUESTION "Windows 2003 (-minidriver +CSP)" IDOK 
	SectionSetFlags ${SecMiniDriver} ${SF_RO}
	SectionSetFlags ${SecCSPAndPropagationService} ${SF_SELECTED}
    Goto lbl_done
 
lbl_winnt_vista:
	Strcpy $R0 'Vista'
	MessageBox MB_OK|MB_ICONQUESTION "Windows Vista (+minidriver -CSP)" IDOK 
	SectionSetFlags ${SecMiniDriver} ${SF_SELECTED}
	SectionSetFlags ${SecCSPAndPropagationService} ${SF_RO}
    Goto lbl_done
 
lbl_winnt_7:
	Strcpy $R0 '7'
	MessageBox MB_OK|MB_ICONQUESTION "Windows 7 (+minidriver -CSP)" IDOK 
	SectionSetFlags ${SecMiniDriver} ${SF_SELECTED}
	SectionSetFlags ${SecCSPAndPropagationService} ${SF_RO}
	Goto lbl_done
 
	lbl_error:
	Strcpy $R0 ''
	lbl_done:
	Pop $R1
	Exch $R0
 
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
