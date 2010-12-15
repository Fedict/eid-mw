:: Check WiX
:: =========
@call "%~dp0..\..\SetPathWix.bat"
@if %ERRORLEVEL%==0 goto find_wix

@echo [TIP] Install ProjectAggregator2.msi and Wix3-3.0.4415.msi
@goto end

:find_wix
@echo [INFO] Using BEID_DIR_WIX=%BEID_DIR_WIX%


@echo [INFO] Making the nl-nl version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=nl -out obj\Product.wixobj -arch x86 -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" Product.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:nl-nl -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" -loc Strings-nl.wxl -loc WixUI_nl-nl.wxl -out bin\nl-nl\BeidMW32-Basic-nl.msi -pdbout bin\nl-nl\BeidMW32-Basic-nl.wixpdb -sice:ICE09 obj\Product.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the fr-fr version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=fr -out obj\Product.wixobj -arch x86 -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" Product.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:fr-fr -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" -loc Strings-fr.wxl -out bin\fr-fr\BeidMW32-Basic-fr.msi -pdbout bin\fr-fr\BeidMW32-Basic-fr.wixpdb -sice:ICE09 obj\Product.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the de-de version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=de -out obj\Product.wixobj -arch x86 -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" Product.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:de-de -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" -loc Strings-de.wxl -out bin\de-de\BeidMW32-Basic-de.msi -pdbout bin\de-de\BeidMW32-Basic-de.wixpdb -sice:ICE09 obj\Product.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@echo [INFO] Making the en-us version

"%BEID_DIR_WIX%\bin\candle.exe" -v -dLang=en -out obj\Product.wixobj -arch x86 -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" Product.wxs

"%BEID_DIR_WIX%\bin\Light.exe" -v -cultures:en-us -ext "%BEID_DIR_WIX%\bin\WixDifxAppExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUtilExtension.dll" -ext "%BEID_DIR_WIX%\bin\WixUIExtension.dll" -loc Strings-en.wxl -out bin\en-us\BeidMW32-Basic-en.msi -pdbout bin\en-us\BeidMW32-Basic-en.wixpdb -sice:ICE09 obj\Product.wixobj "%BEID_DIR_WIX%\bin\difxapp_x86.wixlib"



@call "%~dp0multi-lang.cmd"