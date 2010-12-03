
:: Check SignTool
:: ========================
@call "%~dp0.\SetPath_SignTool.bat"
@if %ERRORLEVEL%==0 goto find_signtool

@echo [TIP] Use SetPath_SignTool.bat script to define SIGNTOOL_PATH
@goto end

:find_signtool
@echo [INFO] Using SIGNTOOL_PATH=%SIGNTOOL_PATH%

:: BUILD
:: =====

set BUILDPATH=..\minidriver\makemsi

:: Certificate name and store
set CERTIFICATENAME=Fedict eID(test)
set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
@if exist "fedicteidtest.cer" goto cert_exist

@echo [INFO] Make cert
%SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" fedicteidtest.cer


:cert_exist


:: Sign the catalog
%SIGNTOOL_PATH%\SignTool.exe sign /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Release\beidmdrv.cat
