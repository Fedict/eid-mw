
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
	/// <summary>
	/// Description of CertificateTypeAttribute.
	/// </summary>
	public class CertificateTypeAttribute:UIntAttribute
	{
		public CertificateTypeAttribute():base((uint)CKA.CERTIFICATE_TYPE)
		{
		}
		
		public CertificateTypeAttribute(CK_ATTRIBUTE ckAttr):base(ckAttr)
		{
		}
		
		public CKC CertificateType {
			get { return (CKC)base.Value; }
			set { base.Value= (uint)value;
				IsAssigned=true;
			}
		}
		
		public override string ToString()
		{
			return string.Format("[CertificateTypeAttribute CertificateType={0}]", this.CertificateType);
		}
		
		
		protected override P11Attribute GetCkLoadedCopy()
		{
			return new CertificateTypeAttribute(this.CK_ATTRIBUTE);
		}
	}
}
