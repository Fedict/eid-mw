@call "%~dp0set_path.bat"

:: Check Visual Studio 2005
:: ========================
@call "%~dp0..\_src\eidmw\SetPathVs2005.bat"
@if %ERRORLEVEL%==0 goto find_vs2005

@echo [TIP] Use set_path.bat script to define BEID_DIR_VS_2005
@goto end

:find_vs2005
@echo [INFO] Using BEID_DIR_VS_2005=%BEID_DIR_VS_2005%

:: Check MS Platform SDK 2008
:: ==========================
@call "%~dp0..\_src\eidmw\SetPathPSdk2008.bat"
@if %ERRORLEVEL%==0 goto find_mssdk2008

@echo [TIP] Use set_path.bat script to define BEID_DIR_PLATFORMSDK_2008
@goto end

:find_mssdk2008
@echo [INFO] Using BEID_DIR_PLATFORMSDK_2008=%BEID_DIR_PLATFORMSDK_2008%


:: BUILD
:: =====

:build
@echo [INFO] cleaning "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln Release|Win32"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln" /clean "Release|Win32" /Out "%~dp0.\clean_limited_32.log"
@echo [INFO] cleaning "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln Release|x64"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln" /clean "Release|x64" /Out "%~dp0.\clean_limited_64.log"
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln Release|Win32"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln" /build "Release|Win32" /Out "%~dp0.\build_limited_32.log"
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln Release|x64"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidLimitedBuilds.sln" /build "Release|x64" /Out "%~dp0.\build_limited_64.log"

@echo [INFO] Done...
@goto end

:end
@pause
