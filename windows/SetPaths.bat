::Define search paths here
@set SEARCH_BEID_DIR_MSBUILD=C:\Windows\Microsoft.NET\Framework\v4.0.30319
@set SEARCH_SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86
@set SEARCH_INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
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
@exit /B 0