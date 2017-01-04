
using System;

namespace Net.Sf.Pkcs11.Wrapper
{

	public enum CKC:uint
	{
		X_509 = 0x00000000 ,
		X_509_ATTR_CERT =0x00000001,
		WTLS = 0x00000002 ,
		VENDOR_DEFINED = 0x80000000
	}
}
