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

namespace EidSamples.tests
{
    [TestFixture]
    public class DataTests
    {
        [Test]
        public void GetSlotDescription()
        {
            ReadData dataTest = new ReadData();
            Assert.AreEqual("ACS CCID USB Reader 0", dataTest.GetSlotDescription());
        }
        [Test]
        public void GetTokenInfoLabel()
        {
            ReadData dataTest = new ReadData();
            Assert.AreEqual("BELPIC", dataTest.GetTokenInfoLabel().Trim());
        }

        [Test]
        public void GetSurname()
        {
            ReadData dataTest = new ReadData();
            Assert.AreEqual("SPECIMEN", dataTest.GetSurname());
        }
        [Test]
        public void GetDateOfBirth()
        {
            ReadData dataTest = new ReadData();
            Assert.AreEqual("01 JAN 1971", dataTest.GetDateOfBirth());
        }

        [Test]
        public void GetIdFile()
        {
            ReadData dataTest = new ReadData();
            byte [] idFile = dataTest.GetIdFile();
            int i = 0;
            
            // poor man's tlv parser...
            // we'll check the first two tag fields (01 and 02)
            Assert.AreEqual(0x01, idFile[i++]); // Tag
            i += idFile[i++];                   // Length - skip value
            Assert.AreEqual(0x02, idFile[i++]); // Tag
        }
        [Test]
        public void GetCertificateLabels()
        {
            ReadData dataTest = new ReadData();
            List<string> labels = dataTest.GetCertificateLabels();
            Assert.True(labels.Contains("Authentication"),"Find Authentication certificate");
            Assert.True(labels.Contains("RRN"), "Find RRN certificate");
        }

    }

}

