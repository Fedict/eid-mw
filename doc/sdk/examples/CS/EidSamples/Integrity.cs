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
using PublicKey = System.Security.Cryptography.X509Certificates.PublicKey;

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

                String algo = x509Certificate.GetKeyAlgorithm();
                if (String.Compare(algo, "1.2.840.113549.1.1.1") == 0) //rsaEncryption
                {
                    // use public key from certificate during verification
                    RSACryptoServiceProvider rsa = (RSACryptoServiceProvider)x509Certificate.PublicKey.Key;

                    // verify signature. assume that the data was SHA1 hashed.
                    return rsa.VerifyData(data, "SHA1", signature);
                }
                else if (String.Compare(algo, "1.2.840.10045.2.1") == 0) //EC Public Key
                {
                    // use public key from certificate during verification
                    PublicKey publicKey = x509Certificate.PublicKey;

                    // Offset(dec)       ENCODING            ASN.1 Syntax
                    //  00               06 05                -- OBJECT_ID LENGTH
                    //  02               2B 81 04 00 22      Secp384r1(1 3 132 0 34)
                    byte[] EncodedParamsCurve = publicKey.EncodedParameters.RawData;

                    // Offset(dec)       ENCODING            ASN.1 Syntax
                    //  00              04                  compression byte
                    //  01              { 48 bytes}          --X coordinate
                    //  49:             { 48 bytes}          --Y coordinate
                    byte[] EncodedParamsPoint = publicKey.EncodedKeyValue.RawData;

                    byte[] KeyParams = new byte[5];
                    byte[] Secp384r1 = { 0x2B, 0x81, 0x04, 0x00, 0x22 };

                    byte[] KeyValue_X = new byte[48];
                    byte[] KeyValue_Y = new byte[48];

                    Array.Copy(EncodedParamsCurve, 0x02, KeyParams, 0, 5);

                    ECParameters parameters = new ECParameters();

                    //check if the curve is Secp384r1(1 3 132 0 34)
                    if (System.Collections.StructuralComparisons.StructuralEqualityComparer.Equals(KeyParams, Secp384r1))
                    {
                        //Fill in parameters named curve:
                        //Create a named curve using the specified Oid object.
                        System.Security.Cryptography.Oid cardP384oid = new Oid("ECDSA_P384");
                        parameters.Curve = ECCurve.CreateFromOid(cardP384oid);

                        Array.Copy(EncodedParamsPoint, 0x01, KeyValue_X, 0, 48);
                        Array.Copy(EncodedParamsPoint, 0x31, KeyValue_Y, 0, 48);

                        //Fill in parameters public key (Q)
                        System.Security.Cryptography.ECPoint Q;
                        Q.X = KeyValue_X;
                        Q.Y = KeyValue_Y;

                        parameters.Q = Q;
                    }
                    else
                    {
                        //not supported, cannot verify, exit
                        return false;
                    }

                    ECDsa dsa = ECDsa.Create(parameters);
                    // verify signature. assume that the data was SHA384 hashed.
                    return dsa.VerifyData(data, signature, HashAlgorithmName.SHA384);
                }
                else
                {
                    return true;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: " + e.Message);
                return false;
            }
        }
       
        /// <summary>
        /// Check a certificate chain. In order to trust the certficate, the root certificate must be
        /// stored in the trusted root certificates store. An online CRL check of the chain will be carried out.
        /// </summary>
        /// <param name="CACertificates">CA certificates</param>
        /// <param name="leafCertificate">The certificate whose chain will be checked</param>
        /// <returns>True if the certificate is trusted according to the system settings</returns>
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
