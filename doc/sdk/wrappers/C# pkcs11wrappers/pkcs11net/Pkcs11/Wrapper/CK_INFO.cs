using System.Runtime.InteropServices;
namespace Net.Sf.Pkcs11.Wrapper
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	public struct CK_INFO
	{
        public CK_VERSION cryptokiVersion;
        
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] manufacturerID;

        public uint flags;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] libraryDescription;

        public CK_VERSION libraryVersion;
	}
}
