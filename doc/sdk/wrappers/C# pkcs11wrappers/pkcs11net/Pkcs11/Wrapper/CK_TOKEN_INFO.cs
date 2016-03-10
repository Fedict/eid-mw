using System.Runtime.InteropServices;
namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_TOKEN_INFO
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
		public byte[] label;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
		public byte[] manufacturerID;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
		public byte[] model;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
		public byte[] serialNumber;

		public uint flags;

		public uint ulMaxSessionCount;
		
		public uint ulSessionCount;
		
		public uint ulMaxRwSessionCount;
		
		public uint ulRwSessionCount;
		
		public uint ulMaxPinLen;
		
		public uint ulMinPinLen;
		
		public uint ulTotalPublicMemory;
		
		public uint ulFreePublicMemory;
		
		public uint ulTotalPrivateMemory;
		
		public uint ulFreePrivateMemory;
		
		public CK_VERSION hardwareVersion;
		
		public CK_VERSION firmwareVersion;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
		public byte[] utcTime;
		
	}
}
