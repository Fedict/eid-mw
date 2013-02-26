::Define search paths here
@set SEARCH_BEID_DIR_MSBUILD=C:\Windows\Microsoft.NET\Framework\v4.0.30319
@set SEARCH_SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86
@set SEARCH_INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
@set SEARCH_BEID_DIR_PLATFORMSDK_1=%~dp0..\ThirdParty\MSPlatformSDK
@set SEARCH_BEID_DIR_PLATFORMSDK_2=C:\Program Files\Microsoft Platform SDK
@set SEARCH_BEID_DIR_PLATFORMSDK_3=C:\Program Files\Microsoft SDKs\Windows\v7.1
@set SEARCH_WIX_PATH=F:\Program Files\Windows Installer XML v3
@set SEARCH_NSIS_PATH=C:\Program Files (x86)\NSIS
@set SEARCH_NSIS_PATH_2=C:\Program Files\NSIS
::end of search paths




@echo [INFO] Define default value for BEID_DIR_MSBUILD if not defined yet
@echo [INFO] Input BEID_DIR_MSBUILD=%BEID_DIR_MSBUILD%
@set FILE_TO_FIND="msbuild.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msbuild
@echo        Not found in "%BEID_DIR_MSBUILD%"

@set BEID_DIR_MSBUILD=%SEARCH_BEID_DIR_MSBUILD%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msbuild
@echo        Not found in "%BEID_DIR_MSBUILD%"

@echo [ERROR] %FILE_TO_FIND% could not be found
@echo         If the path is different from %SEARCH_BEID_DIR_MSBUILD%
@echo         please define BEID_DIR_MSBUILD environment variable or adjust SetPaths.bat
@exit /B 1

:find_msbuild
@echo        Found in "%BEID_DIR_MSBUILD%"


@echo [INFO] Define default value for SIGNTOOL_PATH if not defined yet
@echo [INFO] Input SIGNTOOL_PATH=%SIGNTOOL_PATH%
@set FILE_TO_FIND="SignTool.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@set SIGNTOOL_PATH=%SEARCH_SIGNTOOL_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@echo [ERROR] %FILE_TO_FIND% could not be found
@echo         If the path is different from %SIGNTOOL_PATH%
@echo         please define SIGNTOOL_PATH environment variable or adjust SetPaths.bat
@exit /B 1

:find_signtool
@echo        Found in "%SIGNTOOL_PATH%"


@echo [INFO] Define default value for INF2CAT_PATH if not defined yet
@echo [INFO] Input INF2CAT_PATH=%INF2CAT_PATH%
@set FILE_TO_FIND="inf2cat.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@set INF2CAT_PATH=%SEARCH_INF2CAT_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@echo [ERROR] %FILE_TO_FIND% could not be found
@echo         If the path is different from %INF2CAT_PATH%
@echo         please define INF2CAT_PATH environment variable.
@exit /B 1

:find_inf2cat
@echo        Found in "%INF2CAT_PATH%"


@echo [INFO] Define default value for BEID_DIR_PLATFORMSDK if not defined yet
@echo [INFO] Input BEID_DIR_PLATFORMSDK=%BEID_DIR_PLATFORMSDK%
@set FILE_TO_FIND="bin\msitran.exe" "bin\msidb.exe" "Include\newdev.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_1%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_3%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@echo [ERROR] MS Platform SDK 2008 or Windows SDK v7.1 could not be found
@echo         If the path is different from "C:\Program Files\Microsoft SDKs\Windows\v7.1" or "C:\Program Files\Microsoft Platform SDK"
@echo         please define BEID_DIR_PLATFORMSDK environment variable.
@exit /B 1

:find_mssdk
@echo        Found in "%BEID_DIR_PLATFORMSDK%"


@echo [INFO] Check if WiX is installed
@set FILE_TO_FIND="bin\candle.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set BEID_DIR_WIX=%WIX%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_WIX%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_wix
@echo        Not found in "%BEID_DIR_WIX%"

@set BEID_DIR_WIX=%SEARCH_WIX_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_WIX%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_wix
@echo        Not found in "%BEID_DIR_WIX%"


@echo [ERROR] WiX could not be found (recommended version 3.0.4415)
@echo         Please install ProjectAggregator2.msi and Wix3-3.0.4415.msi.
@exit /B 1

:find_wix
@echo        Found in "%BEID_DIR_WIX%"

@echo [INFO] Define default value for NSIS_PATH if not defined yet
@echo [INFO] Input BEID_DIR_MSBUILD=%NSIS_PATH%
@set FILE_TO_FIND="makensis.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%NSIS_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_nsis
@echo        Not found in "%NSIS_PATH%"

@set NSIS_PATH=%SEARCH_NSIS_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%NSIS_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_nsis
@echo        Not found in "%NSIS_PATH%"

@set NSIS_PATH=%SEARCH_NSIS_PATH_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%NSIS_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_nsis
@echo        Not found in "%NSIS_PATH%"

@echo [ERROR] %FILE_TO_FIND% could not be found
@echo         If the path is different from %NSIS_PATH%
@echo         please define NSIS_PATH environment variable or adjust SetPaths.bat
@exit /B 1

:find_nsis
@echo        Found in "%NSIS_PATH%"
@exit /B 0