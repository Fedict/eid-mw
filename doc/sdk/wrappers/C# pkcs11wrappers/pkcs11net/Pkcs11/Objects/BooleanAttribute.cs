using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
	public class BooleanAttribute:P11Attribute
	{
		bool val_;
		
		public bool Value {
			get { return val_; }
			set { val_ = value;
			IsAssigned=true;
			}
		}
		
		internal BooleanAttribute(uint type ):base(type){
			
		}
		internal BooleanAttribute(CKA type ):base((uint)type){
		}
		
		internal BooleanAttribute(CK_ATTRIBUTE attr ):base(attr){
			
		}
		
		public override byte[] Encode(){
			return new byte[]{ (byte)(Value==true? 1:0) };
		}
		public override void Decode(byte[] val){
			Value= val[0]==1;
		}
		
		public override string ToString()
		{
			return string.Format("[BooleanAttribute Value={0}]", this.val_);
		}
		
		
		protected override P11Attribute GetCkLoadedCopy()
		{
			return new BooleanAttribute(this.CK_ATTRIBUTE);
		}

	}
}
