
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
	/// <summary>
	/// Description of KeyTypeAttribute.
	/// </summary>
	public class KeyTypeAttribute:UIntAttribute
	{
		public KeyTypeAttribute():base((uint)CKA.KEY_TYPE)
		{
		}
		
		public KeyTypeAttribute(CKK keyType):base((uint)CKA.KEY_TYPE)
		{
			KeyType=keyType;
		}
		
		
		public KeyTypeAttribute(CK_ATTRIBUTE ckAttr):base(ckAttr)
		{
		}
		
		public CKK KeyType {
			get { return (CKK)base.Value; }
			set { base.Value= (uint)value; }
		}
		
		protected override P11Attribute GetCkLoadedCopy()
		{
			return new KeyTypeAttribute(this.CK_ATTRIBUTE);
		}
	}
}
