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
using PublicKey = Net.Sf.Pkcs11.Objects.PublicKey;

namespace EidSamples
{
    class ReadData
    {
        private Module m = null;
        private String mFileName;
        /// <summary>
        /// Default constructor. Will instantiate the beidpkcs11.dll pkcs11 module
        /// </summary>
        public ReadData()
        {
            mFileName = "beidpkcs11.dll";
        }
        public ReadData(String moduleFileName)
        {
            mFileName = moduleFileName;
        }
        /// <summary>
        /// Gets the description of the first slot (cardreader) found
        /// </summary>
        /// <returns>Description of the first slot found</returns>
        public string GetSlotDescription()
        {
            String slotID;
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            //initialization now occurs within the getinstance function
            //m.Initialize();
            try
            {
                // Look for slots (cardreaders)
                // GetSlotList(false) will return all cardreaders
                Slot[] slots = m.GetSlotList(false);
                if (slots.Length == 0)
                    slotID = "";
                else
                    slotID = slots[0].SlotInfo.SlotDescription.Trim();
            }
            finally
            {
                //m.Finalize_();
                m.Dispose();
                m = null;
            }
            return slotID;
        }


        /// <summary>
        /// Tries to create a Session, returns NULL in case of failure
        /// </summary>
        /// <returns></returns>
        private Session CreateSession (Slot slot )
        {
            try
            {
                return slot.Token.OpenSession(true);
            }
            catch
            {
                return null;
            }       
        }

