using System.Runtime.InteropServices;

namespace Net.Sf.Pkcs11.Wrapper
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 1)]
	public struct CK_MECHANISM_INFO
	{
		public uint ulMinKeySize;
		
		public uint ulMaxKeySize;
		
		public uint flags;
		
	}
}
