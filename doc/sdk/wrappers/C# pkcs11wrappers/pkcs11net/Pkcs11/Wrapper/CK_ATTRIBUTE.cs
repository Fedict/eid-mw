
using System;
using System.Runtime.InteropServices;
namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_ATTRIBUTE{
		
		public uint type;

		public IntPtr pValue;

		public uint ulValueLen;
	}
}
