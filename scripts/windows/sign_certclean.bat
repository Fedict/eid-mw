:: set all path variables
:: ======================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
@call "%~dp0.\set_eidmw_version.cmd"

:: create the CertClean tool
:: =========================
@echo [INFO] Building CertClean Win32"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Release /Property:Platform=x86 "%~dp0..\..\plugins_tools\CertClean\CertClean.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Release /Property:Platform=x86 "%~dp0..\..\plugins_tools\CertClean\CertClean.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

@echo [INFO] Building CertClean x64"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\plugins_tools\CertClean\CertClean.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /m  /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0..\..\plugins_tools\CertClean\CertClean.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed


:: sign the 32bit version of the CertClean tool
:: ============================================
@echo [INFO] sign the CertClean tool
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "06f01865ee31c88ef2bc9d6f4b3eff06427d1ea7" /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 /v "%~dp0..\..\plugins_tools\CertClean\Release\CertClean.exe"

@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy the signed CertClean tool
copy %~dp0..\..\plugins_tools\CertClean\Release\CertClean.exe %~dp0

goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
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

@echo [INFO] Build CertClean Done...
@goto end

:err
@exit /b 1

:end

