::@call "%~dp0set_path.bat"

:: Check Visual Studio 2010
:: ========================
@call "%~dp0.\SetPathVs2010.bat"
@if %ERRORLEVEL%==0 goto find_vs2010

@echo [TIP] Use SetPathVs2010.bat script to define BEID_DIR_VS_2010
@goto end

:find_vs2010
@echo [INFO] Using BEID_DIR_VS_2010=%BEID_DIR_VS_2010%

:: BUILD
:: =====

:: build pkcs11, minidriver, cleanuptool and sccertprop
:: ====================================================
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln"
:::::::@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\VS_2010\beid.sln" /clean "Release|Win32"
::@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /clean "Release|x64"
::::::::::::::@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\VS_2010\beid.sln" /build "Release|Win32"
::@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /build "Release|x64"


:: create minidriver driver installer
:: ==================================
@call "%~dp0.\build_minidriver_cat_file.bat"
@call "%~dp0.\testsign_minidriver_cat_file.bat"


:: create the MSI installers
:: =========================
@set OUR_CURRENT_PATH = "%~dp0"
@cd %~dp0..\installers
@if NOT "%DEBUG%"=="1" pause
@call "getsvnrevision.bat"
@cd %~dp0..\installers\eid-mw\Windows
@if NOT "%DEBUG%"=="1" pause
@call "candle_light.cmd"
@cd "%OUR_CURRENT_PATH%"


@echo [INFO] Done...
@goto end


:end
@if NOT "%DEBUG%"=="1" pause
