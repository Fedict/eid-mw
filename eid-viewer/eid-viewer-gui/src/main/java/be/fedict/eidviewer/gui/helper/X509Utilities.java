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

package be.fedict.eidviewer.gui.helper;

import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;


public class X509Utilities
{
    private static final int            CONSTRAINT_DIGITALSIGNATURE=0;
    private static final List<String>   keyUsageStringNames;

    static
    {
        keyUsageStringNames=new ArrayList(9);
        keyUsageStringNames.add("constraint_digitalSignature");
        keyUsageStringNames.add("constraint_nonRepudiation");
        keyUsageStringNames.add("constraint_keyEncipherment");
        keyUsageStringNames.add("constraint_dataEncipherment");
        keyUsageStringNames.add("constraint_keyAgreement");
        keyUsageStringNames.add("constraint_keyCertSign");
        keyUsageStringNames.add("constraint_cRLSignKey");
        keyUsageStringNames.add("constraint_encipherOnly");
        keyUsageStringNames.add("constraint_decipherOnly");
    }
    
    public static boolean isSelfSigned(X509Certificate certificate)
    {
        return certificate.getIssuerDN().equals(certificate.getSubjectDN());
    }

    public static boolean isCertificateAuthority(X509Certificate certificate)
    {
        return (certificate.getBasicConstraints()!=-1) && isSelfSigned(certificate);
    }

    public static String getCN(X509Certificate certificate)
    {
        String[] dn=certificate.getSubjectDN().getName().split("\\s*,\\s*");
        for(String dnPart : dn)
        {
            String[] labelValue=dnPart.trim().split("=");
            if(labelValue.length==2 && labelValue[0].equalsIgnoreCase("CN"))
                return labelValue[1].trim();
        }
        return null;
    }

    public static List<String> getKeyUsageStrings(ResourceBundle bundle, boolean[] keyUsage)
    {
        List<String> uses=new ArrayList<String>(9);
        for(int i=0;i<keyUsage.length;i++)
            if(keyUsage[i])
                uses.add(bundle.getString(keyUsageStringNames.get(i)));
        return uses;
    }

    public static boolean keyHasDigitalSignatureConstraint(X509Certificate certificate)
    {
        return certificate.getKeyUsage()[CONSTRAINT_DIGITALSIGNATURE];
    }
}
