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
using System;
namespace EidSamples.tests
{
    /// <summary> 
    /// Tests the integrity of the data files that are retrieved from the eID card
    /// </summary>
    [TestClass]
    public class IntegrityTests
    {
        /// <summary> 
        /// Tests if integrity test fails when no certificateRRN is supplied
        /// </summary>
        [TestMethod]
        public void IntegrityFails()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRRN = null;
            Assert.IsFalse(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        /// <summary> 
        /// Tests the integrity of the identity data file that is retrieved from the eID card
        /// </summary>
        [TestMethod]
        public void IntegrityIdentityFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRRN = dataTest.GetCertificateRNFile();
            Assert.IsTrue(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        /// <summary> 
        /// Tests if integrity test fails when address file signature is used to check validity of id file
        /// </summary>
        [TestMethod]
        public void IntegrityIdentityFileWrongSignature()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetAddressSignatureFile();
            byte[] certificateRRN = dataTest.GetCertificateRNFile();
            Assert.IsFalse(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        /// <summary> 
        /// Tests if integrity test fails when the root certificate is used to check file integrity of the id data file
        /// </summary>
        [TestMethod]
        public void IntegrityIdentityFileWrongCertificate()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRoot = dataTest.GetCertificateRootFile();
            Assert.IsFalse(integrityTest.Verify(idFile, idSignatureFile, certificateRoot));
        }
        /// <summary> 
        /// Tests the integrity of the address data file that is retrieved from the eID card
        /// </summary>
        [TestMethod]
        public void IntegrityAddressFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] addressFile = trimRight(dataTest.GetAddressFile());
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] concatFiles = new byte[addressFile.Length + idSignatureFile.Length];
            Array.Copy(addressFile, 0, concatFiles, 0, addressFile.Length);
            Array.Copy(idSignatureFile, 0, concatFiles, addressFile.Length, idSignatureFile.Length);
            byte[] addressSignatureFile = dataTest.GetAddressSignatureFile();
            byte[] certificateRRN = dataTest.GetCertificateRNFile();
            Assert.IsTrue(integrityTest.Verify(concatFiles, addressSignatureFile, certificateRRN));
        }
        private byte[] trimRight(byte[] addressFile)
        {
            int idx;
            for (idx = 0; idx < addressFile.Length; idx++)
            {
                if (0 == addressFile[idx])
                {
                    break;
                }
            }
            byte[] result = new byte[idx];
            Array.Copy(addressFile, 0, result, 0, idx);
            return result;
        }

    }

}

