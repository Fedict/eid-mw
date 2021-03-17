:: options for build file
::@set DONT_MERGE_VCRT=yes

:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: Create the version and revision number
:: ======================================
@call "%~dp0.\create_eidmw_version_files.cmd"


:: build pkcs11, minidriver and viewer
:: ===================================
@echo [INFO] Building "%~dp0..\..\VS_2015\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=Win32 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=Win32 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\..\VS_2015\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

:: create minidriver driver installer
:: ==================================


:: BuildPath
set MDRVINSTALLPATH=%~dp0..\..\installers\quickinstaller\Drivers\WINALL
@echo MDRVINSTALLPATH = %MDRVINSTALLPATH% 

rmdir /s /q %MDRVINSTALLPATH%\beidmdrv
mkdir %MDRVINSTALLPATH%\beidmdrv
@echo [INFO] Copying minidriver files..

:: copy inf files
copy %~dp0..\..\cardcomm\minidriver\makemsi\beidmdrv.inf %MDRVINSTALLPATH%\beidmdrv

:: copy drivers. We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
copy %~dp0..\..\cardcomm\VS_2015\Binaries\Win32_Release\beidmdrv32.dll %MDRVINSTALLPATH%\beidmdrv\beidmdrv32.dll
copy %~dp0..\..\cardcomm\VS_2015\Binaries\x64_Release\beidmdrv64.dll %MDRVINSTALLPATH%\beidmdrv\beidmdrv64.dll

:: copy icon
:: copy %~dp0..\..\cardcomm\minidriver\img\beid.ico %MDRVINSTALLPATH%\beidmdrv\

:: @echo [INFO] Creating cat file
:: Create catalog
"%INF2CAT_PATH%\inf2cat.exe" /driver:%MDRVINSTALLPATH%\beidmdrv\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

@if "%BUILD_ONLY%" == "1" goto end
:: sign minidriver driver cat file
:: ===============================
@echo [INFO] Sign the catalog
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

:: copy minidriver to quickinstaller's driver folder


:: create cert
:: %SIGNTOOL_PATH%\makecert -r -n CN="ZetesTestCert" -b 01/01/2015 -e 01/01/2020 -ss my -sky signature

:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw32.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] sign 32 bit msi installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if %ERRORLEVEL%==1 goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] sign 64 bit msi installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi"
@if %ERRORLEVEL%==1 goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

@cd "%OUR_CURRENT_PATH%"

@cd %~dp0..\..\installers\eid-viewer\Windows

@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] sign 32 bit msi installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if %ERRORLEVEL%==1 goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@cd "%OUR_CURRENT_PATH%"

:: create the NSIS plugins
:: =========================
@echo [INFO] Building "%~dp0..\..\installers\quickinstaller\NSIS_Plugins\VS_2012\beidplugins.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\installers\quickinstaller\NSIS_Plugins\VS_2015\beidplugins.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\installers\quickinstaller\NSIS_Plugins\VS_2015\beidplugins.sln"

:: copy the NSIS plugins to NSIS default plugin folder
:: nsis installer is updated to look into the build folder
:: =========================
::@echo [INFO] Copying beid NSIS plugin
::copy %~dp0..\..\installers\quickinstaller\NSIS_Plugins\beidread\Release\beid.dll "%NSIS_PATH%\Plugins"
::@echo [INFO] Copying driver_installer NSIS plugin
::copy %~dp0..\..\installers\quickinstaller\NSIS_Plugins\driver_installer\Release\driver_installer.dll "%NSIS_PATH%\Plugins"

:: create the NSIS installers
:: ==========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\Quickinstaller.nsi"
@if %ERRORLEVEL%==1 goto makensis_failed

@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerInstaller.nsi"
@if %ERRORLEVEL%==1 goto makensis_failed

:: sign the NSIS installers
:: =========================
@echo [INFO] Sign nsis installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if %ERRORLEVEL%==1 goto signtool_failed

@echo [INFO] Sign nsis viewer installer
"%SIGNTOOL_PATH%\signtool" sign /a /n "ZetesTestCert" /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if %ERRORLEVEL%==1 goto signtool_failed

:: copy the NSIS installers
:: =========================
@echo [INFO] copy nsis installers
copy "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
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

