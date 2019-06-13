Packages needed to create the eidmw for windows
-----------------------------------------------
1) Visual Studio 2017 Community Edition
For the compilation of the crypto libraries and the eID Viewer
Select desktop development, .Net desktop development and Windows 10 SDK (10.0.17134.0)
https://visualstudio.microsoft.com/downloads/

2) Windows Driver Kit
For the creation of the minidriver cat file (from the inf file)
For the test signing of the cat file
https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk

3) NSIS (version used is 3.04)
For the creation of the quick installers
http://nsis.sourceforge.net/Download

4) Wix Toolset
For the creation of the MSI installers
http://wixtoolset.org/releases/
This one currently still need .NET 3.5 installed
To install .NET 3.5: press the windows keys and type "Windows Features",
then select .NET 3.5 from te WIndows Features dialog box

5) Git can be installed as part of Visual Studio installation
(or added later by updating the Visual Studio installation through add/remove applications)
Select "git" (and github extention if you like it integrated into VS) from the detailed installation list


Code Signing Certificate for testing
------------------------------------

How to create a new code signing certificate/key pair (only written down here for the reference)

1) Make sure you have the tools makecert and pvk2pfx. These tools can be found int the Windows
   Driver Kit (e.g. C:\Program Files (x86)\Windows Kits\10\bin\10.0.17134.0\x64\)

2) To make the keys and the certificate run:
   MakeCert.exe -r -pe -ss my -a sha256 -n "CN=ZetesTestCert" -sv zeteseidtest.pvk zeteseidtest.cer
   Leave the pvk password blank

3) To combine the keys and certificate in a single file run:
   pvk2pfx.exe /pvk zeteseidtest.pvk /spc zeteseidtest.cer /pfx zeteseidtest.pfx

4) double click the zeteseidtest.pfx file to install the certificate and its keys into the user's "my" store
   
Install certificate as trusted root on test machine to trust the signatures
---------------------------------------------------------------------------
Run commands:

certutil -addstore root zeteseidtest.cer
certutil -addstore TrustedPublisher zeteseidtest.cer

To test sign a document
-----------------------
