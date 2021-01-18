/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2010-2016 FedICT.
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

using System.Text;

using System.Runtime.InteropServices;

using Net.Sf.Pkcs11;
using Net.Sf.Pkcs11.Objects;
using Net.Sf.Pkcs11.Wrapper;

using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography;

namespace EidSamples
{
    class Sign
    {
        private Module m = null;
        private String mFileName;
        /// <summary>
        /// Default constructor. Will instantiate the beidpkcs11.dll pkcs11 module
        /// </summary>
        public Sign()
        {
            mFileName = "beidpkcs11.dll";
        }
        public Sign(String moduleFileName)
        {
            mFileName = moduleFileName;
        }
        /// <summary>
        /// Sign data with a named private key
        /// </summary>
        /// <param name="data">Data to be signed</param>
        /// <param name="privatekeylabel">Label for private key. Can be "Signature" or "Authentication"</param>
        /// <returns>Signed data.</returns>
        public byte[] DoSign(byte[] data, string privatekeylabel)
        { 
            if (m == null)
            {
                // link with the pkcs11 DLL
                m = Module.GetInstance(mFileName);
            } //m.Initialize();

            byte[] encryptedData = null;
            try
            {
                Slot slot = m.GetSlotList(true)[0];
                Session session = slot.Token.OpenSession(true);
                ObjectClassAttribute classAttribute = new ObjectClassAttribute(CKO.PRIVATE_KEY);
                ByteArrayAttribute keyLabelAttribute = new ByteArrayAttribute(CKA.LABEL);
                keyLabelAttribute.Value = System.Text.Encoding.UTF8.GetBytes(privatekeylabel);

                session.FindObjectsInit(new P11Attribute[] {
                     classAttribute,
                     keyLabelAttribute
                    }
                );
                P11Object[] privatekeys = session.FindObjects(1) as P11Object[];
                session.FindObjectsFinal();

                if (privatekeys.Length >= 1)
                {
                    if (privatekeys[0] != null)
                    {
                        PrivateKey key = (PrivateKey)privatekeys[0];
                        if (key.KeyType.KeyType == CKK.EC)
                        {
                            SHA384 sha = new SHA384CryptoServiceProvider();
                            byte[] HashValue = sha.ComputeHash(data);
                            session.SignInit(new Mechanism(CKM.ECDSA), (PrivateKey)privatekeys[0]);
                            encryptedData = session.Sign(HashValue);
                        }
                        else if (key.KeyType.KeyType == CKK.RSA)
                        {
                            session.SignInit(new Mechanism(CKM.SHA1_RSA_PKCS), (PrivateKey)privatekeys[0]);
                            encryptedData = session.Sign(data);
                        }
                    }
                }
                
            }
            finally
            {
                m.Dispose();
                m = null;
            }
            return encryptedData;
        }

        /// <summary>
        /// Challenge an applet 1.8 card
        /// </summary>
        /// <param name="data">Data to be signed</param>
        /// <returns>Signed challenge data.</returns>
        public byte[] DoChallenge(byte[] data)
        {
            if (m == null)
            {
                // link with the pkcs11 DLL
                m = Module.GetInstance(mFileName);
            } //m.Initialize();

            byte[] encryptedData = null;
            try
            {
                Slot slot = m.GetSlotList(true)[0];
                Session session = slot.Token.OpenSession(true);
                ObjectClassAttribute classAttribute = new ObjectClassAttribute(CKO.PRIVATE_KEY);
                ByteArrayAttribute keyLabelAttribute = new ByteArrayAttribute(CKA.LABEL);
                keyLabelAttribute.Value = System.Text.Encoding.UTF8.GetBytes("Card");

                session.FindObjectsInit(new P11Attribute[] {
                     classAttribute,
                     keyLabelAttribute
                    }
                );
                P11Object[] privatekeys = session.FindObjects(1) as P11Object[];
                session.FindObjectsFinal();

                if (privatekeys.Length >= 1)
                {
                    session.SignInit(new Mechanism(CKM.ECDSA), (PrivateKey)privatekeys[0]);
                    encryptedData = session.Sign(data);
                }
            }
            finally
            {
                m.Dispose();
                m = null;
            }
            return encryptedData;
        }

    }
}
