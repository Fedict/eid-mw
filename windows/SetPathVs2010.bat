@echo [INFO] Define default value for BEID_DIR_VS_2010 if not defined yet
@echo [INFO] Input BEID_DIR_VS_2010=%BEID_DIR_VS_2010%
@set FILE_TO_FIND="Common7\IDE\VCExpress.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_VS_2010%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_vs2010
@echo        Not found in "%BEID_DIR_VS_2010%"

@set BEID_DIR_VS_2010=C:\Program Files\Microsoft Visual Studio 10.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_VS_2010%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_vs2010
@echo        Not found in "%BEID_DIR_VS_2010%"

@set BEID_DIR_VS_2010=C:\Program Files (x86)\Microsoft Visual Studio 10.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_VS_2010%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_vs2010
@echo        Not found in "%BEID_DIR_VS_2010%"

@echo [ERROR] Visual Studio 2010 could not be found
@echo         If the path is different from "C:\Program Files (x86)\Microsoft Visual Studio 10.0"
@echo         and from "C:\Program Files\Microsoft Visual Studio 10.0"
@echo         please define BEID_DIR_VS_2010 environment variable.
@exit /B 1

:find_vs2010
@echo        Found in "%BEID_DIR_VS_2010%"
@exit /B 0