
using System;
using Net.Sf.Pkcs11.Objects;

namespace Net.Sf.Pkcs11
{
	/// <summary>
	/// Description of KeyPait.
	/// </summary>
	public class KeyPair
	{
		PublicKey pubKey;
		PrivateKey privKey;
		
		public PublicKey PublicKey {
			get { return pubKey; }
		}
		
		public PrivateKey PrivateKey {
			get { return privKey; }
		}
		
		
		public KeyPair(PublicKey publicKey, PrivateKey privateKey )
		{
			this.pubKey=publicKey;
			this.privKey=privateKey;
		}
	}
}
