sample requirements

In order to build the SDK C# samples, the following software needs to be installed:

1) development environment:
Microsoft Visual Studio 2010 express project files are present in the sdk.
You can download Microsoft Visual Studio 2010 express csharp edition from:
http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-csharp-express


2) C# crypto library:
The examples provided in the sdk, use bouncycastle as crypto library. (http://www.bouncycastle.org/)
The C# crypto library of bouncycastle is also included in the pkcs11net project (see 4)),
but if needed, you can find the latest version at: http://www.bouncycastle.org/csharp/index.html
(copy the BouncyCastle.Crypto.dll into the "CS\References" folder)


3) nunit (needed by Net.pkcs11)
you can download nunit from :
http://www.nunit.org/?p=download
After installing the msi, you can find the nunit.framework.dll and nunit.framework.xml at
C:\Program Files (x86)\NUnit 2.5.10\bin\net-2.0\framework
copy the nunit.framework.dll and nunit.framework.xml into the "CS\References" folder


4) C# pkcs11 wrapper 
you'll need to rebuild the pkcs11net, download the sources:
svn checkout https://pkcs11net.svn.sourceforge.net/svnroot/pkcs11net (homepage at http://sourceforge.net/projects/pkcs11net/)
Build the version from trunk (we tested with revision 76)

Adjust the links in the 'references' folder of the projects if needed (depending on where you placed bouncycastle, nunit and our sdk samples)

4.1) A change is needed to pkcs11net as our pkcs11 library uses the CDECL calling convention
add "[System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]"
before each delegate function. (all delegate functions are listed in a 'delegate' folder)
e.g.

[System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]
    internal delegate CKR C_CloseAllSessions(
        uint slotID
    );

4.2) A second change might also be needed to pkcs11net, as some of its pkcs11 structs do not have their alignment set to 1.
We (beidpkcs11.dll) package the pkcs11 structs with 1-byte alignment, but the pkcs11net wrapper uses the default.

How to change the alignment of the pkcs11net wrapper structs:
e.g. for the CK_ATTRIBUTE struct:
in CK_ATTRIBUTE.cs change [StructLayout(LayoutKind.Sequential,Charset.Unicode)] to
[StructLayout(LayoutKind.Sequential,Charset.Unicode,Pack=1)]


5) adjust our SDK sample, depending on what version of pkcs11net you are using
(Module.Initialize(); is now included in the constructor and Module.Finalize_() is replaced by Module.Dispose();)
copy the Net.Pkcs11.dll into the "CS\References" folder