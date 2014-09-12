::Define search paths here
@set SEARCH_BEID_DIR_MSBUILD=C:\Windows\Microsoft.NET\Framework\v4.0.30319
@set SEARCH_SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86
@set SEARCH_INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
@set SEARCH_BEID_DIR_PLATFORMSDK_1=%~dp0..\ThirdParty\MSPlatformSDK\bin
@set SEARCH_BEID_DIR_PLATFORMSDK_2=C:\Program Files\Microsoft Platform SDK\bin
@set SEARCH_BEID_DIR_PLATFORMSDK_3=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\bin
@set SEARCH_BEID_DIR_PLATFORMSDK_4=C:\Program Files (x86)\Windows Kits\8.0\bin\x86
@set SEARCH_WIX_PATH=C:\Program Files (x86)\WiX Toolset v3.8\bin
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

@echo [INFO] Define default value for INF2CAT_PATH if not defined yet
@echo [INFO] Input BEID_DIR_MSBUILD=%INF2CAT_PATH%
@set FILE_TO_FIND="inf2cat.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@set INF2CAT_PATH=%SEARCH_INF2CAT_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"
@exit /B 1

:find_inf2cat
@echo        Found in "%INF2CAT_PATH%"

@echo [INFO] Define default value for BEID_DIR_PLATFORMSDK if not defined yet
@echo [INFO] Input BEID_DIR_PLATFORMSDK=%BEID_DIR_PLATFORMSDK%
@set FILE_TO_FIND="msitran.exe" "msidb.exe" "..\Include\newdev.h"
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

@rem paths for Windows 8 SDK are slightly different
@set FILE_TO_FIND="msitran.exe" "msidb.exe" "..\..\Include\um\newdev.h"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_4%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@echo [ERROR] MS Platform SDK 2008, Windows SDK v7.1, or Windows SDK 8.0 could not be found
@echo         If the path is not any of the above,
@echo         please define BEID_DIR_PLATFORMSDK environment variable.
@exit /B 1

:find_mssdk
@echo        Found in "%BEID_DIR_PLATFORMSDK%"


@echo [INFO] Check if WiX is installed
@set FILE_TO_FIND="candle.exe"
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

@exit /B 0
