:: set all path variables
:: ======================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
@call "%~dp0.\set_eidmw_version.cmd"

set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

set MDRVINSTALLPATH=%~dp0..\..\installers\quickinstaller\Drivers\WINALL
set MDRVCERTPATH=%~dp0..\..\cardcomm\minidriver\makemsi

:: Create catalog
:: create the MSI installers
:: =========================
@del "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
%INF2CAT_PATH%\inf2cat.exe /driver:%MDRVINSTALLPATH%\beidmdrv\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver driver dll's and cat file
:: =========================================

@echo MDRVINSTALLPATH = %MDRVINSTALLPATH% 
@echo [INFO] Sign the minidriver catalog
::"%SIGNTOOL_PATH%\signtool" sign /ac "%MDRVINSTALLPATH%\GlobalSign Root CA.crt" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVCERTPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
@echo [INFO] Sign the minidriver 32bit dll
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv32.dll"
@echo [INFO] Sign the minidriver 64bit dll
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv64.dll"

@if "%ERRORLEVEL%" == "1" goto signtool_failed


@cd "%MDRVINSTALLPATH%"
del /q beidmdrv.zip
:: zip the minidriver folder
powershell.exe -nologo -noprofile -command "Compress-Archive -Path .\beidmdrv\* -CompressionLevel Optimal -DestinationPath ./beidmdrv.zip"
@echo [INFO] Sign the zip file
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv.zip"

copy %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv.zip %~dp0

@cd "%OUR_CURRENT_PATH%"

:: sign pkcs11
:: ===========

@echo [INFO] Sign the pkcs11 dll, 32bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\cardcomm\VS_2015\Binaries\Win32_Release\beidpkcs11.dll"
@echo [INFO] Sign the pkcs11_ff dll, 32bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\cardcomm\VS_2015\Binaries\Win32_PKCS11_FF_Release\beid_ff_pkcs11.dll"
@echo [INFO] Sign the pkcs11 dll, 64bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\cardcomm\VS_2015\Binaries\x64_Release\beidpkcs11.dll"
@echo [INFO] Sign the pkcs11_ff dll, 64bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\cardcomm\VS_2015\Binaries\x64_PKCS11_FF_Release\beid_ff_pkcs11.dll"


:: create the MSI installers
:: =========================

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw32.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
::sign the 32bit msi
@echo [INFO] sign 32 bit msi installer
::signtool fails at dual-signing msi's at the moment
::"%SIGNTOOL_PATH%\signtool" sign /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto signtool_failed
::sign the 64bit msi
@echo [INFO] sign 64 bit msi installer
::signtool fails at dual-signing msi's at the moment
::"%SIGNTOOL_PATH%\signtool" sign /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
:: /as
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

::need current dir to be pointing at one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-viewer\Windows

@echo [INFO] Sign the eID Viewer executable
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewer.exe"
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewerBackend.dll"
@if %ERRORLEVEL%==1 goto signtool_failed

@echo [INFO] create eID Viewer msi installer
@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error

@echo [INFO] sign eID Viewer msi installer
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi"
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
@echo [INFO] sign nsis installer
"%SIGNTOOL_PATH%\signtool" sign /n "FedICT" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@echo [INFO] sign nsis viewer installer
"%SIGNTOOL_PATH%\signtool" sign /n "FedICT" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /t http://timestamp.verisign.com/scripts/timestamp.dll /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /s MY /n "Fedict" /sha1 "2259EF223A51E91964D7F4695706091194E018BB" /tr http://tsa.belgium.be/connect /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
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

