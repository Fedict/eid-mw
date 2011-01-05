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
    class ReadData
    {
        private Module m = null;
        /// <summary>
        /// Default constructor. Will instantiate the beidpkcs11.dll pkcs11 module
        /// </summary>
        public ReadData()
        {
            if (m == null)
            {
                m = Module.GetInstance("beidpkcs11.dll");
            }
        }

        /// <summary>
        /// Gets the description of the first slot (cardreader) found
        /// </summary>
        /// <returns>Description of the first slot found</returns>
        public string GetSlotDescription()
        { 
            String slotID;
            m.Initialize();
            try
            {
                // Look for slots (cardreaders)
                // GetSlotList(false) will return all cardreaders
                Slot []slots = m.GetSlotList(false);
                if (slots.Length == 0)
                    slotID = "";
                slotID = slots[0].SlotInfo.SlotDescription.Trim();
            }
            finally
            {
                m.Finalize_();
            }
            return slotID;
        }

        /// <summary>
        /// Gets label of token found in the first non-empty slot (cardreader)
        /// </summary>
        /// <returns></returns>
        public string GetTokenInfoLabel()
        {
            String tokenInfoLabel;
            m.Initialize();
            try
            {
                // Look for slots (cardreaders)
                // GetSlotList(true) will return only the cardreaders with a 
                // token (smart card)
                tokenInfoLabel = m.GetSlotList(true)[0].Token.TokenInfo.Label.Trim();
            }
            finally
            {
                m.Finalize_();
            }
            return tokenInfoLabel;
            
        }

        /// <summary>
        /// Get surname of the owner of the token (eid) in the first non-empty slot (cardreader)
        /// </summary>
        /// <returns></returns>
        public string GetSurname()
        {
            String name;
            m.Initialize();
            try
            {
                // Get the first slot (cardreader) with a token
                Slot slot = m.GetSlotList(true)[0];
                Session session = slot.Token.OpenSession(true);
                
                // Search for objects
                // First, define a search template 
                ByteArrayAttribute surnameLabel = new ByteArrayAttribute(CKA.LABEL);
                surnameLabel.Value = System.Text.Encoding.UTF8.GetBytes("Surname");
                session.FindObjectsInit(new P11Attribute[] {
                        surnameLabel
                    }
                    );

                Net.Sf.Pkcs11.Objects.Data surname = session.FindObjects(1)[0] as Net.Sf.Pkcs11.Objects.Data;

                name = System.Text.Encoding.UTF8.GetString(surname.Value.Value);
                session.FindObjectsFinal();
            }
            finally
            {
                m.Finalize_();
            }
            return name;

        }
        public string GetDateOfBirth()
        {
            String value;
            m.Initialize();
            try
            {
                Slot slot = m.GetSlotList(false)[0];
                Session session = slot.Token.OpenSession(true);
                ByteArrayAttribute dateOfBirthLabel = new ByteArrayAttribute(CKA.LABEL);
                dateOfBirthLabel.Value = System.Text.Encoding.UTF8.GetBytes("Date_Of_Birth");
                session.FindObjectsInit(new P11Attribute[] {
                    dateOfBirthLabel
                }
                    );

                Data dateOfBirth = session.FindObjects(1)[0] as Data;
                value = System.Text.Encoding.UTF8.GetString(dateOfBirth.Value.Value);
                session.FindObjectsFinal();
            }
            finally
            {
                m.Finalize_();
            }
            return value;
        }
        public byte[] GetIdFile()
        {
            return GetFile("DATA_FILE");
        }
        public byte[] GetAddressFile()
        {
            return GetFile("ADDRESS_FILE");
        }
        public byte[] GetPhotoFile()
        {
            return GetFile("PHOTO");
        }
        public byte[] GetIdSignatureFile()
        {
            return GetFile("SGN_DATA_FILE");
        }
        public byte[] GetAddressSignatureFile()
        {
            return GetFile("SGN_ADDRESS_FILE");
        }
        public byte[] GetCertificateRNFile()
        {
            return GetFile("CERT_RN_FILE");
        }
        public byte[] GetCertificateRNCAFile()
        {
            return GetFile("CERT_RNCA_FILE");
        }        
        private byte[] GetFile(String Filename)
        {
            byte[] value = null;
            m.Initialize();
            try
            {
                Slot slot = m.GetSlotList(false)[0];
                Session session = slot.Token.OpenSession(true);
                ByteArrayAttribute fileLabel = new ByteArrayAttribute(CKA.LABEL);
                fileLabel.Value = System.Text.Encoding.UTF8.GetBytes(Filename);
                session.FindObjectsInit(new P11Attribute[] {
                        fileLabel
                    });
                P11Object[] foundObjects = session.FindObjects(1);
                if (foundObjects.Length != 0)
                {
                    Data file = foundObjects[0] as Data;
                    value = file.Value.Value;
                }
                session.FindObjectsFinal();
            }
            finally
            {
                m.Finalize_();
            }
            return value;
        }
        public byte[] GetCertificateAuthenticationFile()
        {
            // Return the "authentication" leaf certificate file
            return GetCertificateFile("Authentication");
        }
        public byte[] GetCertificateSignatureFile()
        {
            // Return the "signature" leaf certificate file
            return GetCertificateFile("Signature");
        }
        public byte[] GetCertificateCAFile()
        {
            // return the Intermediate CA certificate file
            return GetCertificateFile("CA");
        }
        public byte[] GetCertificateRootFile()
        {
            // return the root certificate file
            return GetCertificateFile("Root");
        }
        private byte[] GetCertificateFile(String Certificatename)
        {
            // returns Root Certificate on the eid.
            byte[] value = null;
            m.Initialize();
            try
            {
                Slot slot = m.GetSlotList(false)[0];
                Session session = slot.Token.OpenSession(true);
                ByteArrayAttribute fileLabel = new ByteArrayAttribute(CKA.LABEL);
                ObjectClassAttribute certificateAttribute = new ObjectClassAttribute(CKO.CERTIFICATE);
                fileLabel.Value = System.Text.Encoding.UTF8.GetBytes(Certificatename);
                session.FindObjectsInit(new P11Attribute[] {
                        certificateAttribute,
                        fileLabel
                    });
                P11Object[] foundObjects = session.FindObjects(1);
                if (foundObjects.Length != 0)
                {
                    X509PublicKeyCertificate cert = foundObjects[0] as X509PublicKeyCertificate;
                    value = cert.Value.Value;
                }
                session.FindObjectsFinal();
            }
            finally
            {
                m.Finalize_();
            }
            return value;

        }
        public List <string> GetCertificateLabels()
        {
            // Returns a list of PKCS11 labels of the certificate on the card
            m.Initialize();
            List<string> labels = new List<string>();
            try
            {

                Slot slot = m.GetSlotList(false)[0];
                Session session = slot.Token.OpenSession(true);
                ObjectClassAttribute certificateAttribute = new ObjectClassAttribute(CKO.CERTIFICATE);
                session.FindObjectsInit(new P11Attribute[] {
                     certificateAttribute
                    }
                );


                P11Object[] certificates = session.FindObjects(100) as P11Object[];
                foreach (P11Object certificate in certificates)
                {
                    labels.Add (new string (((X509PublicKeyCertificate)certificate).Label.Value));
                }
                
                //certs. new X509Certificate(certificate.Value.Value);
                
                session.FindObjectsFinal();
            }
            finally
            {
                m.Finalize_();
            }
            return labels;
        }
    }
}
