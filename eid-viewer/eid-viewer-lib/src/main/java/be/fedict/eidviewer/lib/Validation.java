/*
 * eID Middleware Project.
 * Copyright (C) 2010 FedICT.
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
 */

package be.fedict.eidviewer.lib;

import java.security.Signature;
import java.security.cert.X509Certificate;

/**
 *
 * @author Frank Marien
 */
public class Validation
{
    public static boolean isValidSignature(X509Certificate certificate, byte[] data, byte[] data2, byte[] signature )
    {
        try
        {
            Signature   verifier = Signature.getInstance("SHA1withRSA");
                        verifier.initVerify(certificate);
                        verifier.update(data);
                        if(data2!=null)
                            verifier.update(data2);      
                 return verifier.verify(signature);
        }
        catch (Exception e)
        {
            return false;
        }
    }

    public static boolean isValidSignature(X509Certificate certificate, byte[] data, byte[] signature )
    {
        return isValidSignature(certificate, data, null, signature);
    }
}
