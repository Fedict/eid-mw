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
using System.Collections.Generic;
using System;
using System.Security.Cryptography.X509Certificates;
using EidSamples.tests;

namespace EidSamples
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Diagnostics.Stopwatch duurtijd = new System.Diagnostics.Stopwatch();
            duurtijd.Start();
            DataTests dt = new DataTests();
            //dt.StoreCertificateRNFile();
            dt.GetSurname();
            dt.GetCertificateLabels();
            duurtijd.Stop();
            Console.WriteLine(duurtijd.ElapsedMilliseconds.ToString());
        }
    }
}
