::Stand alone buildscript for the 64bit MSI installer
::use eid-mw\scripts\windowsbuild_all.bat to build entire middleware

:: Check WiX
:: =========
@echo [INFO] Checking paths
@call "%~dp0..\..\..\scripts\windows\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end


:: Create the revision number
:: ==========================
@echo [INFO] Creating version numbers
@call "%~dp0..\..\..\scripts\windows\revision.cmd"


:: Create the MSI installer
:: ========================
@echo [INFO] Creating msi installer
@call "%~dp0.\build_msi_eidmwarm64.cmd"
