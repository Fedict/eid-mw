::Stand alone buildscript for 32bit MSI installer
::use eid-mw\scripts\windowsbuild_all.bat to build entire middleware

:: Check paths
:: =========
@echo [INFO] Checking paths
@call "%~dp0..\..\..\scripts\windows\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end


:: Create the version and revision number
:: ======================================
@echo [INFO] Creating version numbers
@call "%~dp0..\..\..\scripts\windows\create_eidmw_version_files.cmd"


:: Create the MSI installer
:: ========================
@echo [INFO] Creating msi installer
@call "%~dp0.\build_msi_eidmw32.cmd"
