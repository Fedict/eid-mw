
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11
{

	public class MechanismInfo
	{
		CK_MECHANISM_INFO mi;
		internal MechanismInfo(CK_MECHANISM_INFO mi)
		{
			this.mi=mi;
		}
		
		public uint MaxKeySize{get{return mi.ulMaxKeySize;}}
		public uint MinKeySize{get{return mi.ulMinKeySize;}}
		

		public bool HW{
			get{ return (mi.flags&0x00000001) >0; }
		}
		
		public bool Encrypt{
			get{ return (mi.flags&0x00000100) >0; }
		}
		
		public bool Decrypt{
			get{ return (mi.flags&0x00000200) >0; }
		}
		
		public bool Digest{
			get{ return (mi.flags&0x00000400) >0; }
		}
		
		public bool Sign{
			get{ return (mi.flags&0x00000800) >0; }
		}
		
		public bool SignRecover{
			get{ return (mi.flags&0x00001000) >0; }
		}
		
		public bool Verify{
			get{ return (mi.flags&0x00002000) >0; }
		}
		
		public bool VerifyRecover{
			get{ return (mi.flags&0x00004000) >0; }
		}
		
		public bool Generate{
			get{ return (mi.flags&0x00008000) >0; }
		}
		
		public bool GenerateKeyPair{
			get{ return (mi.flags&0x00010000) >0; }
		}
		
		public bool Wrap{
			get{ return (mi.flags&0x00020000) >0; }
		}
		
		public bool Unwrap{
			get{ return (mi.flags&0x00040000) >0; }
		}
		
		public bool Derive{
			get{ return (mi.flags&0x00080000) >0; }
		}
		
		public bool Extension{
			get{ return (mi.flags&0x80000000) >0; }
		}
	}
}
