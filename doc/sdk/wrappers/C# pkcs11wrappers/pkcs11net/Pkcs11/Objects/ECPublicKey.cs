
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Objects
{
	/// <summary>
	/// Description of ECPublicKey.
	/// </summary>
	public class ECPublicKey:PublicKey
	{		
		protected ByteArrayAttribute ecparams_ = new ByteArrayAttribute(CKA.EC_PARAMS);
		
		public ByteArrayAttribute ECParams {
			get { return ecparams_; }
		}
		protected ByteArrayAttribute ecpoint_ = new ByteArrayAttribute(CKA.EC_POINT);
		
		public ByteArrayAttribute ECPoint {
			get { return ecpoint_; }
		}
				
		public ECPublicKey()
		{
			this.KeyType.KeyType= CKK.EC;
		}
		
		public ECPublicKey(Session session, uint hObj):base(session,hObj)
		{
			
		}
		
		public static new P11Object GetInstance(Session session, uint hObj)
		{
			return new ECPublicKey(session,hObj) ;
		}
		
		public override void ReadAttributes(Session session)
		{
			base.ReadAttributes(session);

			ecparams_ = ReadAttribute(session,HObj,new ByteArrayAttribute(CKA.EC_PARAMS));

			ecpoint_ = ReadAttribute(session,HObj,new ByteArrayAttribute(CKA.EC_POINT));
			
		}
	}
}
