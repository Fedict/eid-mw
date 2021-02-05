@SET CORRECT_SHA256_HASH=903bf4a6ef6e2ece1783ed5146340e2d2d12e130abb8f0f14974a5a5098dd5f8
@echo hash verification current beidmdrv started 
@echo known hash is %CORRECT_SHA256_HASH%

:: downloading signed minidriver
::==============================
@echo downloading signed minidriver
@call curl -O https://dist.eid.belgium.be/releases/506/beidmdrv_all.zip
@if %ERRORLEVEL%==1 goto download_failed

@FOR /F "tokens=1" %%F IN ('CertUtil.exe -hashfile beidmdrv_all.zip SHA256 ^| find /i /v "SHA256" ^| find /i /v "certutil"') DO (
	SET var=%%F
)

@echo The Calculated SHA256 hash is %var%

IF %var%==%CORRECT_SHA256_HASH% GOTO hash_ok
goto hash_not_ok

:hash_ok
@echo hash was calculated correctly

:: extract downloaded minidriver
::==============================

powershell Expand-archive beidmdrv_all.zip -DestinationPath .\beidmdrv_dist -Force
@if %ERRORLEVEL%==1 goto zip_extract_failed

:: copy WIN10 signed minidriver
::=============================

del %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv\* \q
rd %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv
mkdir %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv

copy %~dp0.\beidmdrv_dist\WIN10\* %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv

:: copy WIN7 / WIN8.1 signed minidriver
::=====================================
del %~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv\* \q
rd %~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv
mkdir %~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv

copy %~dp0.\beidmdrv_dist\WIN7_WIN81\* %~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv

goto end:


::ERRORS:

:hash_not_ok
@echo hash verification failed, deleting what we downloaded
del beidmdrv_all.zip
exit /b 1

:download failed
@echo download failed
exit /b 1

:zip_extract_failed
@echo failed extracting beidmdrv_all.zip
exit /b 1



:end 
::@exit 0
