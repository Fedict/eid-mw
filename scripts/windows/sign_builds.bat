:: install the attestation signed minidriver in the drivers folder
:: ===============================================================
call "%~dp0.\latest_mdrv.bat"
@if %ERRORLEVEL%==1 goto download_latest_mdrv_failed

:: sign the eid mw (but not the minidriver, that should be attestation signed already)
:: ===================================================================================
call "%~dp0.\sign_eidmw.bat"
@if %ERRORLEVEL%==1 goto sign_eidmw_failed

:: sign the eid viewers (and launchers)
:: ====================================
call "%~dp0.\sign_viewer.bat"
@if %ERRORLEVEL%==1 goto sign_viewer_failed

@echo [INFO] signing builds done
@goto end


:download_latest_mdrv_failed
@echo [ERR ] error running script latest_mdrv.bat
@goto err

:sign_eidmw_failed
@echo [ERR ] error running script sign_eidmw.bat
@goto err

:sign_viewer_failed
@echo [ERR ] error running script sign_viewer.bat
@goto err


:err
@exit /b 1

:end
