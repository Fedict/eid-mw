:: make sure paths and version files are created first,
:: either by eid-mw\scripts\windowsbuild_all.bat(build entire middleware) 
:: or by start_build_msi_eidmw64.cmd (create only the 64bit MSI installer)

:: because we use Visual studio merge modules, some warning messages (string overflow and duplicate sequence number) will be generated, see also
:: http://wixtoolset.org/documentation/manual/v3/howtos/redistributables_and_install_checks/install_vcredist.html
:: These warnings are expected and are due to how the Visual C++ merge modules were authored.

:: create the language specific msi's
:: ==================================

@echo build_msi_eidmwarm64.cmd
@echo [INFO] Making the nl-nl version

wix build -arch arm64 -culture nl-nl -d Lang=nl -ext "%BEID_DIR_WIX%\WixToolset.UI.wixext.dll" -ext "%BEID_DIR_WIX%\WixToolset.Util.wixext.dll" -loc "%~dp0\Strings-nl-arm64.wxl" -loc WixUI_nl-nl_arm64.wxl -out bin\nl-nl\BeidMWarm64-Basic-nl.msi -pdb bin\nl-nl\BeidMWarm64-Basic-nl.wixpdb "%~dp0\Productarm64.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed

@echo [INFO] Making the fr-fr version

wix build -arch arm64 -culture fr-fr -d Lang=fr -ext "%BEID_DIR_WIX%\WixToolset.UI.wixext.dll" -ext "%BEID_DIR_WIX%\WixToolset.Util.wixext.dll" -loc "%~dp0\Strings-fr-arm64.wxl" -out bin\fr-fr\BeidMWarm64-Basic-fr.msi -pdb bin\fr-fr\BeidMWarm64-Basic-fr.wixpdb "%~dp0\Productarm64.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


@echo [INFO] Making the de-de version

wix build -arch arm64 -culture de-de -d Lang=de -ext "%BEID_DIR_WIX%\WixToolset.UI.wixext.dll" -ext "%BEID_DIR_WIX%\WixToolset.Util.wixext.dll" -loc "%~dp0\Strings-de-arm64.wxl" -out bin\de-de\BeidMWarm64-Basic-de.msi -pdb bin\de-de\BeidMWarm64-Basic-de.wixpdb "%~dp0\Productarm64.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


@echo [INFO] Making the en-us version

wix build -arch arm64 -culture en-us -d Lang=en -ext "%BEID_DIR_WIX%\WixToolset.UI.wixext.dll" -ext "%BEID_DIR_WIX%\WixToolset.Util.wixext.dll" -loc "%~dp0\Strings-en-arm64.wxl" -out bin\en-us\BeidMWarm64-Basic-en.msi -pdb bin\en-us\BeidMWarm64-Basic-en.wixpdb "%~dp0\Productarm64.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


:: join the language specific msi's together
:: =========================================

set OUT_PATH=%~dp0bin\
echo [INFO] OUT_PATH=%OUT_PATH%

set MSI_FILE_IN=BeidMWarm64-Basic
set MSI_FILE_OUT=BeidMW_arm64

::------------------------------------
:: check if all the MSI files were built
::------------------------------------
set LANG=en-us
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi" goto NOT_%LANG%

set LANG=nl-nl
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi" goto NOT_%LANG%

set LANG=fr-fr
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi" goto NOT_%LANG%

set LANG=de-de
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi" goto NOT_%LANG%

::------------------------------------
:: take all the MSI files and process
::------------------------------------

set LANG=en-us
copy /Y "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi" "%OUT_PATH%\%MSI_FILE_OUT%.msi"
set LANG=nl-nl
"%BEID_DIR_PLATFORMSDK%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi" "2067"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed
echo ...
set LANG=fr-fr
"%BEID_DIR_PLATFORMSDK%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi" "2060"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed
echo ...
set LANG=de-de
"%BEID_DIR_PLATFORMSDK%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi" "1031"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed
echo ...

"%BEID_DIR_PLATFORMSDK%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "2067"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed
"%BEID_DIR_PLATFORMSDK%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "2060"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed
"%BEID_DIR_PLATFORMSDK%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "1031"
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed

::------------------------------------
:: add all available LCIDs
::------------------------------------
"%BEID_DIR_PLATFORMSDK%\MsiInfo.Exe" "%OUT_PATH%\%MSI_FILE_OUT%.msi" /p Arm64;1033,2067,2060,1031
@if NOT "%ERRORLEVEL%" == "0" goto comb_msi_failed

goto END

:msi_failed
echo [ERR ] failed to create the MSI
exit /B 1

:comb_msi_failed
echo [ERR ] failed to combine the language specific MSI's
exit /B 1

:NOT_en-us
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi'
exit /B 1

:NOT_nl-nl
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi'
exit /B 1

:NOT_fr-fr
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi'
exit /B 1

:NOT_de-de
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi'
exit /B 1

echo done creating 64 bit installers
:END
echo end
