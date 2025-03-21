:: set all path variables
:: ======================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: get eidmw version
:: =================
@call "%~dp0.\set_eidmw_version.cmd"

set OUR_CURRENT_PATH="%cd%"
@echo OUR_CURRENT_PATH = %OUR_CURRENT_PATH% 

set MDRVINSTALLPATH=%~dp0..\..\installers\quickinstaller\Drivers\WINALL
set MDRVCERTPATH=%~dp0..\..\cardcomm\minidriver\makemsi


:: sign pkcs11
:: ===========

@echo [INFO] Sign the pkcs11_ff dll, 32bit
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2019\Binaries\Win32_PKCS11_FF_Release\beid_ff_pkcs11.dll"
:: @echo [INFO] Sign the pkcs11_ff dll, 64bit
:: "%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\cardcomm\pkcs11\VS_2019\Binaries\x64_PKCS11_FF_Release\beid_ff_pkcs11.dll"


:: create the MSI installers
:: =========================

::need current dir to be pointing at one of the wxs files, or light.exe can't find the paths
@cd %~dp0..\..\installers\eid-viewer\Windows

@echo [INFO] Sign the eID Viewer executable and the viewer backend dll x86
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\Release\eIDViewer.exe"
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewerBackend\VS_2019\Binaries\Win32_Release\eIDViewerBackend.dll"
@if %ERRORLEVEL%==1 goto signtool_failed

@echo [INFO] create eID Viewer msi installer x86
@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
@if %ERRORLEVEL%==1 goto end_resetpath_with_error

@echo [INFO] sign eID Viewer msi installer x86
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi"
@if "%ERRORLEVEL%" == "1" goto signtool_failed
@echo [INFO] copy 32 bit msi installer
@copy "%~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi" "%~dp0\BeidViewer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.msi"


::@echo [INFO] Sign the eID Viewer executable and the viewer backend dll x64
::"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewer\bin\x64\Release\eIDViewer.exe"
::"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\plugins_tools\eid-viewer\Windows\eIDViewerBackend\VS_2019\Binaries\x64_Release\eIDViewerBackend.dll"
::@if %ERRORLEVEL%==1 goto signtool_failed
::
::@echo [INFO] create eID Viewer msi installer x64
::@call "%~dp0..\..\installers\eid-viewer\Windows\build_msi_eidviewer.cmd"
::@if %ERRORLEVEL%==1 goto end_resetpath_with_error
::
::@echo [INFO] sign eID Viewer msi installer x64
::"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\installers\eid-viewer\Windows\bin\x64\BeidViewer64.msi"
::@if "%ERRORLEVEL%" == "1" goto signtool_failed
::@echo [INFO] copy 64 bit msi installer
::@copy "%~dp0..\..\installers\eid-viewer\Windows\bin\BeidViewer.msi" "%~dp0\BeidViewer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.msi"




@cd "%OUR_CURRENT_PATH%"


:: create the NSIS viewer installer
:: ================================
@echo [INFO] Make nsis viewer installer
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerInstaller.nsi"
@if "%ERRORLEVEL%" == "1" goto nsis_failed

:: create the NSIS viewer launcher
:: ===============================
@echo [INFO] Make nsis viewer launcher
"%NSIS_PATH%\makensis.exe" "%~dp0..\..\installers\quickinstaller\eIDViewerLauncher.nsi"
@if "%ERRORLEVEL%" == "1" goto nsis_failed



:: sign the nsis viewer installer
:: ==============================

@echo [INFO] sign nsis viewer installer by signing a SHA256 hash of it
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed

:: sign the nsis viewer launcher
:: =============================

@echo [INFO] sign nsis viewer launcher by signing a SHA256 hash of it
"%SIGNTOOL_PATH%\signtool" sign /fd SHA256 /s MY /n "Zetes SA" /sha1 "3f85e2a3538669c2a04aaeeb318497c780101872" /tr http://rfc3161timestamp.globalsign.com/advanced /td SHA256 /v "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Launcher %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe"
@if "%ERRORLEVEL%" == "1" goto signtool_failed


:: copy the deliverables
:: =====================
@echo [INFO] copy nsis installer
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Installer %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
copy "%~dp0..\..\installers\quickinstaller\Belgium eID Viewer Launcher %BASE_VERSION1%.%BASE_VERSION2%.%BASE_VERSION3%.%EIDMW_REVISION%.exe" %~dp0
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

:nsis_failed
@echo [ERR ] failed to create the NSIS installer
@goto err

:paths_failed
@echo [ERR ] could not set patsh
@goto err

:end_resetpath
@cd %OUR_CURRENT_PATH%

@echo [INFO] Sign_viewer Done...
@goto end

:err
@exit /b 1

:end
