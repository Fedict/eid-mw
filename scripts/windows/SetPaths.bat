::Define search paths here
@set SEARCH_BEID_DIR_MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin
@set SEARCH_BEID_DIR_MSBUILD_2=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin
@set SEARCH_BEID_DIR_MSBUILD_3=C:\Program Files (x86)\MSBuild\14.0\Bin

::@set SEARCH_SIGNTOOL_PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin
@set SEARCH_SIGNTOOL_PATH=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\arm64
@set SEARCH_SIGNTOOL_PATH_2=C:\Program Files (x86)\Windows Kits\8.1\bin\x64
@set SEARCH_SIGNTOOL_PATH_3=C:\WinDDK\7600.16385.1\bin\x86
@set SEARCH_INF2CAT_PATH=C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86
@set SEARCH_INF2CAT_PATH_2=C:\WinDDK\7600.16385.1\bin\selfsign
@set SEARCH_BEID_DIR_PLATFORMSDK_1=C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86
@set SEARCH_BEID_DIR_PLATFORMSDK_2=C:\Program Files (x86)\Windows Kits\10\bin\x86
@set SEARCH_BEID_DIR_PLATFORMSDK_3=C:\Program Files\Microsoft Platform SDK\bin
@set SEARCH_BEID_DIR_PLATFORMSDK_4=C:\Program Files (x86)\Windows Kits\8.1\bin\x86
@set SEARCH_BEID_DIR_PLATFORMSDK_5=C:\Program Files (x86)\Windows Kits\8.0\bin\x86
@set SEARCH_BEID_DIR_PLATFORMSDK_6=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\bin


@set SEARCH_WIX_PATH=%USERPROFILE%\.dotnet\tools\.store\wix\5.0.2\wix\5.0.2\tools\net6.0\any
@set SEARCH_WIX_PATH_2=C:\Program Files (x86)\WiX Toolset v3.11\bin
@set SEARCH_NSIS_PATH=C:\Program Files (x86)\NSIS
@set SEARCH_NSIS_PATH_2=C:\Program Files\NSIS

@set BEID_SEARCH_PATHS_SET=1
::end of search paths




@echo [INFO] Define default value for BEID_DIR_MSBUILD if not defined yet
@echo [INFO] Input BEID_DIR_MSBUILD=%BEID_DIR_MSBUILD%
@set FILE_TO_FIND="msbuild.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

::do not try to re-use MSBUILD path (could be set to an older version)
::@set FILE_NOT_FOUND=
::@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
::@if "%FILE_NOT_FOUND%"=="" goto found_msbuild
::@echo        Not found in "%BEID_DIR_MSBUILD%"

@set BEID_DIR_MSBUILD=%SEARCH_BEID_DIR_MSBUILD%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_msbuild
@echo        Not found in "%BEID_DIR_MSBUILD%"

@set BEID_DIR_MSBUILD=%SEARCH_BEID_DIR_MSBUILD_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_msbuild
@echo        Not found in "%BEID_DIR_MSBUILD_2%"

@set BEID_DIR_MSBUILD=%SEARCH_BEID_DIR_MSBUILD_3%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSBUILD%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_msbuild
@echo        Not found in "%BEID_DIR_MSBUILD_3%"

@echo [ERROR] %FILE_TO_FIND% could not be found
@echo         If the path is different from %SEARCH_BEID_DIR_MSBUILD%
@echo         please define BEID_DIR_MSBUILD environment variable or adjust SetPaths.bat
@exit /B 1

:found_msbuild
@echo        Found in "%BEID_DIR_MSBUILD%"

@echo [INFO] Define default value for INF2CAT_PATH if not defined yet
@set FILE_TO_FIND="inf2cat.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@set INF2CAT_PATH=%SEARCH_INF2CAT_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@set INF2CAT_PATH=%SEARCH_INF2CAT_PATH_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@exit /B 1

:found_inf2cat
@echo        Found in "%INF2CAT_PATH%"

@echo [INFO] Define default value for BEID_DIR_PLATFORMSDK if not defined yet
@echo [INFO] Input BEID_DIR_PLATFORMSDK=%BEID_DIR_PLATFORMSDK%
@set FILE_TO_FIND="msitran.exe" "msidb.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_1%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_3%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@rem paths for Windows 8 SDK are slightly different
@set FILE_TO_FIND="msitran.exe" "msidb.exe"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_4%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_5%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%SEARCH_BEID_DIR_PLATFORMSDK_6%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@echo [ERROR] MS Platform SDK 2008, Windows SDK v7.1, or Windows SDK 8.0/8.1/10 could not be found
@echo         If the path is not any of the above,
@echo         please define BEID_DIR_PLATFORMSDK environment variable.
: don't care @exit /B 1

:found_mssdk
@echo        Found in "%BEID_DIR_PLATFORMSDK%"


@echo [INFO] Check if WiX is installed
@set FILE_TO_FIND="WixToolset.UI.wixext.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set BEID_DIR_WIX=%WIX%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_WIX%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_wix
@echo        Not found in "%BEID_DIR_WIX%"

@set BEID_DIR_WIX=%SEARCH_WIX_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_WIX%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_wix
@echo        Not found in "%BEID_DIR_WIX%"


@echo [ERROR] WiX could not be found
@echo         Please install Wix v5
@exit /B 1

:found_wix
@echo        Found in "%BEID_DIR_WIX%"


@echo [INFO] Define default value for SEARCH_NSIS_PATH if not defined yet
@echo [INFO] Input NSIS_PATH=%SEARCH_NSIS_PATH%
@set FILE_TO_FIND="makensis.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set NSIS_PATH=%SEARCH_NSIS_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%NSIS_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_nsis
@echo        Not found in "%NSIS_PATH%"

@set NSIS_PATH=%SEARCH_NSIS_PATH_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%NSIS_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_nsis
@echo        Not found in "%NSIS_PATH%"

@echo [ERROR] NSIS could not be found
@echo         Please install NSIS
: don't care@exit /B 1

:found_nsis
@echo        Found in "%NSIS_PATH%"


@echo [INFO] Check if signtool is installed
@set FILE_TO_FIND="signtool.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set SIGNTOOL_PATH=%SEARCH_SIGNTOOL_PATH%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@set SIGNTOOL_PATH=%SEARCH_SIGNTOOL_PATH_2%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@set SIGNTOOL_PATH=%SEARCH_SIGNTOOL_PATH_3%
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto found_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@echo [ERROR] Signtool could not be found
@echo         Please install Visual Studio or WINDDK
@exit /B 1

:found_signtool
@echo        Found in "%SEARCH_SIGNTOOL_PATH%"

@exit /B 0
