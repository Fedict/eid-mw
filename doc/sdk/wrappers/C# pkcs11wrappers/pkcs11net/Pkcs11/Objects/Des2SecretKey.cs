using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
	public class Des2SecretKey:SecretKey
	{
		ByteArrayAttribute value_= new ByteArrayAttribute(CKA.VALUE);
		
		public ByteArrayAttribute Value {
			get { return value_; }
		}
		
		public Des2SecretKey(){
			this.KeyType.KeyType= CKK.DES2;
		}
		
		public Des2SecretKey(Session session, uint hObj):base(session,hObj)
		{
		}
		
		public override void ReadAttributes(Session session)
		{
			base.ReadAttributes(session);
			
			value_=ReadAttribute(session,HObj,new ByteArrayAttribute(CKA.VALUE));
		}
		
		public static new P11Object GetInstance(Session session, uint hObj)
		{
			return new Des2SecretKey(session,hObj);
		}
	}
}
