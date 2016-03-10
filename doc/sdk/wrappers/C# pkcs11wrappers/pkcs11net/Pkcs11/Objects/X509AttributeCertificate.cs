
using System;

namespace Net.Sf.Pkcs11.Objects
{
	/// <summary>
	/// Description of X509AttributeCertificate.
	/// </summary>
	public class X509AttributeCertificate:Certificate
	{
		public X509AttributeCertificate()
		{
		}
		
		public static new P11Object GetInstance(Session session, uint hObj)
		{
			return null;
		}
	}
}
