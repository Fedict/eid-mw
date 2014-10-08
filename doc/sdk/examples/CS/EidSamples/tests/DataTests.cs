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
using NUnit.Framework;
using EidSamples;
using System.Collections.Generic;
using System;
using System.Security.Cryptography.X509Certificates;
using System.Drawing;
using System.IO;

namespace EidSamples.tests
{
    [TestFixture]
    public class DataTests
    {
        [Test]
        public void GetSlotDescription()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("ACS CCID USB Reader 0", dataTest.GetSlotDescription());
        }
        [Test]
        public void GetTokenInfoLabel()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("BELPIC", dataTest.GetTokenInfoLabel().Trim());
        }

        [Test]
        public void GetSurname()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            dataTest.GetSurname();
            //Assert.AreEqual("SPECIMEN", dataTest.GetSurname());
        }
        [Test]
        public void GetChipnumber()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            dataTest.GetChipnumber();
            //Assert.AreEqual("SPECIMEN", dataTest.GetSurname());
        }
        [Test]
        public void GetDateOfBirth()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Assert.AreEqual("01 JAN 1971", dataTest.GetDateOfBirth());
        }

        [Test]
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
        [Test]
        public void GetCertificateLabels()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            List<string> labels = dataTest.GetCertificateLabels();
            //Assert.True(labels.Contains("Authentication"),"Find Authentication certificate");
        }
        [Test]
        public void GetCertificateRNFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] certificateRNFile = dataTest.GetCertificateRNFile();
            X509Certificate certificateRN;
            Assert.DoesNotThrow(delegate { certificateRN = new X509Certificate(certificateRNFile); });
            certificateRN = new X509Certificate(certificateRNFile);
            Assert.True(certificateRN.Issuer.Contains("Root"));
        }
        [Test]
        public void GetCertificateRootFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] certificateFile = dataTest.GetCertificateRootFile();
            X509Certificate certificateRoot;
            Assert.DoesNotThrow(delegate { certificateRoot = new X509Certificate(certificateFile); });
            certificateRoot = new X509Certificate(certificateFile);
            Assert.AreEqual(certificateRoot.Subject, certificateRoot.Issuer, "Should be a self-signed root certificate");
            Assert.True(certificateRoot.Subject.Contains("Root"));
        }
        [Test]
        public void GetPhotoFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            byte[] photoFile = dataTest.GetPhotoFile();
            Bitmap photo = new Bitmap(new MemoryStream(photoFile));
            Assert.AreEqual(140, photo.Width);
            Assert.AreEqual(200, photo.Height);
            
        }
        [Test]
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

