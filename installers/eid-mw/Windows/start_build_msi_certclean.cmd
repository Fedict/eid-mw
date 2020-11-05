::Stand alone buildscript for certclean MSI installer

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
@call "%~dp0.\build_msi_certclean.cmd"
