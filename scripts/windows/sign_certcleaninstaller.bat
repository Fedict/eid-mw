:: set all path variables
:: ======================
call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
call "%~dp0.\set_eidmw_version.cmd"

:: sign the 32bit version of the CertClean tool
:: ============================================
::@echo [INFO] sign the CertClean tool x86
::"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\plugins_tools\CertClean\Release\CertClean.exe"
::
::@if "%ERRORLEVEL%" == "1" goto signtool_failed
::@echo [INFO] copy the signed CertClean tool
::copy %~dp0..\..\plugins_tools\CertClean\Release\CertClean.exe %~dp0


:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_certclean.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error

::sign the 32bit msi
@echo [INFO] sign 32bit certclean msi installer

"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\installers\eid-mw\Windows\bin\certclean.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit certclean msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\certclean.msi %~dp0


@cd "%OUR_CURRENT_PATH%"

goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:minidriver_failed
@echo [ERR ] signing minidriver failed
@goto err

:minidriver_cabinet_failed
@echo [ERR ] signing minidriver cabinet failed
@goto err

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto err

:makecert_failed
@echo [ERR ] makecert failed
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

