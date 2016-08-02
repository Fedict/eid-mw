/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2010-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

using Net.Sf.Pkcs11;
using Net.Sf.Pkcs11.Objects;
using Net.Sf.Pkcs11.Wrapper;

using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography;

namespace EidSamples
{
    /// Example Integrity checking class
    /** Some examples on how to verify certificates and signatures
     */
    class Integrity
    {
        public Integrity()
        {
        }
        /// <summary>
        /// Verify a signature with a given certificate. It is assumed that
        /// the signature is made from a SHA1 hash of the data.
        /// </summary>
        /// <param name="data">Signed data</param>
        /// <param name="signature">Signature to be verified</param>
        /// <param name="certificate">Certificate containing the public key used to verify the code</param>
        /// <returns>True if the verification succeeds</returns>
        public bool Verify(byte[] data, byte[] signature, byte[] certificate) 
        {
            try
            {
                X509Certificate2 x509Certificate;

                // create certificate object from byte 'file' 
                x509Certificate = new X509Certificate2(certificate);

                // use public key from certificate during verification
                RSACryptoServiceProvider rsa = (RSACryptoServiceProvider)x509Certificate.PublicKey.Key;

                // verify signature. assume that the data was SHA1 hashed.
                return rsa.VerifyData(data,"SHA256",signature);
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: " + e.Message);
                return false;
            }
        }
       
        /// <summary>
        /// Check a certificate chain. In order to trust the certficate, the root certificate must be in
        /// stored in the trusted root certificates store. An online CRL check of the chain will be carried out.
        /// </summary>
        /// <param name="CACertificates">CA certificates</param>
        /// <param name="leafCertificate">The certificate whose chain will be checked</param>
        /// <returns>True if the certificate is trusted according the system settings</returns>
        public bool CheckCertificateChain(List <byte[]> CACertificates, byte[] leafCertificate)
        {
            X509Chain chain = new X509Chain();
            // check CRL of certificates online
            chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;//X509RevocationMode.Online;

            // add intermediate CA certificates in order to build the correct chain
            foreach (byte[] CACert in CACertificates)
                chain.ChainPolicy.ExtraStore.Add(new X509Certificate2(CACert));

            // do chain validation
            bool chainIsValid = chain.Build(new X509Certificate2(leafCertificate));

            // write some more information if anything went wrong
            for (int i = 0; i < chain.ChainStatus.Length; i++)
            {
                Console.WriteLine("Chain status: " + chain.ChainStatus[i].Status 
                    + " (" + chain.ChainStatus[i].StatusInformation + ")");
            }
            return chainIsValid;
        }
   
    }
}
