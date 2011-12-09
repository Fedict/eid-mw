sample requierments

In order to build the SDK C# samples, following software needs to be installed:

1) development environment:
Microsoft Visual Studio 2010 express project files are present in the sdk.
You can download Microsoft Visual Studio 2010 express csharp edition from:
http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-csharp-express


2) C# crypto library:
The examples provided in the sdk, use bouncycastle as crypto library. (http://www.bouncycastle.org/)
The C# crypto library of bouncycastle can be found on : http://www.bouncycastle.org/csharp/index.html
copy the BouncyCastle.Crypto.dll into the "CS\References" folder


3) nunit (needed by Net.pkcs11)
you can download nunit from :
http://www.nunit.org/?p=download
After installing the msi, you can find the nunit.framework.dll and nunit.framework.xml at
C:\Program Files (x86)\NUnit 2.5.10\bin\net-2.0\framework
copy the nunit.framework.dll and nunit.framework.xml into the "CS\References" folder


4) C# pkcs11 wrapper 
you'll need to rebuild the pkcs11net, download the sources:
svn checkout https://pkcs11net.svn.sourceforge.net/svnroot/pkcs11net (homepage at http://sourceforge.net/projects/pkcs11net/)

Adjust the links in the 'references' folder of the projects if needed (depend on where you placed bouncycastle, nunit and our sdk samples)

Some change is needed to pkcs11 as our pkcs11 library uses the CDECL calling convention
add "[System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]"
before each delegate function. (all delegate functions are listed in a 'delegate' folder)
e.g.

[System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]
    internal delegate CKR C_CloseAllSessions(
        uint slotID
    );




adjust our SDK sample, depending on what version of pkcs11net you are using
(Module.Initialize(); is now included in the constructor and Module.Finalize_() is replaced by Module.Dispose();)
copy the Net.Pkcs11.dll into the "CS\References" folder
