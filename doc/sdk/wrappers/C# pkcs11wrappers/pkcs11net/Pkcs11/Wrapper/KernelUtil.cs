
using System;
using System.Runtime.InteropServices;
namespace Net.Sf.Pkcs11.Wrapper
{
	/// <summary>
	/// Description of LibraryManager.
	/// </summary>
	public static class KernelUtil 
	{

		#region KernelCalls
		[DllImport("kernel32")]
        public extern static IntPtr LoadLibrary(string lpLibFileName);
		
		[DllImport("kernel32")]
        public extern static bool FreeLibrary(IntPtr hLibModule);
		
		[DllImport("kernel32", CharSet = CharSet.Ansi)]
        public extern static IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
		#endregion
		
	}
}
