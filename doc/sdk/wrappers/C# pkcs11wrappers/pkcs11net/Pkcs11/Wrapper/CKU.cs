
using System;

namespace Net.Sf.Pkcs11.Wrapper
{	
	public enum CKU:uint
	{
		SO = PKCS11Constants.CKU_SO,
		USER = PKCS11Constants.CKU_USER,
		CONTEXT_SPECIFIC =PKCS11Constants.CKU_CONTEXT_SPECIFIC
	}
}
