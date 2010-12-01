
:: Check Visual Studio 2010
:: ========================
@call "%~dp0.\SetPathVs2010.bat"
@if %ERRORLEVEL%==0 goto find_vs2010

@echo [TIP] Use SetPathVs2010.bat script to define BEID_DIR_VS_2010
@goto end

:find_vs2010
@echo [INFO] Using BEID_DIR_VS_2010=%BEID_DIR_VS_2010%

:: BUILD
:: =====

:build
@echo [INFO] Building "%~dp0..\minidriver\VS_2010\beidmdrv.sln"
@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\minidriver\VS_2010\beidmdrv.sln" /clean "Release|Win32"
@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\minidriver\VS_2010\beidmdrv.sln" /clean "Release|x64"
@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\minidriver\VS_2010\beidmdrv.sln" /build "Release|Win32"
@"%BEID_DIR_VS_2010%\Common7\IDE\VCExpress.exe" "%~dp0..\minidriver\VS_2010\beidmdrv.sln" /build "Release|x64"

@echo [INFO] Done...
@goto end


:end
@if NOT "%DEBUG%"=="1" pause
