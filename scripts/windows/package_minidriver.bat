
del /q beidmdrv_all.zip
del /q %~dp0\beidmdr_all

:: copy the minidriver with the attestation signed .cat file to the WIN10 folder
robocopy %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv %~dp0\beidmdr_all\WIN10 beidmdrv32.dll beidmdrv64.dll beidmdrv.inf beidmdrv.cat
:: copy the minidriver without the attestation signed .cat file to the WIN7 and WIN8.1 folder
robocopy %~dp0..\..\installers\quickinstaller\Drivers\WINALL\beidmdrv %~dp0\beidmdr_all\WIN7_WIN81 beidmdrv32.dll beidmdrv64.dll beidmdrv.inf
:: copy the ZTS signed .cat file to the WIN7 and WIN8.1 folder
robocopy %~dp0..\..\installers\quickinstaller\Drivers\XP-WIN8\beidmdrv %~dp0\beidmdr_all\WIN7_WIN81 beidmdrv.cat

:: zip the minidrivers folders
powershell.exe -nologo -noprofile -command "Compress-Archive -Path .\beidmdr_all\* -CompressionLevel Optimal -DestinationPath ./beidmdrv_all.zip"


:end
@exit /B 0
