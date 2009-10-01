:: Required: 7zip installed. Copy C:\Program Files\7-Zip\7z.exe to C:\WINDOWS\system32 

set x=belgiumeid
xcopy %x% build /i /e

cd build
7z a -tzip "%x%.xpi" * -r -mx=9
cd ..

move build\%x%.xpi %x%.xpi

rmdir /s /q build
