echo %0 %1

if %1%=="" goto END

set LOCAL_FILE=%~1
set TARGET_MACHINE=\\10.2.250.42
set TARGET_DIR=%TARGET_MACHINE%\eidmwbuilds

::------------------------------------
:: get only the filename from the variable
::------------------------------------
set FILENAME=%~nx1  
echo [INFO] File=%FILENAME%

::------------------------------------
:: check if we can copy the file to the network
::------------------------------------
if exist "%TARGET_DIR%\%FILENAME%" goto NOCOPY
echo [INFO] Copying file %LOCAL_FILE% to %TARGET_DIR%\%FILENAME%
copy /Y "%LOCAL_FILE%" "%TARGET_DIR%\%FILENAME%"
goto END

:NOCOPY
echo [WARN] File '%LOCAL_FILE%' NOT copied. The file '%TARGET_DIR%\%FILENAME%' already exists.
goto END


:END