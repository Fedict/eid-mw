@echo [INFO] Define default value for BEID_DIR_QT_334 if not defined yet
@echo [INFO] Input BEID_DIR_QT_334=%BEID_DIR_QT_334%
@set FILE_TO_FIND="bin\qt-mt334.dll" "lib\qt-mt334.lib" "bin\uic.exe" "bin\moc.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_QT_334%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt334
@echo        Not found in "%BEID_DIR_QT_334%"

@set BEID_DIR_QT_334=%~dp0..\..\..\ThirdParty\Qt\3.3.4
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_QT_334%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt334
@echo        Not found in "%BEID_DIR_QT_334%"

@set BEID_DIR_QT_334=C:\Qt\3.3.4
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_QT_334%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt334
@echo        Not found in "%BEID_DIR_QT_334%"

@echo [ERROR] Qt 3.3.4 could not be found
@echo         If the path is different from "C:\Qt\3.3.4" or "%~dp0..\..\..\ThirdParty\Qt\3.3.4"
@echo         please define BEID_DIR_QT_334 environment variable.
@exit /B 1

:find_qt334
@echo        Found in "%BEID_DIR_QT_334%"
@exit /B 0