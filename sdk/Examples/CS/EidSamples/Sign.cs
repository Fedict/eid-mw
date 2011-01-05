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

using System.Text;

using System.Runtime.InteropServices;

using Net.Sf.Pkcs11;
using Net.Sf.Pkcs11.Objects;
using Net.Sf.Pkcs11.Wrapper;

using System.Security.Cryptography.X509Certificates;

namespace EidSamples
{
    class Sign
    {
        private Module m = null;

        public Sign()
        {
            if (m == null)
            {
                m = Module.GetInstance("beidpkcs11D.dll");
            }
        }

        public byte[] DoSign(byte[] data, string privatekeylabel)
        { 
             // Returns a list of PKCS11 labels of the certificate on the card
            m.Initialize();
            byte[] encryptedData = null;
            try
            {
                Slot slot = m.GetSlotList(false)[0];
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
                    session.SignInit(new Mechanism(CKM.SHA1_RSA_PKCS), (PrivateKey)privatekeys[0]);
                    encryptedData = session.Sign(data);
                }
                
            }
            finally
            {
                m.Finalize_();
            }
            return encryptedData;
        }
        
    }
}
