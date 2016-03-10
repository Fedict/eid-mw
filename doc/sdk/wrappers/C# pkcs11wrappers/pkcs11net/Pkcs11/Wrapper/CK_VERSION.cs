using System.Runtime.InteropServices;

namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_VERSION
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
		public byte[] major;
		
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
		public byte[] minor;
	}
}
