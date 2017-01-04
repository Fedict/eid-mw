
using System;

namespace Net.Sf.Pkcs11.Objects
{
	public class MetaData
	{
		bool isPresent;
		public bool IsPresent {
			get { return isPresent; }
			internal set{isPresent=value;}
		}
		
		bool isSensitive;
		public bool IsSensitive {
			get { return isSensitive; }
			internal set{isSensitive=value;}
		}		
	}
}
