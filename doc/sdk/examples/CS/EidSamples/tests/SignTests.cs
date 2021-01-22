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
        /// <summary> 
        /// Tests the creation of EC digital signatures, using the card key
        /// Only available for applet 1.8 cards
        /// </summary>
        [TestMethod]
        public void ChallengeCard()
        {
            // Sign
            Sign signTest = new Sign("beidpkcs11.dll");
            byte[] testdata = new byte[] {  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,};
            byte[] signeddata = signTest.DoChallenge(testdata);
            Assert.IsNotNull(signeddata);

            // Verification
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            Assert.IsTrue(integrityTest.VerifyChallenge(testdata, signeddata, dataTest.GetPublicKey("Card")));
        }

    }

}

