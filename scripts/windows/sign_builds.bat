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


:sign_eidmw_failed
@echo [ERR ] error running script sign_eidmw.bat
@goto err

:sign_viewer_failed
@echo [ERR ] error running script sign_viewer.bat
@goto err


:err
@exit /b 1

:end
