::echo off
:: Required: 7zip installed

set thecurrentdir=%cd%
set BUILD_DIR=_build
set SRC_DIR=src
set EXT_NAME_BASE=belgiumeid

cd %~dp0 

call .\SetPath7zip.bat

for /f %%a in (VERSION) do (
set EXT_NAME_VERSION=%%a
)
set EXT_NAME=%EXT_NAME_BASE%-%EXT_NAME_VERSION%.xpi
echo %EXT_NAME%

mkdir %BUILD_DIR%
xcopy %SRC_DIR% %BUILD_DIR% /i /e /exclude:exclude_list

:: create XPI
cd %~dp0\%BUILD_DIR%
echo %EXT_NAME%
"%BEID_DIR_7ZIP%\7z" a -tzip %EXT_NAME% * -r -mx=9
cd %~dp0 
:: copy XPI to builds dir
move %BUILD_DIR%\%EXT_NAME% %EXT_NAME%

:: clean up
rmdir /s /q %BUILD_DIR%

PAUSE

cd %thecurrentdir%
