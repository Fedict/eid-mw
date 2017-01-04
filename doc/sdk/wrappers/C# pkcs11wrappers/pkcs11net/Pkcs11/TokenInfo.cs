
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11
{
	/// <summary>
	/// Description of TokenInfo.
	/// </summary>
	public class TokenInfo
	{
		CK_TOKEN_INFO paramCK_TOKEN_INFO;
		
		internal TokenInfo(CK_TOKEN_INFO paramCK_TOKEN_INFO)
		{
			this.paramCK_TOKEN_INFO=paramCK_TOKEN_INFO;
			
		}
		
		public string Label {
			get { return P11Util.ConvertToUtf8String(paramCK_TOKEN_INFO.label); }
		}
		
		public string ManufacturerID {
			get { return P11Util.ConvertToUtf8String(paramCK_TOKEN_INFO.manufacturerID); }
		}
		
		public string Model {
			get { return  P11Util.ConvertToUtf8String(paramCK_TOKEN_INFO.model); }
		}
		
		public string SerialNumber {
			get { return P11Util.ConvertToUtf8String(paramCK_TOKEN_INFO.serialNumber); }
		}
		
		public long MaxSessionCount {
			get { return paramCK_TOKEN_INFO.ulMaxSessionCount; }
		}
		
		public long SessionCount {
			get { return paramCK_TOKEN_INFO.ulSessionCount; }
		}
		
		public long MaxRwSessionCount {
			get { return paramCK_TOKEN_INFO.ulMaxRwSessionCount; }
		}
		
		public long RwSessionCount {
			get { return paramCK_TOKEN_INFO.ulRwSessionCount; }
		}
		
		public long MaxPinLen {
			get { return paramCK_TOKEN_INFO.ulMaxPinLen; }
		}
		
		public long MinPinLen {
			get { return paramCK_TOKEN_INFO.ulMinPinLen; }
		}
		
		public long TotalPublicMemory {
			get { return paramCK_TOKEN_INFO.ulTotalPublicMemory; }
		}
		
		public long FreePublicMemory {
			get { return paramCK_TOKEN_INFO.ulFreePublicMemory;}
		}
		
		public long TotalPrivateMemory {
			get { return paramCK_TOKEN_INFO.ulTotalPrivateMemory; }
		}
		
		public long FreePrivateMemory {
			get { return paramCK_TOKEN_INFO.ulFreePrivateMemory; }
		}
		
		public Version HardwareVersion {
			get { return new Version(paramCK_TOKEN_INFO.hardwareVersion); }
		}
		
		
		public Version FirmwareVersion {
			get { return  new Version(paramCK_TOKEN_INFO.firmwareVersion); }
		}
		protected DateTime time;
		
		public DateTime Time {
			get {
				return P11Util.ConvertToDateTimeYYYYMMDDhhmmssxx( P11Util.ConvertToASCIIString(paramCK_TOKEN_INFO.utcTime));
			}
		}
		
		public bool Rng {
			get { return ((paramCK_TOKEN_INFO.flags & 1L) != 0L); }
		}
		
		public bool WriteProtected {
			get { return ((paramCK_TOKEN_INFO.flags & 0x2) != 0L); }
		}
		
		public bool LoginRequired {
			get { return ((paramCK_TOKEN_INFO.flags & 0x4) != 0L); }
		}
		
		public bool UserPinInitialized {
			get { return ((paramCK_TOKEN_INFO.flags & 0x8) != 0L); }
		}
		
		public bool RestoreKeyNotNeeded {
			get { return  ((paramCK_TOKEN_INFO.flags & 0x20) != 0L); }
		}
		
		public bool ClockOnToken {
			get { return ((paramCK_TOKEN_INFO.flags & 0x40) != 0L); }
		}
		
		public bool ProtectedAuthenticationPath {
			get { return ((paramCK_TOKEN_INFO.flags & 0x100) != 0L); }
		}
		
		public bool DualCryptoOperations {
			get { return ((paramCK_TOKEN_INFO.flags & 0x200) != 0L); }
		}
		
		public bool TokenInitialized {
			get { return ((paramCK_TOKEN_INFO.flags &  0x400) != 0L); }
		}
		
		public bool SecondaryAuthentication {
			get { return ((paramCK_TOKEN_INFO.flags & 0x800) != 0L); }
		}
		
		public bool UserPinCountLow {
			get { return ((paramCK_TOKEN_INFO.flags & 0x10000) != 0L); }
		}
		
		public bool UserPinFinalTry {
			get { return ((paramCK_TOKEN_INFO.flags & 0x20000) != 0L); }
		}
		
		public bool UserPinLocked {
			get { return ((paramCK_TOKEN_INFO.flags & 0x40000) != 0L); }
		}
		
		public bool UserPinToBeChanged {
			get { return ((paramCK_TOKEN_INFO.flags & 0x80000) != 0L); }
		}
		
		public bool SoPinCountLow {
			get { return ((paramCK_TOKEN_INFO.flags & 0x100000) != 0L); }
		}
		
		public bool SoPinFinalTry {
			get { return  ((paramCK_TOKEN_INFO.flags & 0x200000) != 0L); }
		}
		
		public bool SoPinLocked {
			get { return ((paramCK_TOKEN_INFO.flags & 0x400000) != 0L); }
		}
		
		public bool SoPinToBeChanged {
			get { return ((paramCK_TOKEN_INFO.flags & 0x800000) != 0L);}
		}
	}
}
