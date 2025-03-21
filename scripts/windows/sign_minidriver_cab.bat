:: set all path variables
:: ======================
@if DEFINED BEID_SEARCH_PATHS_SET goto searchpaths_set
call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:searchpaths_set

set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::set MDRVINSTALLPATH=%~dp0..\..\installers\quickinstaller\Drivers\WINALL
set MDRVCERTPATH=%~dp0..\..\cardcomm\minidriver\makemsi

:: sign the minidriver 
:: ===================
@echo [INFO] Calling script to sign the minidriver files
call "%~dp0.\sign_minidriver.bat"
@if "%ERRORLEVEL%" == "1" goto sign_minidriver_failed

:: Create minidriver cabinet file
:: ==============================
@echo [INFO] Creating minidriver cabinet file
MakeCab /f ".\beidmdrv.dff"

:: Sign cab file
:: =============
@echo [INFO] Sign the minidriver cabinet file
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /ac "%MDRVCERTPATH%\MSCV-GlobalSign Root CA.cer" /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0\disk1\Beidmdrv.cab"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

@cd "%MDRVINSTALLPATH%"


@cd "%OUR_CURRENT_PATH%"
goto end

:sign_minidriver_failed
@echo [ERR ] sign_minidriver failed
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
