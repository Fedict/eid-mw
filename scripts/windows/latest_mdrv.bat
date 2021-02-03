@SET CORRECT_SHA256_HASH=903bf4a6ef6e2ece1783ed5146340e2d2d12e130abb8f0f14974a5a5098dd5f8
@echo hash verification current beidmdrv started 
@echo known hash is %CORRECT_SHA256_HASH%

@echo downloading latest minidriver
@call curl -O https://dist.eid.belgium.be/releases/506/beidmdrv_all.zip
@if %ERRORLEVEL%==1 goto download_failed

@FOR /F "tokens=1" %%F IN ('CertUtil.exe -hashfile beidmdrv_all.zip SHA256 ^| find /i /v "SHA256" ^| find /i /v "certutil"') DO (
	SET var=%%F
)

@echo The Calculated SHA256 hash is %var%

IF %var%==%CORRECT_SHA256_HASH% GOTO hash_ok

@echo hash verification failed
exit /b 1

:download failed
@echo download failed
exit /b 1

:hash_ok
@echo hash was calculated correctly

:end 
::@exit 0
