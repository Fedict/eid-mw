@SET CORRECT_SHA256_HASH=fcf8b503d1660185be90630dfa1c5b7625bcb26dc7aa3d38be61de163d29f298
@SET BEIDMDRV_ZIP=beidmdrv_all_5.0.27.5677.zip
@echo hash verification current beidmdrv started 
@echo known hash is %CORRECT_SHA256_HASH%

:: downloading signed minidriver
::==============================
@echo downloading signed minidriver %BEIDMDRV_ZIP%
@call curl -O "https://dist.eid.belgium.be/releases/5.0.27/%BEIDMDRV_ZIP%"
@if %ERRORLEVEL%==1 goto download_failed

@FOR /F "tokens=1" %%F IN ('CertUtil.exe -hashfile %BEIDMDRV_ZIP% SHA256 ^| find /i /v "SHA256" ^| find /i /v "certutil"') DO (
	SET var=%%F
)

@echo The Calculated SHA256 hash is %var%

IF %var%==%CORRECT_SHA256_HASH% GOTO hash_ok
goto hash_not_ok

:hash_ok
@echo hash was calculated correctly

:: extract downloaded minidriver
::==============================

rd /s /q "%~dp0\beidmdrv_dist"
powershell Expand-archive %BEIDMDRV_ZIP% -DestinationPath .\beidmdrv_dist -Force
@if %ERRORLEVEL%==1 goto zip_extract_failed

:: copy WIN10 signed minidriver
::=============================

del /q "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv\*"
rd "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv"
mkdir "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv"

copy "%~dp0.\beidmdrv_dist\WIN10\*" "%~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv"

:: copy WIN7 / WIN8.1 signed minidriver
::=====================================
del /q "%~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv\*"
rd "%~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv"
mkdir "%~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv"

copy "%~dp0.\beidmdrv_dist\WIN7_WIN81\*" "%~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv"

goto end:


::ERRORS:

:hash_not_ok
@echo hash verification failed, deleting what we downloaded
del %BEIDMDRV_ZIP%
exit /b 1

:download failed
@echo download failed
exit /b 1

:zip_extract_failed
@echo failed extracting %BEIDMDRV_ZIP%
exit /b 1



:end 
::@exit 0
