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

using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Signers;
using Org.BouncyCastle.Crypto.Digests;
using Org.BouncyCastle.Crypto.Parameters;
namespace EidSamples
{
    class Integrity
    {
        public Integrity()
        {
        }
        public bool CheckFileIntegrity(byte[] data, byte[] signature, byte[] certificate) 
        {
            X509Certificate x509Certificate;
            x509Certificate = new X509Certificate(certificate);
            
            RsaDigestSigner signer = new RsaDigestSigner(new Sha1Digest());
            signer.Init(false, new KeyParameter(x509Certificate.GetPublicKey()));
            signer.BlockUpdate(data, 0, data.Length);
            return signer.VerifySignature(signature);
        }
   
    }
}
