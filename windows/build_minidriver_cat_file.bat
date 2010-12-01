
:: Check inf2cat
:: ========================
@call "%~dp0.\SetPath_Inf_to_Cat.bat"
@if %ERRORLEVEL%==0 goto find_inf2cat

@echo [TIP] Use SetPath_Inf_to_Cat.bat script to define INF2CAT_PATH
@goto end

:find_inf2cat
@echo [INFO] Using INF2CAT_PATH=%INF2CAT_PATH%

:: BUILD
:: =====

:: Path to images
set IMG_PATH=..\minidriver\img
set BUILDPATH=..\minidriver\makemsi

md %BUILDPATH%\Release

:: copy inf files
copy %BUILDPATH%\beidmdrv.inf %BUILDPATH%\Release

:: copy drivers. We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
copy ..\VS_2010\Binaries\Win32_Release\beidmdrv.dll %BUILDPATH%\Release\beidmdrv32.dll
copy ..\VS_2010\Binaries\x64_Release\beidmdrv.dll %BUILDPATH%\Release\beidmdrv64.dll

:: copy icon
copy %IMG_PATH%\beid.ico %BUILDPATH%\Release\

:: Create catalog
%INF2CAT_PATH%\inf2cat.exe /driver:%BUILDPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64

@echo [INFO] Done...
@goto end


:end
@if NOT "%DEBUG%"=="1" pause
