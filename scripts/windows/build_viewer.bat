:: options for build file
::@set DONT_MERGE_VCRT=yes

:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: Create the version and revision number
:: ======================================
@call "%~dp0.\create_eidmw_version_files.cmd"


:: build viewer
:: ============
@echo [INFO] Building PKCS11_FF Win32"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=x86 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=x86 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

:: @echo [INFO] Building PKCS11_FF x64"
:: @"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\..\VS_2022\beid.sln"
:: @if "%ERRORLEVEL%" == "1" goto msbuild_failed
:: @"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\..\VS_2022\beid.sln"
:: @if "%ERRORLEVEL%" == "1" goto msbuild_failed


@echo [INFO] Building eIDViewer Win32"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Release /Property:Platform=x86 "%~dp0..\..\plugins_tools\eid-viewer\Windows\VS_2022\eIDViewer.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Release /Property:Platform=x86 "%~dp0..\..\plugins_tools\eid-viewer\Windows\VS_2022\eIDViewer.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

:: @echo [INFO] Building eIDViewer x64"
:: @"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\plugins_tools\eid-viewer\Windows\VS_2022\eIDViewer.sln"
:: @if "%ERRORLEVEL%" == "1" goto msbuild_failed
:: @"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\plugins_tools\eid-viewer\Windows\VS_2022\eIDViewer.sln"
:: @if "%ERRORLEVEL%" == "1" goto msbuild_failed


:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths

@cd %~dp0..\..\installers\eid-viewer\Windows

@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] sign 32 bit msi installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\eid-viewer\Windows\bin\beidViewer.msi"
@if %ERRORLEVEL%==1 goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-viewer\Windows\bin\beidViewer.msi %~dp0

:: @call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer64.cmd"
:: @if %ERRORLEVEL%==1 goto end_resetpath_with_error
:: @echo [INFO] sign 64 bit msi installer
:: "%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\eid-viewer\Windows\bin\x64\beidViewer64.msi"
:: @if %ERRORLEVEL%==1 goto signtool_failed
:: @echo [INFO] copy 64 bit msi installer
:: copy %~dp0..\..\installers\eid-viewer\Windows\bin\x64\beidViewer64.msi %~dp0

@cd "%OUR_CURRENT_PATH%"


:: create the NSIS installers
:: ==========================

@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerInstaller.nsi"
@if %ERRORLEVEL%==1 goto makensis_failed


:: create the NSIS launcher installer
:: ==================================
@echo [INFO] Make nsis viewer launcher installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerLauncher.nsi"
@if %ERRORLEVEL%==1 goto makensis_failed


:: sign the NSIS installers
:: ========================

@echo [INFO] Sign nsis viewer installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if %ERRORLEVEL%==1 goto signtool_failed

@echo [INFO] sign nsis viewer launcher
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Launcher %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed


:: copy the NSIS installer
:: =======================
@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0


:: copy the NSIS launcher installer
:: ================================
@echo [INFO] copy nsis launcher installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Launcher %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto err

:makensis_failed
@echo [ERR ] makensis failed
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

