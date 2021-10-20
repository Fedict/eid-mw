:: set all path variables
:: ======================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
@call "%~dp0.\set_eidmw_version.cmd"

:: create the certreg tool
:: =======================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

set MDRVCERTPATH=%~dp0..\..\cardcomm\minidriver\makemsi

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows
@if %ERRORLEVEL%==1 goto 
::sign the certreg tool
@echo [INFO] sign the certreg tool
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVCERTPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Zetes SA" /sha1 "e20634d42e8bc522c6341dce24badd103f5f4312" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\plugins_tools\certreg\Release\certreg.exe"

@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy the certreg tool
copy %~dp0..\..\plugins_tools\certreg\Release\certreg.exe %~dp0

@cd "%OUR_CURRENT_PATH%"

:: create the NSIS installer
:: =========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\CertRegInstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath

:: sign the nsis installer
:: =======================
@echo [INFO] sign nsis installer
%SIGNTOOL_PATH%\SignTool.exe sign /n /n "Zetes SA" /sha1 "e20634d42e8bc522c6341dce24badd103f5f4312" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\installers\quickinstaller\certRegInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\certRegInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
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

