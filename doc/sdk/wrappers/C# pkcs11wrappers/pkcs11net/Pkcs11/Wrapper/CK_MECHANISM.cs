using System;
using System.Runtime.InteropServices;

namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_MECHANISM
	{
		public uint mechanism;
		
		public IntPtr pParameter;
		
		public uint ulParameterLen;
	}
}
