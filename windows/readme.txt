Packages needed to create the eidmw for windows
-----------------------------------------------
1) Visual Studio 2010 Express Edition
For the compilation of the crypto libraries and the sccertprop tool
http://www.microsoft.com/express/Downloads/

2) Windows Driver Kit (version used is 7600.16385.1)
For the creation of the minidriver cat file (from the inf file)
For the test signing of the cat file
http://www.microsoft.com/whdc/devtools/wdk/wdkpkg.mspx

3) NSIS (version used is 2.46)
For the creation of the outlook supress names tool
http://nsis.sourceforge.net/Download

4) Wix (version used is Windows Installer XML Toolset 3.0.5419.0)
For the creation of the MSI installers

5) Windows platform SDK 2008 or Microsoft SDK v7.1
For the tools to create multi language MSI's

Code Signing Certificate for testing
------------------------------------
A test code signing certificate and private key is checked in. This certificate should be used 
for test signing only!


How to create a new code signing certificate/key pair (only written down here for the reference)

1) Make sure you have the tools makecert and pvk2pfx. These tools can be found int the Windows
   Driver Kit (eg in C:\WinDDK\7600.16385.1\bin\amd64\)

2) run:
   MakeCert.exe -r -pe -n "CN=Fedict eID(test)" -sv fedicteidtest.pvk fedicteidtest.cer
   Leave the pvk password blank

3) run:
   pvk2pfx.exe /pvk fedicteidtest.pvk /spc fedicteidtest.cer /pfx fedicteidtest.pfx


Install certificate as trusted root on test machine
---------------------------------------------------
Run commands:

certutil -addstore root fedicteidtest.cer
certutil -addstore TrustedPublisher fedicteidtest.cer