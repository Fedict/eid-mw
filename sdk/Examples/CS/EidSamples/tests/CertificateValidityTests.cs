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
using System.Collections.Generic;
namespace EidSamples.tests
{
    [TestFixture]
    public class CertificateValidityTests
    {
        [Test]
        public void ValidityAuthenticationChain()
        {
            ReadData dataTest = new ReadData("beidpkcs11D.dll");
            Integrity integrityTest = new Integrity();
            List<byte[]> caCerts = new List <byte[]>();
            caCerts.Add(dataTest.GetCertificateCAFile());
            Assert.True(integrityTest.CheckCertificateChain(
                caCerts,
                dataTest.GetCertificateAuthenticationFile()));
        }
        [Test]
        public void ValiditySignatureChain()
        {
            ReadData dataTest = new ReadData("beidpkcs11D.dll");
            Integrity integrityTest = new Integrity();
            List<byte[]> caCerts = new List<byte[]>();
            caCerts.Add(dataTest.GetCertificateCAFile());

            Assert.True(integrityTest.CheckCertificateChain(
                caCerts,
                dataTest.GetCertificateSignatureFile()));
        }
    }

}

