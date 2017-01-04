
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11
{
	/// <summary>
	/// Description of Version.
	/// </summary>
	public class Version
	{
		public byte major;		
		
		public byte Major {
			get { return major; }
		}
		public byte minor;
		
		public byte Minor {
			get { return minor; }
		}
		
		internal Version(CK_VERSION ckVersion){
			this.minor=ckVersion.minor[0];
			this.major=ckVersion.major[0];
		}
		
		public override string ToString()
		{
			return string.Format("[Version Major={0} Minor={1}]", this.major, this.minor);
		}

		
	}
}
