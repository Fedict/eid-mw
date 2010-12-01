@echo [INFO] Define default value for INF2CAT_PATH if not defined yet
@echo [INFO] Input INF2CAT_PATH=%INF2CAT_PATH%
@set FILE_TO_FIND="inf2cat.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@set INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%INF2CAT_PATH%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_inf2cat
@echo        Not found in "%INF2CAT_PATH%"

@echo [ERROR] INF2CAT_PATH could not be found
@echo         If the path is different from "C:\WinDDK\7600.16385.1\bin\selfsign"
@echo         please define INF2CAT_PATH environment variable.
@exit /B 1

:find_inf2cat
@echo        Found in "%INF2CAT_PATH%"
@exit /B 0