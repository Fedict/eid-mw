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
using System;
namespace EidSamples.tests
{
    [TestFixture]
    public class IntegrityTests
    {
        [Test]
        public void IntegrityFails()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRRN = null;
            Assert.False(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        [Test]
        public void IntegrityIdentityFile()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRRN = dataTest.GetCertificateRNFile();
            Assert.True(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        [Test]
        public void IntegrityIdentityFileWrongSignature()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetAddressSignatureFile();
            byte[] certificateRRN = dataTest.GetCertificateRNFile();
            Assert.False(integrityTest.Verify(idFile, idSignatureFile, certificateRRN));
        }
        [Test]
        public void IntegrityIdentityFileWrongCertificate()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            byte[] idFile = dataTest.GetIdFile();
            byte[] idSignatureFile = dataTest.GetIdSignatureFile();
            byte[] certificateRoot = dataTest.GetCertificateRootFile();
            Assert.False(integrityTest.Verify(idFile, idSignatureFile, certificateRoot));
        }
        [Test]
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
            Assert.True(integrityTest.Verify(concatFiles, addressSignatureFile, certificateRRN));
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

