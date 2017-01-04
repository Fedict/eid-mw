
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11
{
	/// <summary>
	/// Description of TokenException.
	/// </summary>
	public class TokenException:Exception
	{
		public TokenException()
		{
		}
		
		public TokenException(CKR errorCode):base(errorCode.ToString())
		{
			this.errorCode=errorCode;
		}
		
		CKR errorCode;
		
		public CKR ErrorCode {
			get { return errorCode; }
		}
		
	}
}
