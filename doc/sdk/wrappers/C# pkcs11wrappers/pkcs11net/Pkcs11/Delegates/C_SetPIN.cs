
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Delegates
{
     [System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]
	internal delegate CKR C_SetPIN(
		uint hSession, 
		byte[] pOldPin, 
		uint ulOldLen,
		byte[] pNewPin, 
		uint ulNewLen
	);
}
