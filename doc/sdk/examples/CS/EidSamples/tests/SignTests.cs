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
    /// Tests the creation of digital signatures
    /// </summary>
    [TestClass]
    public class SignTests
    {
        /// <summary> 
        /// Tests the creation of digital signatures, using the authentication key
        /// </summary>
        [TestMethod]
        public void SignAuthentication()
        {
            // Sign
            Sign signTest = new Sign("beidpkcs11.dll");
            byte[] testdata = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
            byte[] signeddata = signTest.DoSign(testdata, "Authentication");
            Assert.IsNotNull(signeddata);
            // Verification
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            Assert.IsTrue(integrityTest.Verify(testdata, signeddata,
                dataTest.GetCertificateAuthenticationFile()));
            //Assert.False(integrityTest.Verify(testdata, signeddata,
            //    dataTest.GetCertificateSignatureFile()));

        }
        /// <summary> 
        /// Tests the creation of digital signatures, using the signature key
        /// </summary>
        [TestMethod]
        public void SignSignature()
        {
            // Sign
            Sign signTest = new Sign("beidpkcs11.dll");
            byte[] testdata = new byte[] { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
            byte[] signeddata = signTest.DoSign(testdata, "Signature");
            Assert.IsNotNull(signeddata);
            // Verification
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            Assert.IsFalse(integrityTest.Verify(testdata, signeddata,
                dataTest.GetCertificateAuthenticationFile()));
            Assert.IsTrue(integrityTest.Verify(testdata, signeddata,
                dataTest.GetCertificateSignatureFile()));

        }

    }

}

