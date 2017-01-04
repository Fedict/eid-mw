
using System.Runtime.InteropServices;
using System;

namespace Net.Sf.Pkcs11.Wrapper
{
    /// <summary>
    /// Converted to class because it can be inherited.
    /// </summary>
    [StructLayout(LayoutKind.Sequential,Pack=1, CharSet = CharSet.Unicode)]
    public class CK_FUNCTION_LIST
    {
        [MarshalAs(UnmanagedType.U1)]
        public byte major;
        [MarshalAs(UnmanagedType.U1)]
        public byte minor;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Initialize;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Finalize;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetInfo;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetFunctionList;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetSlotList;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetSlotInfo;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetTokenInfo;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetMechanismList;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetMechanismInfo;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_InitToken;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_InitPIN;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SetPIN;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_OpenSession;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_CloseSession;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_CloseAllSessions;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetSessionInfo;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetOperationState;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SetOperationState;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Login;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Logout;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_CreateObject;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_CopyObject;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DestroyObject;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetObjectSize;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetAttributeValue;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SetAttributeValue;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_FindObjectsInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_FindObjects;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_FindObjectsFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_EncryptInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Encrypt;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_EncryptUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_EncryptFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DecryptInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Decrypt;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DecryptUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DecryptFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DigestInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Digest;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DigestUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DigestKey;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DigestFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Sign;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignRecoverInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignRecover;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_VerifyInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_Verify;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_VerifyUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_VerifyFinal;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_VerifyRecoverInit;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_VerifyRecover;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DigestEncryptUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DecryptDigestUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SignEncryptUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DecryptVerifyUpdate;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GenerateKey;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GenerateKeyPair;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_WrapKey;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_UnwrapKey;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_DeriveKey;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_SeedRandom;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GenerateRandom;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_GetFunctionStatus;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_CancelFunction;
        [MarshalAs(UnmanagedType.SysInt)]
        public IntPtr C_WaitForSlotEvent;
    }
}