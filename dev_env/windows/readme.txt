mingw64: Linking against windows native libraries
=================================================
The eid middleware links against these native windows libaries:
* winscard.dll
* crypt32.dll
* comctl32.dll v6 or higher
* ole32.dll

This is done by linking to an intermediate static library that comes with the mingw64 distribution.
Unfortunately, there are some issues:

[mingw64-32 and mingw64-64] all libraries
-----------------------------------------
For one reason or another, the gcc that comes with mingw on Windows searches for libraries in lib32/ or lib64/,
but most of the libraries in the mingw dist are located in lib/. So, we copy the libraries we need from:
 * 32-bit: i686-w64-mingw32/lib/ to i686-w64-mingw32/lib32/
 * 64-bit: x86_64-w64-mingw32/lib/ to x86_64-w64-mingw32/lib64/

Copy libole32.a, libcrypt32.a and libwinscard.a: 

   # 32-bit
   cp /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib/libole32.a      /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib32/libole32.a
   cp /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib/libcrypt32.a    /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib32/libcrypt32.a
   # for the reference, as libwinscard.a is not there for now 
   # cp /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib/libwinscard.a /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib32/libwinscard.a

   # 64-bit
   cp /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib/libole32.a    /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib64/libole32.a
   cp /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib/libcrypt32.a  /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib64/libcrypt32.a
   cp /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib/libwinscard.a /c/eid_dev_env/mingw64-32/x86_64-w64-mingw32/lib64/libwinscard.a     

[mingw64-32 and mingw64-64] libcomctl32.a v6
--------------------------------------------
We need libcomctl32.a v6. The version that comes with mingw64 is an older one, so we create the libcomctl32v6.a ourselves:

   i686-w64-mingw32-dlltool.exe --kill-at --output-lib libcomctl32v6.a --input-def imports/comctl32v6-32.def --dllname=comctl32.dll

Then copy it into the library path of your mingw dist. eg:

   cp libcomctl32v6.a /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib32/

The same procedure for 64-bit:
  
   x86_64-w64-mingw32-dlltool.exe --kill-at --output-lib libcomctl32v6.a --input-def imports/comctl32v6-64.def --dllname=comctl32.dll
   cp libcomctl32v6.a /c/eid_dev_env/mingw64-64/x86_64-w64-mingw32/lib64/

[mingw64-32] libwinscard.a
--------------------------
As libwinscard.a is not available in mingw64-32, we need to create it ourselves 

   i686-w64-mingw32-dlltool.exe --kill-at --output-lib libwinscard.a --input-def imports/winscard-32.def --dllname=winscard.dll

Then, copy it into the library path of your mingw dist. eg:
  
   cp libwinscard.a /c/eid_dev_env/mingw64-32/i686-w64-mingw32/lib32/