        /// <summary>
        /// Gets label of token found in the first non-empty slot (cardreader)
        /// </summary>
        /// <returns></returns>
        public string GetTokenInfoLabel()
        {
            String tokenInfoLabel;
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            //m.Initialize();
            try
            {
                // Look for slots (cardreaders)
                // GetSlotList(true) will return only the cardreaders with a 
                // token (smart card)
                tokenInfoLabel = m.GetSlotList(true)[0].Token.TokenInfo.Label.Trim();
            }
            finally
            {
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return tokenInfoLabel;

        }

        /// <summary>
        /// Get surname of the owner of the token (eid) in the first non-empty slot (cardreader)
        /// </summary>
        /// <returns></returns>
        public string GetSpecialStatus()
        {
            return GetData("special_status");
        }

        /// <summary>
        /// Get surname of the owner of the token (eid) in the first non-empty slot (cardreader)
        /// </summary>
        /// <returns></returns>
        public string GetSurname()
        {
            return GetData("surname");
        }

        /// <summary>
        /// Get date of birth of the owner. This is a language specific string
        /// More info about the format can be found in the eid specs.
        /// </summary>
        /// <returns></returns>
        public string GetDateOfBirth()
        {
            return GetData("date_of_birth");
        }

    
        /// <summary>
        /// Generic function to get string data objects from label
        /// </summary>
        /// <param name="label">Value of label attribute of the object</param>
        /// <returns></returns>
        public string GetData(String label)
        {
            String value = "";
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            // pkcs11 module init
            //m.Initialize();
            try
            {
                // Get the first slot (cardreader) with a token
                Slot[] slotlist = m.GetSlotList(true);
                if (slotlist.Length > 0)
                {
                    Slot slot = slotlist[0];

                    //Session session = slot.Token.OpenSession(true);
                    Session session = CreateSession(slot);
                    if (session != null)
                    {

                        // Search for objects
                        // First, define a search template 

                        // "The label attribute of the objects should equal ..."
                        ByteArrayAttribute classAttribute = new ByteArrayAttribute(CKA.CLASS);
                        classAttribute.Value = BitConverter.GetBytes((uint)Net.Sf.Pkcs11.Wrapper.CKO.DATA);


                        ByteArrayAttribute labelAttribute = new ByteArrayAttribute(CKA.LABEL);
                        labelAttribute.Value = System.Text.Encoding.UTF8.GetBytes(label);


                        session.FindObjectsInit(new P11Attribute[] { classAttribute, labelAttribute });
                        P11Object[] foundObjects = session.FindObjects(50);
                        int counter = foundObjects.Length;
                        Data data;
                        while (counter > 0)
                        {
                            //foundObjects[counter-1].ReadAttributes(session);
                            //public static BooleanAttribute ReadAttribute(Session session, uint hObj, BooleanAttribute attr)
                            data = foundObjects[counter - 1] as Data;
                            label = data.Label.ToString();
                            if (label != null)
                                Console.WriteLine(label);
                            if (data.Value.Value != null)
                            {
                                value = System.Text.Encoding.UTF8.GetString(data.Value.Value);
                                Console.WriteLine(value);
                            }
                            counter--;
                        }
                        session.FindObjectsFinal();
                        session.Dispose();
                    }
                }
                else
                {
                    Console.WriteLine("No card found\n");
                }
            }
            finally
            {
                // pkcs11 finalize
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return value;
        }

        public void GetAndTestIdFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            //byte[] certificateRRN = null;
            //Assert.isFalse(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        /// <summary>
        /// Return ID data file contents
        /// </summary>
        /// <returns></returns>
        public byte[] GetIdFile()
        {
            return GetFile("DATA_FILE");
        }
        /// <summary>
        /// Return Address file contents
        /// </summary>
        /// <returns></returns>
        public byte[] GetAddressFile()
        {
            return GetFile("ADDRESS_FILE");
        }
        /// <summary>
        /// Return Photo file contents
        /// </summary>
        /// <returns></returns>
        public byte[] GetPhotoFile()
        {
            return GetFile("PHOTO_FILE");
        }
        /// <summary>
        /// Return ID file signature
        /// </summary>
        /// <returns></returns>
        public byte[] GetIdSignatureFile()
        {
            return GetFile("SIGN_DATA_FILE");
        }
        /// <summary>
        /// Return Address file signature
        /// </summary>
        /// <returns></returns>
        public byte[] GetAddressSignatureFile()
        {
            return GetFile("SIGN_ADDRESS_FILE");
        }
        /// <summary>
        /// Return RRN Certificate. This certificate is used to validate
        /// the file signatures
        /// </summary>
        /// <returns></returns>
        public byte[] GetCertificateRNFile()
        {
            return GetFile("CERT_RN_FILE");
        }

        /// <summary>
        /// Return raw byte data from objects
        /// </summary>
        /// <param name="Filename">Label value of the object</param>
        /// <returns>byte array with file</returns>
        private byte[] GetFile(String Filename)
        {
            byte[] value = null;
            // pkcs11 module init
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            //m.Initialize();
            try
            {
                // Get the first slot (cardreader) with a token
                Slot[] slotlist = m.GetSlotList(true);
                if (slotlist.Length > 0)
                {
                    Slot slot = slotlist[0];
                    Session session = slot.Token.OpenSession(true);

                    // Search for objects
                    // First, define a search template 

                    // "The label attribute of the objects should equal ..."                
                    ByteArrayAttribute fileLabel = new ByteArrayAttribute(CKA.LABEL);
                    fileLabel.Value = System.Text.Encoding.UTF8.GetBytes(Filename);
                    ByteArrayAttribute fileData = new ByteArrayAttribute(CKA.CLASS);
                    fileData.Value = BitConverter.GetBytes((uint)Net.Sf.Pkcs11.Wrapper.CKO.DATA);
                    session.FindObjectsInit(new P11Attribute[] {
                        fileLabel,fileData
                    });
                    P11Object[] foundObjects = session.FindObjects(1);
                    if (foundObjects.Length != 0)
                    {
                        Data file = foundObjects[0] as Data;
                        value = file.Value.Value;
                    }
                    session.FindObjectsFinal();
                }
                else
                {
                    Console.WriteLine("No card found\n");
                }
            }
            finally
            {
                // pkcs11 finalize
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return value;
        }
        /// <summary>
        /// Return the "authentication" leaf certificate file
        /// </summary>
        /// <returns></returns>
        public byte[] GetCertificateAuthenticationFile()
        {
            return GetCertificateFile("Authentication");
        }
        /// <summary>
        /// Return the "signature" leaf certificate file
        /// </summary>
        /// <returns></returns>
        public byte[] GetCertificateSignatureFile()
        {
            return GetCertificateFile("Signature");
        }
        /// <summary>
        /// Return the Intermediate CA certificate file
        /// </summary>
        /// <returns></returns>
        public byte[] GetCertificateCAFile()
        {
            return GetCertificateFile("CA");
        }
        /// <summary>
        /// Return the root certificate file
        /// </summary>
        /// <returns></returns>
        public byte[] GetCertificateRootFile()
        {
            return GetCertificateFile("Root");
        }
        /// <summary>
        /// Return raw byte data from objects of object class Certificate
        /// </summary>
        /// <param name="Certificatename">Label value of the certificate object</param>
        /// <returns>byte array with certificate file</returns>
        private byte[] GetCertificateFile(String Certificatename)
        {
            // returns Root Certificate on the eid.
            byte[] value = null;
            // pkcs11 module init
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            //m.Initialize();
            try
            {
                // Get the first slot (cardreader) with a token
                Slot[] slotlist = m.GetSlotList(true);
                if (slotlist.Length > 0)
                {
                    Slot slot = slotlist[0];
                    Session session = slot.Token.OpenSession(true);
                    // Search for objects
                    // First, define a search template 

                    // "The label attribute of the objects should equal ..."      
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
                else
                {
                    Console.WriteLine("No card found\n");
                }
            }
            finally
            {
                // pkcs11 finalize
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return value;

        }
        /// <summary>
        /// Returns a list of PKCS11 labels of the certificate on the card
        /// </summary>
        /// <returns>List of labels of certificate objects</returns>
        public List<string> GetCertificateLabels()
        {
            // pkcs11 module init
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            //m.Initialize();
            List<string> labels = new List<string>();
            try
            {
                // Get the first slot (cardreader) with a token
                Slot[] slotlist = m.GetSlotList(true);
                if (slotlist.Length > 0)
                {
                    Slot slot = slotlist[0];
                    Session session = slot.Token.OpenSession(true);
                    // Search for objects
                    // First, define a search template 

                    // "The object class of the objects should be "certificate"" 
                    ObjectClassAttribute certificateAttribute = new ObjectClassAttribute(CKO.CERTIFICATE);
                    session.FindObjectsInit(new P11Attribute[] {
                     certificateAttribute
                    }
                    );


                    P11Object[] certificates = session.FindObjects(100) as P11Object[];
                    foreach (P11Object certificate in certificates)
                    {
                        labels.Add(new string(((X509PublicKeyCertificate)certificate).Label.Value));
                    }
                    session.FindObjectsFinal();
                }
                else
                {
                    Console.WriteLine("No card found\n");
                }
            }
            finally
            {
                // pkcs11 finalize
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return labels;
        }

        /// <summary>
        /// Return raw byte data from objects of object class Public Key
        /// </summary>
        /// <param name="PubKeyName">Label value of the key object</param>
        /// <returns>ECPublicKey object of the public key found</returns>
        public ECPublicKey GetPublicKey(String PubKeyName)
        {
            ECPublicKey eCPublicKey = null;
            // pkcs11 module init
            if (m == null)
            {
                m = Module.GetInstance(mFileName);
            }
            try
            {
                // Get the first slot (cardreader) with a token
                Slot[] slotlist = m.GetSlotList(true);
                if (slotlist.Length > 0)
                {
                    Slot slot = slotlist[0];
                    Session session = slot.Token.OpenSession(true);
                    // Search for objects
                    // First, define a search template 

                    // The label attribute of the objects should equal PubKeyName
                    ObjectClassAttribute classAttribute = new ObjectClassAttribute(CKO.PUBLIC_KEY);
                    ByteArrayAttribute keyLabelAttribute = new ByteArrayAttribute(CKA.LABEL);
                    keyLabelAttribute.Value = System.Text.Encoding.UTF8.GetBytes(PubKeyName);

                    session.FindObjectsInit(new P11Attribute[] { classAttribute, keyLabelAttribute });
                    //P11Object[] pubkeys = session.FindObjects(1) as P11Object[];
                    P11Object[] pubkeys = session.FindObjects(1);
                    session.FindObjectsFinal();

                    if ( (pubkeys.Length == 0) || (pubkeys[0] == null) )
                    {
                        Console.WriteLine("Public Key Object not found");
                        return eCPublicKey;
                    }
                    eCPublicKey = (ECPublicKey)pubkeys[0];
                  //  session.FindObjectsFinal();
                }
                else
                {
                    Console.WriteLine("No card found\n");
                }
            }
            finally
            {
                // pkcs11 finalize
                m.Dispose();//m.Finalize_();
                m = null;
            }
            return eCPublicKey;
        }
    }
}
