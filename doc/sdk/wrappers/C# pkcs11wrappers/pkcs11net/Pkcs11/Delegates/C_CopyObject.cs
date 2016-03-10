
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Delegates
{
     [System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute(System.Runtime.InteropServices.CallingConvention.Cdecl)]
	internal delegate CKR C_CopyObject(
		uint hSession,
		uint hObject,
		CK_ATTRIBUTE[] hTemplate,
		uint ulCount,
		ref uint phNewObject
	);
}
