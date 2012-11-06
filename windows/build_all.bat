:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end

:: BUILD
:: =====

:: Create svn_revision.h
:: =====================
@call "%~dp0.\svn_revision.cmd"

:: build pkcs11, minidriver, cleanuptool and sccertprop
:: ====================================================
@echo [INFO] Building "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=PKCS11_FF_Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

:: create minidriver driver installer
:: ==================================

:: BuildPath
set INSTALLPATH=%~dp0..\minidriver\makemsi


md %INSTALLPATH%\Release
@echo [INFO] Copying minidriver files..

:: copy inf files
copy %INSTALLPATH%\beidmdrv.inf %INSTALLPATH%\Release

:: copy drivers. We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
copy %~dp0..\VS_2010\Binaries\Win32_Release\beidmdrv.dll %INSTALLPATH%\Release\beidmdrv32.dll
copy %~dp0..\VS_2010\Binaries\x64_Release\beidmdrv.dll %INSTALLPATH%\Release\beidmdrv64.dll

:: copy icon
copy %~dp0..\minidriver\img\beid.ico %INSTALLPATH%\Release\

@echo [INFO] Creating cat file
:: Create catalog
%INF2CAT_PATH%\inf2cat.exe /driver:%INSTALLPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver driver cat file
:: ===============================

:: Sign the catalog
@echo [INFO] Sign the catalog
%SIGNTOOL_PATH%\SignTool.exe sign /v /f %~dp0fedicteidtest.pfx /t http://timestamp.verisign.com/scripts/timestamp.dll %INSTALLPATH%\Release\beidmdrv.cat
@if "%ERRORLEVEL%" == "1" goto signtool_failed

:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

@cd %~dp0..\installers\eid-mw\Windows

@call "%~dp0..\installers\eid-mw\Windows\build_msi_eidmw32.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

:: create the NSIS plugins
:: =========================
@echo [INFO] Building "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\installers\quickinstaller\NSIS_Plugins\beidplugins.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\installers\quickinstaller\NSIS_Plugins\beidplugins.sln"

:: copy the NSIS plugins to NSIS default plugin folder
:: =========================
@echo [INFO] Copying beid NSIS plugin
copy %~dp0..\installers\quickinstaller\NSIS_Plugins\beidread\Release\beid.dll "%NSIS_PATH%\Plugins"
@echo [INFO] Copying driver_installer NSIS plugin
copy %~dp0..\installers\quickinstaller\NSIS_Plugins\driver_installer\Release\driver_installer.dll "%NSIS_PATH%\Plugins"

:: create the NSIS installer
:: =========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\installers\quickinstaller\Quickinstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath
@echo [INFO] copy nsis installer
copy %~dp0..\installers\quickinstaller\eID-QuickInstaller*.exe %~dp0
goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
@goto end

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto end

:makecert_failed
@echo [ERR ] makecert failed
@goto end

:signtool_failed
@echo [ERR ] signtool failed
@goto end

:end_resetpath_with_error
@echo [ERR ] failed to create the MSI installer

:end_resetpath
@cd %OUR_CURRENT_PATH%


@echo [INFO] Build_all Done...
@goto end


:end

