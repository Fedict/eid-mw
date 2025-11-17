:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: Create the version and revision number
:: ======================================
@call "%~dp0.\create_eidmw_version_files.cmd"


:: build pkcs11, minidriver and viewer
:: ===================================
@echo [INFO] Building "%~dp0..\..\VS_2022\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Minidriver_Release /Property:Platform=x86 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Minidriver_Release /Property:Platform=x86 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Minidriver_Release /Property:Platform=x64 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Minidriver_Release /Property:Platform=x64 "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Minidriver_Release /Property:Platform=ARM64EC "%~dp0..\..\VS_2022\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Minidriver_Release /Property:Platform=ARM64EC "%~dp0..\..\VS_2022\beid.sln"
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

:: copy minidriver to quickinstaller's driver folder
:: We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
copy %~dp0..\..\cardcomm\minidriver\VS_2022\Binaries\Win32_Release\beidmdrv32.dll %MDRVINSTALLPATH%\beidmdrv\beidmdrv32.dll
copy %~dp0..\..\cardcomm\minidriver\VS_2022\Binaries\x64_Release\beidmdrv64.dll %MDRVINSTALLPATH%\beidmdrv\beidmdrv64.dll
copy %~dp0..\..\cardcomm\minidriver\VS_2022\Binaries\ARM64EC_Release\beidmdrv64.dll %MDRVINSTALLPATH%\beidmdrv\beidmdrv_arm64.dll

:: copy icon
:: copy %~dp0..\..\cardcomm\minidriver\img\beid.ico %MDRVINSTALLPATH%\beidmdrv\

:: @echo [INFO] Creating cat file
:: Create catalog
"%INF2CAT_PATH%\inf2cat.exe" /driver:%MDRVINSTALLPATH%\beidmdrv\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server10_ARM64,10_RS3_ARM64,10_RS4_ARM64,ServerRS5_ARM64,10_RS5_ARM64,10_19H1_ARM64,10_VB_ARM64,ServerFE_ARM64,10_CO_ARM64,10_NI_ARM64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver driver cat file
:: ===============================
@echo [INFO] Sign the catalog
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /a /n "ZetesTestCert" /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
@if "%ERRORLEVEL%" == "1" goto signtool_failed


goto end


:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto err

:signtool_failed
@echo [ERR ] signtool failed
@goto err

:paths_failed
@echo [ERR ] could not set patsh
@goto err

:err
@exit /b 1

:end
