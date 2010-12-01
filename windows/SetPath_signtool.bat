@echo [INFO] Define default value for SIGNTOOL_PATH if not defined yet
@echo [INFO] Input SIGNTOOL_PATH=%SIGNTOOL_PATH%
@set FILE_TO_FIND="SignTool.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@set SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%SIGNTOOL_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_signtool
@echo        Not found in "%SIGNTOOL_PATH%"

@echo [ERROR] SIGNTOOL_PATH could not be found
@echo         If the path is different from "C:\WinDDK\7600.16385.1\bin\x86"
@echo         please define SIGNTOOL_PATH environment variable.
@exit /B 1

:find_signtool
@echo        Found in "%SIGNTOOL_PATH%"
@exit /B 0


