@if not "%WindowsSdkDir%" == ""  goto build

@call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"

set PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\Tools\10.0.26100.0\arm64"
set PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\arm64"

set PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\Tools\10.0.26100.0\x64"
set PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64"

set PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\build\10.0.26100.0\bin"


@set PATH=%WindowsSdkDir%bin\%WindowsSDKVersion%\x86;%PATH%

:build
nmake %1