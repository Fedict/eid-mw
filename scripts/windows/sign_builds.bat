:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: sign the minidriver
::%SIGNTOOL_PATH%\SignTool.exe sign

:: create the MSI installers
:: =========================
set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

::need current dir to be pointing at the one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-mw\Windows

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw32.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error
::sign the 32bit msi
@echo [INFO] sign 32 bit msi installer
%SIGNTOOL_PATH%\SignTool.exe sign /n "FedICT - BE0367302178" /t http://timestamp.verisign.com/scripts/timestamp.dlll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi %~dp0

@call "%~dp0..\..\installers\eid-mw\Windows\build_msi_eidmw64.cmd"
@if %ERRORLEVEL%==1 goto 
::sign the 64bit msi
@echo [INFO] sign 64 bit msi installer
%SIGNTOOL_PATH%\SignTool.exe sign /n "FedICT - BE0367302178" /t http://timestamp.verisign.com/scripts/timestamp.dlll /v "%~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_32.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 64 bit msi installer
copy %~dp0..\..\installers\eid-mw\Windows\bin\BeidMW_64.msi %~dp0

@cd "%OUR_CURRENT_PATH%"


:: create the NSIS installer
:: =========================
@echo [INFO] Make nsis installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\Quickinstaller.nsi"
@if %ERRORLEVEL%==1 goto end_resetpath

:: sign the nsis installer
:: =======================
@echo [INFO] sign 64 bit msi installer
%SIGNTOOL_PATH%\SignTool.exe sign /n "FedICT - BE0367302178" /t http://timestamp.verisign.com/scripts/timestamp.dlll /v "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID-QuickInstaller %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
goto end_resetpath


:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:inf2cat_failed_failed
@echo [ERR ] inf2cat_failed failed
@goto err

:makecert_failed
@echo [ERR ] makecert failed
@goto err

:signtool_failed
@echo [ERR ] signtool failed
@goto err

:end_resetpath_with_error
@echo [ERR ] failed to create the MSI installer
@goto err

:paths_failed
@echo [ERR ] could not set patsh
@goto err

:end_resetpath
@cd %OUR_CURRENT_PATH%

@echo [INFO] Build_all Done...
@goto end

:err
@exit /b 1

:end

