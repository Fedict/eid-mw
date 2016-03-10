using System.Runtime.InteropServices;
namespace Net.Sf.Pkcs11.Wrapper
{

	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct CK_SLOT_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public byte[] slotDescription;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] manufacturerID;

        public uint flags;

        public CK_VERSION hardwareVersion;

        public CK_VERSION firmwareVersion;

    }
}
