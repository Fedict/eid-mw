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
using System.Collections.Generic;
namespace EidSamples.tests
{
    /// <summary> 
    /// checks the validity of the certificate chain
    /// </summary> 
    [TestClass]
    public class CertificateValidityTests
    {
        /// <summary> 
        /// checks the validity of the authentication certificate chain
        /// </summary> 
        [TestMethod]
        public void ValidityAuthenticationChain()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            List<byte[]> caCerts = new List <byte[]>();
            caCerts.Add(dataTest.GetCertificateCAFile());
            //string leaf_path = "C:\\Users\\Frederik\\certs_fail\\ronald_asselberghs_signature.pem";//fred_auth.cer";//ronald_asselberghs_authentication.pem
            //caCerts.Add(System.IO.File.ReadAllBytes(leaf_path));

            string CA_path = "C:\\Users\\Frederik\\certs_fail\\citizen_ct.pem";//"C:\\Users\\Frederik\\certs_fail\\fve_citizen_ca.pem";
            
            Assert.IsTrue(integrityTest.CheckCertificateChain(
                caCerts,
                System.IO.File.ReadAllBytes(CA_path)));

            //caCerts.Add()
            //Assert.True(integrityTest.CheckCertificateChain(
            //    caCerts,
            //    dataTest.GetCertificateAuthenticationFile()));
        }
        /// <summary> 
        /// checks the validity of the signature certificate chain
        /// </summary> 
        [TestMethod]
        public void ValiditySignatureChain()
        {
            ReadData dataTest = new ReadData("beidpkcs11.dll");
            Integrity integrityTest = new Integrity();
            List<byte[]> caCerts = new List<byte[]>();
            caCerts.Add(dataTest.GetCertificateCAFile());

            Assert.IsTrue(integrityTest.CheckCertificateChain(
                caCerts,
                dataTest.GetCertificateSignatureFile()));
        }
    }

}

