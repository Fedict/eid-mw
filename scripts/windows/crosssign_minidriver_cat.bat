:: set all path variables
:: ======================
@if DEFINED BEID_SEARCH_PATHS_SET goto searchpaths_set
call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:searchpaths_set
:: get eidmw version
:: =================
::@call "%~dp0.\set_eidmw_version.cmd"

set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

set MDRVINSTALLPATH=%~dp0..\..\installers\quickinstaller\Drivers\WINALL
set MDRVCERTPATH=%~dp0..\..\cardcomm\minidriver\makemsi

:: Sign catalog
:: ============
@echo [INFO] Sign the minidriver catalog
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVCERTPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@cd "%MDRVINSTALLPATH%"

del /q beidmdrv_signed.zip
:: zip the minidriver folder
powershell.exe -nologo -noprofile -command "Compress-Archive -Path .\beidmdrv\* -CompressionLevel Optimal -DestinationPath ./beidmdrv_signed.zip"
move "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv_signed.zip" "%~dp0"

@cd "%OUR_CURRENT_PATH%"
goto end


:signtool_failed
@echo [ERR ] signtool failed
@goto err

:paths_failed
@echo [ERR ] could not set paths
@goto err

:err
@exit /B 1

:end
@exit /B 0
