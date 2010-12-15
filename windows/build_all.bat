:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end

:: BUILD
:: =====

:: build pkcs11, minidriver, cleanuptool and sccertprop
:: ====================================================
@echo [INFO] Building "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0..\VS_2010\beid.sln"


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



:: sign minidriver driver cat file
:: ===============================

:: Certificate name and store
set CERTIFICATENAME=Fedict eID(test)
set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
@if exist "%~dp0fedicteidtest.cer" goto cert_exist

@echo [INFO] Make cert
%SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" %~dp0fedicteidtest.cer

:cert_exist

:: Sign the catalog
@echo [INFO] Sign the catalog
%SIGNTOOL_PATH%\SignTool.exe sign /a /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %INSTALLPATH%\Release\beidmdrv.cat



:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 
@cd %~dp0..\installers

@call "%~dp0..\installers\getsvnrevision.bat"
@cd %~dp0..\installers\eid-mw\Windows

@call "%~dp0..\installers\eid-mw\Windows\candle_light.cmd"
@call "%~dp0..\installers\eid-mw\Windows\candle_light64.cmd"

@cd %OUR_CURRENT_PATH%


@echo [INFO] Build_all Done...
@goto end


:end

