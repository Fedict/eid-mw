#define USE_GETFUNCTIONLIST

using System;
using System.Runtime.InteropServices;
using Net.Sf.Pkcs11.Delegates;

namespace Net.Sf.Pkcs11.Wrapper
{
	/// <summary>
	/// Description of DelegateUtil.
	/// </summary>
	public static class DelegateUtil
	{
        public static CK_FUNCTION_LIST GeFunctionList(IntPtr hLib)
        {
            IntPtr flpp = KernelUtil.GetProcAddress(hLib, "C_GetFunctionList");
            if (flpp == IntPtr.Zero) { throw new Exception("C_GetFunctionList could not be found"); }

            C_GetFunctionList GetFunctionList = (C_GetFunctionList)Marshal.GetDelegateForFunctionPointer(flpp, typeof(C_GetFunctionList));

            IntPtr flp;
            GetFunctionList(out flp);
            if (flp == IntPtr.Zero) { throw new Exception("C_GetFunctionList return invalid pointer."); }
            CK_FUNCTION_LIST flist;
            flist = (CK_FUNCTION_LIST)Marshal.PtrToStructure(flp, typeof(CK_FUNCTION_LIST));
            return flist;
        }

		public static Delegate GetDelegate(IntPtr hLib, Type delegateType){
			
			#if USE_GETFUNCTIONLIST
            CK_FUNCTION_LIST flist = GeFunctionList(hLib);
			IntPtr fp = (IntPtr)flist.GetType().GetField(delegateType.Name).GetValue(flist);
			if (fp == IntPtr.Zero) { throw new Exception("Bad pointer to "+delegateType.Name + " function."); }

			#else
			IntPtr fp = KernelUtil.GetProcAddress(hLib, delegateType.Name);
			if (fp == IntPtr.Zero) { throw new Exception(delegateType.Name + " could not be found"); }
			#endif
			
			return Marshal.GetDelegateForFunctionPointer(fp, delegateType);

			
		}
	}
}
