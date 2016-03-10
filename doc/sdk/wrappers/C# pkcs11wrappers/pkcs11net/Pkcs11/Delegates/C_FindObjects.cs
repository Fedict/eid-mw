
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Delegates
{
    [System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]
	internal delegate CKR C_FindObjects(
		uint hSession,
		uint[] phObject,
		uint ulMaxObjectCount,
		ref uint pulObjectCount
	);
}
