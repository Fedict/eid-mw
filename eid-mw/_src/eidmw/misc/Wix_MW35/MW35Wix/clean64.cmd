::echo %0 %1 %2
set CONFIGURATION=%1
set LANG=%2

set OUT_PATH=%~dp0bin\x64\%CONFIGURATION%
set OBJ_PATH=%~dp0obj\x64\%CONFIGURATION%

set FILE_TO_DELETE=BeidMW40-64-Basic*.msi
echo [INFO] Deleting %OUT_PATH%\%LANG%\%FILE_TO_DELETE%
del "%OUT_PATH%\%LANG%\%FILE_TO_DELETE%"

set FILE_TO_DELETE=Product64.wixobj
echo [INFO] Deleting %OBJ_PATH%\%FILE_TO_DELETE%
del "%OBJ_PATH%\%FILE_TO_DELETE%"

