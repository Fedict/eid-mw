
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
	/// <summary>
	/// Description of ObjectClassAttribute.
	/// </summary>
	public class ObjectClassAttribute:UIntAttribute
	{
		public ObjectClassAttribute():base((uint)CKA.CLASS)
		{
		}
		
		public ObjectClassAttribute(CK_ATTRIBUTE ckAttr):base(ckAttr)
		{
		}
		public ObjectClassAttribute(CKO objectType):base((uint)CKA.CLASS)
		{
			ObjectType=objectType;
		}
		
		public CKO ObjectType {
			get { return (CKO)base.Value; }
			internal set { base.Value= (uint)value; }
		}
		
		public override string ToString()
		{
			return string.Format("[ObjectClassAttribute ObjectType={0}]", this.ObjectType);
		}
		
		
		protected override P11Attribute GetCkLoadedCopy()
		{
			return new ObjectClassAttribute(this.CK_ATTRIBUTE);
		}
	}
}
