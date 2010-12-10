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
package be.fedict.eidviewer.gui;

import be.fedict.trust.client.jaxb.xades132.RevocationValuesType;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author frank
 */
public class X509CertificateChainAndTrust
{
    private List<X509Certificate>           certificates;
    private List<X509CertificateAndTrust>   certificatesAndTrusts;
    private String                          trustDomain;
    private Exception                       validationException;
    private RevocationValuesType            revocationValues;
    private List<String>                    invalidReasons;
    private boolean                         validated,trusted;

    public X509CertificateChainAndTrust(String trustDomain, List<X509Certificate> certificates)
    {
        this.certificates=certificates;
        this.certificatesAndTrusts=new ArrayList<X509CertificateAndTrust>();
        for(X509Certificate certificate : certificates)
            this.certificatesAndTrusts.add(new X509CertificateAndTrust(certificate, trustDomain));
        this.validated = false;
        this.trusted = false;
    }

    public List<X509Certificate> getCertificates()
    {
        return certificates;
    }

     public List<X509CertificateAndTrust> getCertificatesAndTrusts()
    {
        return certificatesAndTrusts;
    }

    public Exception getValidationException()
    {
        return validationException;
    }

    public List<String> getInvalidReasons()
    {
        return invalidReasons;
    }

    public RevocationValuesType getRevocationValues()
    {
        return revocationValues;
    }

    public boolean isTrusted()
    {
        return trusted;
    }

    public boolean isValidated()
    {
        return validated;
    }
    
    public String getTrustDomain()
    {
        return trustDomain;
    }

    /* ------------------------------------------------------------------------------ */

    public void setValidationException(Exception validationException)
    {
        this.validationException = validationException;
        this.validated=true;
        this.trusted=false;
        for(X509CertificateAndTrust certificate : certificatesAndTrusts)
        {
            certificate.setValidated(true);
            certificate.setTrusted(false);
        }
    }

    public void setInvalidReasons(List<String> invalidReasons)
    {
        this.invalidReasons = invalidReasons;
        this.validated=true;
        this.trusted=false;
        for(X509CertificateAndTrust certificate : certificatesAndTrusts)
        {
            certificate.setValidated(true);
            certificate.setTrusted(false);
        }
    }

    public void setRevocationValues(RevocationValuesType revocationValues)
    {
        this.revocationValues = revocationValues;
        this.validated=true;
        for(X509CertificateAndTrust certificate : certificatesAndTrusts)
        {
            certificate.setValidated(true);
            certificate.setTrusted(false);
        }
    }
    
    public void setTrusted()
    {
        this.validated=true;
        this.trusted = true;
        for(X509CertificateAndTrust certificate : certificatesAndTrusts)
        {
            certificate.setValidated(true);
            certificate.setTrusted(true);
        }
    }
}
