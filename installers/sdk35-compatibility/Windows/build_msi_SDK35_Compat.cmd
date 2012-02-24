:: Check WiX
:: =========
@call "%~dp0..\..\..\windows\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end


:: Create the revision number
:: ==========================
set SVNREvision=
@call "%~dp0..\..\..\windows\svn_revision.cmd"

if not "%SVNRevision%"=="" goto skiptarfile
Set /P SVNRevision=< ..\..\..\svn_revision                                                      

:skiptarfile
Set /A SVNRevision+=6000

echo ^<Include^>>svn_revision.wxs
echo ^<?define RevisionNumber=%SVNRevision%?^>>>svn_revision.wxs
echo ^</Include^>>>svn_revision.wxs


:: create the language specific msi's
:: ==================================

@echo [INFO] Making the nl-nl version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=nl -out obj\SDK35_Compatibility.wixobj -arch x86 SDK35_Compatibility.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:nl-nl -loc Strings-nl.wxl -loc WixUI_nl-nl.wxl -out bin\nl-nl\SDK35_Compatibility-Basic-nl.msi -pdbout bin\nl-nl\SDK35_Compatibility-Basic-nl.wixpdb -sice:ICE09 obj\SDK35_Compatibility.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the fr-fr version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=fr -out obj\SDK35_Compatibility.wixobj -arch x86 SDK35_Compatibility.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:fr-fr -loc Strings-fr.wxl -out bin\fr-fr\SDK35_Compatibility-Basic-fr.msi -pdbout bin\fr-fr\SDK35_Compatibility-Basic-fr.wixpdb -sice:ICE09 obj\SDK35_Compatibility.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the de-de version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=de -out obj\SDK35_Compatibility.wixobj -arch x86 SDK35_Compatibility.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:de-de -loc Strings-de.wxl -out bin\de-de\SDK35_Compatibility-Basic-de.msi -pdbout bin\de-de\SDK35_Compatibility-Basic-de.wixpdb -sice:ICE09 obj\SDK35_Compatibility.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the en-us version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=en -out obj\SDK35_Compatibility.wixobj -arch x86 SDK35_Compatibility.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:en-us -loc Strings-en.wxl -out bin\en-us\SDK35_Compatibility-Basic-en.msi -pdbout bin\en-us\SDK35_Compatibility-Basic-en.wixpdb -sice:ICE09 obj\SDK35_Compatibility.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"


:: join the language specific msi's together
:: =========================================

set OUT_PATH=%~dp0bin\
echo [INFO] OUT_PATH=%OUT_PATH%

set MSI_FILE_IN=SDK35_Compatibility-Basic
set MSI_FILE_OUT=SDK35_Compatibility

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
"%BEID_DIR_PLATFORMSDK%\Bin\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi" "2067"
echo ...
set LANG=fr-fr
"%BEID_DIR_PLATFORMSDK%\Bin\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi" "2060"
echo ...
set LANG=de-de
"%BEID_DIR_PLATFORMSDK%\Bin\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi" "1031"
echo ...

"%BEID_DIR_PLATFORMSDK%\Bin\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "2067"
"%BEID_DIR_PLATFORMSDK%\Bin\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "2060"
"%BEID_DIR_PLATFORMSDK%\Bin\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "1031"

::------------------------------------
:: add all available LCIDs
::------------------------------------
"%BEID_DIR_PLATFORMSDK%\Bin\MsiInfo.Exe" "%OUT_PATH%\%MSI_FILE_OUT%.msi" /p Intel;1033,2067,2060,1031

goto END

:NOT_en-us
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi'
exit /B 1
goto END

:NOT_nl-nl
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi'
exit /B 1
goto END

:NOT_fr-fr
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi'
exit /B 1
goto END

:NOT_de-de
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi'
exit /B 1
goto END

echo [INFO] Done creating multi-lang msi installers
:END
echo end