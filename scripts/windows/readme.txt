Packages needed to create the eidmw for windows
-----------------------------------------------
1) Visual Studio 2022 Community Edition
For the compilation of the crypto libraries and the eID Viewer
Select desktop development, .Net desktop development and Windows 10 SDK (10.0.26100.0)
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
dotnet tool install --global wix --version 5.0.2
move to C:\Users\<YOU>\.dotnet\tools\.store\wix\5.0.2\wix\5.0.2\tools\net6.0\any
install UI toolset: wix extension add WixToolset.UI.wixext/5.0.2
install Util toolset: wix extension add WixToolset.Util.wixext/5.0.2

5) Git can be installed as part of Visual Studio installation
(or added later by updating the Visual Studio installation through add/remove applications)
Select "git" (and github extention if you like it integrated into VS) from the detailed installation list


Code Signing Certificate for testing
------------------------------------

How to create a new code signing certificate/key pair (only written down here for the reference)

1) Make sure you have the tools makecert and pvk2pfx. These tools can be found in the Windows
   Driver Kit (e.g. C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\)

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

Build and signing scripts:
--------------------------
build_all.bat : builds all (certclean, pkcs11, pkcs11_ff, minidriver, viewer, viewer launcher, msi's and NSIS installers) and signs all with test certificate
sign_minidriver_cab.bat : signs the minidriver, and creates and signs the minidriver cab file in preparation for attestation signing
package_minidriver.bat : creates a zip file that contains all attestation signed minidriver files
--latest_mdrv.bat-- : downloads and verifies the zip file created by package_minidriver.bat and places that minidriver in the folder to be included in the installers,
			this script needs to be updated when a new minidriver zip has been placed online, this script will be called from sign_builds.bat	
sign_builds.bat : sets the attestation signed minidriver that will be packaged, and signs all dll's and exe's and msi's (also rebuilds installers with signed components)

build_viewer.bat : builds all viewer components and installers and signs them with test certificate
sign_viewer.bat : signs all viewer related dll's and exe's and msi's (also rebuilds installers with signed components)
