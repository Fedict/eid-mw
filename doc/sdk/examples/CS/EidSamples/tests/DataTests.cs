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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using EidSamples;
using System.Collections.Generic;
using System;
using System.Security.Cryptography.X509Certificates;
using System.Drawing;
using System.IO;

namespace EidSamples.tests
{
    /// <summary> 
    /// Tests some basic data retrieval (from the eID card, or the pkcs11 module)
    /// </summary>
    [TestClass]
    public class DataTests
    {
        /// <summary>
        /// Tests if pkcs11 finds the attached card reader "ACS CCID USB Reader 0"
        /// Test is only valid if such card reader is attached
        /// </summary>
        [TestMethod]
        public void GetSlotDescription()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("ACS CCID USB Reader 0", dataTest.GetSlotDescription());
        }
        /// <summary>
        /// Tests if pkcs11 labels the eID card as "BELPIC"
        /// </summary>
        [TestMethod]
        public void GetTokenInfoLabel()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("BELPIC", dataTest.GetTokenInfoLabel().Trim());
        }
        /// <summary>
        /// Tests the retrieval of the special status from the parsed identity file from the eID card
        /// </summary>
        [TestMethod]
        public void GetSpecialStatus()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            dataTest.GetSpecialStatus();
            //Assert.AreEqual("SPECIMEN", dataTest.GetSurname());
        }
        /// <summary>
        /// Tests the retrieval of the surname from the parsed identity file from the eID card
        /// </summary>
        [TestMethod]
        public void GetSurname()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            dataTest.GetSurname();
            //Assert.AreEqual("SPECIMEN", dataTest.GetSurname());
        }
        /// <summary>
        /// Tests the retrieval of the birth date from the parsed identity file from the eID card
        /// </summary>
        [TestMethod]
        public void GetDateOfBirth()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("01 JAN 1971", dataTest.GetDateOfBirth());
        }
        /// <summary>
        /// Tests the retrieval of the Identity file from the eID card
        /// </summary>
        [TestMethod]
        public void GetIdFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte [] idFile = dataTest.GetIdFile();
            int i = 0;
            
            // poor man's tlv parser...
            // we'll check the first two tag fields (01 and 02)
            Assert.AreEqual(0x01, idFile[i++]); // Tag
            i += idFile[i];                     // Length - skip value
            i++;
            Assert.AreEqual(0x02, idFile[i]); // Tag
        }
        /// <summary> 
        /// Tests the retrieval of the Authentication certificate label
        /// This test is only valid for eID cards with an authentication certificate
        /// </summary> 
        [TestMethod]
        public void GetCertificateLabels()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            List<string> labels = dataTest.GetCertificateLabels();
            Assert.IsTrue(labels.Contains("Authentication"),"Find Authentication certificate");
        }
        /// <summary> 
        /// Tests the retrieval of the RN certificate, and check if it is named 'root'
        /// </summary> 
        [TestMethod]
        public void GetCertificateRNFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] certificateRNFile = dataTest.GetCertificateRNFile();
            X509Certificate certificateRN;
            try
            {
                certificateRN = new X509Certificate(certificateRNFile);
                Assert.IsTrue(certificateRN.Issuer.Contains("Root"));
            }
            catch
            {
                Assert.Fail();
            }   
        }
        /// <summary> 
        /// Tests the retrieval of the Belgium root certificate, and check if it is self-signed, and named 'root'
        /// </summary> 
        [TestMethod]
        public void GetCertificateRootFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] certificateFile = dataTest.GetCertificateRootFile();
            X509Certificate certificateRoot;
            try
            {
                certificateRoot = new X509Certificate(certificateFile);
                Assert.IsTrue(certificateRoot.Subject.Contains("Root"));
                Assert.AreEqual(certificateRoot.Subject, certificateRoot.Issuer, "Should be a self-signed root certificate");
            }
            catch
            {
                Assert.Fail();
            }             
        }
        /// <summary> 
        /// Tests retrieval of the photo file, and checks if its size is as expected
        /// </summary> 
        [TestMethod]
        public void GetPhotoFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] photoFile = dataTest.GetPhotoFile();
            Bitmap photo = new Bitmap(new MemoryStream(photoFile));
            Assert.AreEqual(140, photo.Width);
            Assert.AreEqual(200, photo.Height);
            
        }
        /// <summary> 
        /// Tests the retrieval of the RN certificate, and try to add it in the my store
        /// </summary> 
        [TestMethod]
        public void StoreCertificateRNFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] certificateRNFile = dataTest.GetCertificateRNFile();
            X509Certificate2 certificateRN = new X509Certificate2(certificateRNFile);
            
            X509Store mystore = new X509Store(StoreName.My);
            mystore.Open(OpenFlags.ReadWrite);
            mystore.Add(certificateRN);
        }
    }

}

