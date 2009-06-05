REM  This script is called as a post-build step by eidlib.vcproj
REM  to copy things to _binaries\release\ and _binaries\debug\
REM  (only if they weren't copied yet)

@echo off

echo Copying 3rd party libs etc. (if not yet copied)

if exist "%BEID_DIR_OPENSSL_098G%" goto find_openssl
set BEID_DIR_OPENSSL_098G=%~dp0..\..\ThirdParty\openssl.0.9.8g
if exist "%BEID_DIR_OPENSSL_098G%" goto find_openssl
goto print_err_openssl

:find_openssl
set TARGET1=%BEID_DIR_OPENSSL_098G%\lib\libeay32_0_9_8g.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" _binaries\debug\
xcopy /D /Y "%TARGET1%" _binaries\release\
copy /Y "%TARGET1%" _binaries\debug\beidlibeay32.dll
copy /Y "%TARGET1%" _binaries\release\beidlibeay32.dll

set TARGET1=%BEID_DIR_OPENSSL_098G%\lib\ssleay32_0_9_8g.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" _binaries\debug\
xcopy /D /Y "%TARGET1%" _binaries\release\
copy /Y "%TARGET1%" _binaries\debug\beidssleay32.dll
copy /Y "%TARGET1%" _binaries\release\beidssleay32.dll


if exist "%BEID_DIR_QT_334%" goto find_qt334
set BEID_DIR_QT_334=%~dp0..\..\ThirdParty\Qt\3.3.4
if exist "%BEID_DIR_QT_334%" goto find_qt334
goto print_err_qt334

:find_qt334
set TARGET1=%BEID_DIR_QT_334%\bin\qt-mt334.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" _binaries\debug\
xcopy /D /Y "%TARGET1%" _binaries\release\

goto done

:print_err
echo ERROR: couldn't find file %TARGET1%
goto done

:print_err_openssl
echo ERROR: couldn't find folder %BEID_DIR_OPENSSL_098G%
goto done

:print_err_qt334
echo ERROR: couldn't find folder %BEID_DIR_QT_334%
goto done

:done

echo on
