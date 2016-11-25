:: set all path variables
:: ======================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
@call "%~dp0.\set_eidmw_version.cmd"


:: Create catalog
:: create the MSI installers
:: =========================
@del "%MDRVINSTALLPATH%\Release\beidmdrv.cat"
%INF2CAT_PATH%\inf2cat.exe /driver:%MDRVINSTALLPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver driver cat file
:: ===============================
set MDRVINSTALLPATH=%~dp0..\..\cardcomm\minidriver\makemsi
@echo MDRVINSTALLPATH = %MDRVINSTALLPATH% 
@echo [INFO] Sign the minidriver catalog
::"%SIGNTOOL_PATH%\signtool" sign /ac "%MDRVINSTALLPATH%\GlobalSign Root CA.crt" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%MDRVINSTALLPATH%\Release\beidmdrv.cat"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVINSTALLPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\Release\beidmdrv.cat"

@if "%ERRORLEVEL%" == "1" goto signtool_failed


:: Create catalog for no dialogs minidriver
:: ========================================
@del "%MDRVINSTALLPATH%\Release_nd\beidmdrv.cat"
%INF2CAT_PATH%\inf2cat.exe /driver:%MDRVINSTALLPATH%\Release_nd\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver no dialogs cat file
:: ===================================
set MDRVINSTALLPATH=%~dp0..\..\cardcomm\minidriver\makemsi
@echo MDRVINSTALLPATH = %MDRVINSTALLPATH% 
@echo [INFO] Sign the minidriver catalog
::"%SIGNTOOL_PATH%\signtool" sign /ac "%MDRVINSTALLPATH%\GlobalSign Root CA.crt" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%MDRVINSTALLPATH%\Release\beidmdrv.cat"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVINSTALLPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\Release_nd\beidmdrv.cat"

@if "%ERRORLEVEL%" == "1" goto signtool_failed

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
::"%SIGNTOOL_PATH%\signtool" sign /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto 
::sign the 64bit msi
@echo [INFO] sign 64 bit msi installer
::signtool fails at dual-signing msi's at the moment
::"%SIGNTOOL_PATH%\signtool" sign /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

@cd "%OUR_CURRENT_PATH%"


:: create the NSIS installer
:: =========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\Quickinstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath

:: sign the nsis installer
:: =======================
@echo [INFO] sign nsis installer
"%SIGNTOOL_PATH%\signtool" sign /n "FedICT" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
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

