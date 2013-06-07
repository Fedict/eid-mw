using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Security.Cryptography;
using System.Security.Cryptography.Xml;
using System.Xml;

namespace beidmdrvTest
{
  class SignTests
  {
    public void TestSignWithSignatureKey()
    {
      try
      {
        // Create a new CspParameters object to specify a key container.
        CspParameters cspParms = new CspParameters(1, //provider type 1 : PROV_RSA_FULL 
            "Microsoft Base Smart Card Crypto Provider"
        );

        cspParms.Flags = CspProviderFlags.UseDefaultKeyContainer;
        cspParms.Flags |= CspProviderFlags.UseExistingKey;

        //cspParms.KeyNumber = (int) KeyNumber.Exchange;// Specify an exchange key.
        cspParms.KeyNumber = (int)KeyNumber.Signature;// Specify a signature key.

        // Create a new RSACryptoServiceProvider that uses the RSA signing key. 
        RSACryptoServiceProvider rsacsp = new RSACryptoServiceProvider(cspParms);

        CspKeyContainerInfo keycontinfo = rsacsp.CspKeyContainerInfo;
        string unkeycontname = keycontinfo.UniqueKeyContainerName;

        Console.WriteLine();
        Console.WriteLine("UniqueKeyContainerName: " + unkeycontname);

        // Create some data to sign. 
        byte[] data = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7 };

        Console.WriteLine("Data: " + BitConverter.ToString(data));

        // Sign the data using the Smart Card CryptoGraphic Provider. 
        byte[] sig = rsacsp.SignData(data, "SHA1");

        Console.WriteLine("Signature: " + BitConverter.ToString(sig));

        // Verify the data using the Smart Card CryptoGraphic Provider. 
        bool verified = rsacsp.VerifyData(data, "SHA1", sig);

        Console.WriteLine("Verified: " + verified);

      }
      catch (Exception e)
      {
        Console.WriteLine(e.Message);
      }
    }
  }
}
