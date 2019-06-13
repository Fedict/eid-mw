:: set all path variables
:: ======================
call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
call "%~dp0.\set_eidmw_version.cmd"


:: sign the minidriver 
:: ===================
:: Do not re-sign the minidriver, it should be signed already by running the sign_minidriver_cab.bat,
:: and by now replaced by the attestation signed minidriver (which we will package)


:: package the minidriver 
:: ======================
call "%~dp0.\package_minidriver.bat"


:: sign pkcs11
:: ===========
@echo [INFO] Sign the pkcs11 dll, 32bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2017\Binaries\Win32_Release\beidpkcs11.dll"
@echo [INFO] Sign the pkcs11_ff dll, 32bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2017\Binaries\Win32_PKCS11_FF_Release\beid_ff_pkcs11.dll"
@echo [INFO] Sign the pkcs11 dll, 64bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2017\Binaries\x64_Release\beidpkcs11.dll"
@echo [INFO] Sign the pkcs11_ff dll, 64bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2017\Binaries\x64_PKCS11_FF_Release\beid_ff_pkcs11.dll"


:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw32.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
::sign the 32bit msi
@echo [INFO] sign 32 bit msi installer
::signtool fails at dual-signing msi's at the moment
::"%SIGNTOOL_PATH%\signtool" sign /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto signtool_failed
::sign the 64bit msi
@echo [INFO] sign 64 bit msi installer
::signtool fails at dual-signing msi's at the moment
::"%SIGNTOOL_PATH%\signtool" sign /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

::need current dir to be pointing at one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-viewer\Windows

@echo [INFO] Sign the eID Viewer executable
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewer.exe"
@echo [INFO] Sign the eID Viewer backend library
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewerBackend.dll"
@if %ERRORLEVEL%==1 goto signtool_failed

@echo [INFO] create eID Viewer msi installer
@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error

@echo [INFO] sign eID Viewer msi installer
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi %~dp0

@cd "%OUR_CURRENT_PATH%"


:: create the NSIS installer
:: =========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\Quickinstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath

@echo [INFO] Make nsis viewer installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerInstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath

:: sign the nsis installer
:: =======================
@echo [INFO] sign nsis installer by signing a SHA1 hash of it
"%SIGNTOOL_PATH%\signtool" sign /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@echo [INFO] add a SHA256 signature to nsis installer
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@echo [INFO] sign nsis viewer installer by signing a SHA1 hash of it
"%SIGNTOOL_PATH%\signtool" sign /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@echo [INFO] add a SHA256 signature to nsis viewer installer
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:minidriver_failed
@echo [ERR ] signing minidriver failed
@goto err

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto err

:makecert_failed
@echo [ERR ] makecert failed
@goto err

:signtool_failed
@echo [ERR ] signtool failed
@goto err

:end_resetpath_with_error
@echo [ERR ] failed to create the MSI installer
@goto err

:paths_failed
@echo [ERR ] could not set patsh
@goto err

:end_resetpath
@cd %OUR_CURRENT_PATH%

@echo [INFO] Build_all Done...
@goto end

:err
@exit /b 1

:end

