
using System;
using System.Runtime.InteropServices;

namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_DATE
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] year;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public byte[] month;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
		public byte[] day;
		
	}
}
