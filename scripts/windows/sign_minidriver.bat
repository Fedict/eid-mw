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

:: Create catalog
:: ==============
@del "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
"%INF2CAT_PATH%\inf2cat.exe" /driver:%MDRVINSTALLPATH%\beidmdrv\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
@if "%ERRORLEVEL%" == "1" goto inf2cat_failed

:: sign minidriver dll's
:: =====================
@echo MDRVINSTALLPATH = %MDRVINSTALLPATH% 
@echo [INFO] Sign the minidriver 32bit dll
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv32.dll"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@echo [INFO] Sign the minidriver 64bit dll
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv64.dll"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

:: Sign catalog
:: ============
@echo [INFO] Sign the minidriver catalog
"%SIGNTOOL_PATH%\signtool" sign /as /fd SHA256 /ac "%MDRVCERTPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%MDRVINSTALLPATH%\beidmdrv\beidmdrv.cat"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
copy "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv\beidmdrv.cat" "%~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv\"


@cd "%MDRVINSTALLPATH%"

del /q beidmdrv.zip
:: zip the minidriver folder
powershell.exe -nologo -noprofile -command "Compress-Archive -Path .\beidmdrv\* -CompressionLevel Optimal -DestinationPath ./beidmdrv.zip"
move "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv.zip" "%~dp0"

@cd "%OUR_CURRENT_PATH%"
goto end

:inf2cat_failed
@echo [ERR ] inf2cat failed
@goto err

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
