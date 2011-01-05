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
    class Integrity
    {
        public Integrity()
        {
        }
        public bool CheckFileIntegrity(byte[] data, byte[] signature, byte[] certificate) 
        {
            try
            {
                X509Certificate2 x509Certificate;
                x509Certificate = new X509Certificate2(certificate);
                RSACryptoServiceProvider rsa = (RSACryptoServiceProvider)x509Certificate.PublicKey.Key;
                return rsa.VerifyData(data,"SHA1",signature);
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: " + e.Message);
                return false;
            }
        }
        public bool CheckCertificateChain(List <byte[]> CACertificates, byte[] leafCertificate)
        {
            X509Chain chain = new X509Chain();
            chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;
            foreach (byte[] CACert in CACertificates)
                chain.ChainPolicy.ExtraStore.Add(new X509Certificate2(CACert));
            bool chainIsValid = chain.Build(new X509Certificate2(leafCertificate));
            for (int i = 0; i < chain.ChainStatus.Length; i++)
            {
                Console.WriteLine("Chain status: " + chain.ChainStatus[i].Status 
                    + " (" + chain.ChainStatus[i].StatusInformation + ")");
            }
            return chainIsValid;
        }
   
    }
}
