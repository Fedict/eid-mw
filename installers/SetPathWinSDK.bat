@echo [INFO] Define default value for BEID_DIR_PLATFORMSDK if not defined yet
@echo [INFO] Input BEID_DIR_PLATFORMSDK=%BEID_DIR_PLATFORMSDK%
@set FILE_TO_FIND="bin\msitran.exe" "bin\msidb.exe" "Include\newdev.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=%~dp0..\ThirdParty\MSPlatformSDK
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=C:\Program Files\Microsoft Platform SDK
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%BEID_DIR_PLATFORMSDK%"

@set BEID_DIR_PLATFORMSDK=C:\Program Files\Microsoft SDKs\Windows\v7.1
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
@exit /B 0