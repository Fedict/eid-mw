:: options for build file
::@set DONT_MERGE_VCRT=yes

:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: Create the version and revision number
:: ======================================
@call "%~dp0.\create_eidmw_version_files.cmd"


@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:clean /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\tests\units\windows\Pkcs11_tests.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0..\..\tests\units\windows\Pkcs11_tests.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

goto end_resetpath

:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:end_resetpath
@cd %OUR_CURRENT_PATH%

@echo [INFO] Build_all Done...
@goto end

:err
@exit /b 1

:end

