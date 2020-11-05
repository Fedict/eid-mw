:: make sure paths and version files are created first,
:: by start_build_msi_certclean.cmd (create only the cerclean (32-bit) MSI installer)


:: create the language specific msi's
:: ==================================

@echo [INFO] Making the nl-nl version

"%BEID_DIR_WIX%\candle.exe" -v -dLang=nl -out obj\CertClean.wixobj -arch x86 -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" "%~dp0\CertClean.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed
"%BEID_DIR_WIX%\Light.exe" -v -cultures:nl-nl -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" -loc "%~dp0\Strings-nl.wxl" -loc "%~dp0\WixUI_nl-nl.wxl" -out bin\nl-nl\CertClean-Basic-nl.msi -pdbout bin\nl-nl\CertClean-Basic-nl.wixpdb -sice:ICE09 obj\CertClean.wixobj"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


@echo [INFO] Making the fr-fr version

"%BEID_DIR_WIX%\candle.exe" -v -dLang=fr -out obj\CertClean.wixobj -arch x86 -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" "%~dp0\CertClean.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed
"%BEID_DIR_WIX%\Light.exe" -v -cultures:fr-fr -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" -loc "%~dp0\Strings-fr.wxl" -out bin\fr-fr\CertClean-Basic-fr.msi -pdbout bin\fr-fr\CertClean-Basic-fr.wixpdb -sice:ICE09 obj\CertClean.wixobj"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


@echo [INFO] Making the de-de version

"%BEID_DIR_WIX%\candle.exe" -v -dLang=de -out obj\CertClean.wixobj -arch x86 -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" "%~dp0\CertClean.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed
"%BEID_DIR_WIX%\Light.exe" -v -cultures:de-de -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" -loc "%~dp0\Strings-de.wxl" -out bin\de-de\CertClean-Basic-de.msi -pdbout bin\de-de\CertClean-Basic-de.wixpdb -sice:ICE09 obj\CertClean.wixobj"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed


@echo [INFO] Making the en-us version

"%BEID_DIR_WIX%\candle.exe" -v -dLang=en -out obj\CertClean.wixobj -arch x86 -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" "%~dp0\CertClean.wxs"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed
"%BEID_DIR_WIX%\Light.exe" -v -cultures:en-us -ext "%BEID_DIR_WIX%\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\WixUIExtension.dll" -loc "%~dp0\Strings-en.wxl" -out bin\en-us\CertClean-Basic-en.msi -pdbout bin\en-us\CertClean-Basic-en.wixpdb -sice:ICE09 obj\CertClean.wixobj"
@if NOT "%ERRORLEVEL%" == "0" goto msi_failed

:: join the language specific msi's together
:: =========================================

set OUT_PATH=%~dp0bin\
echo [INFO] OUT_PATH=%OUT_PATH%

set MSI_FILE_IN=CertClean-Basic
set MSI_FILE_OUT=CertClean

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
"%BEID_DIR_PLATFORMSDK%\MsiInfo.Exe" "%OUT_PATH%\%MSI_FILE_OUT%.msi" /p Intel;1033,2067,2060,1031
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

echo [INFO] Done creating multi-lang msi installers
:END
echo end