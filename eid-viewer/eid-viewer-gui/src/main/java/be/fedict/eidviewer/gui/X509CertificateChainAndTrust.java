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

import be.fedict.eidviewer.gui.helper.IdFormatHelper;
import be.fedict.trust.client.jaxb.xades132.RevocationValuesType;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Frank Marien
 */
public class X509CertificateChainAndTrust
{
    private List<X509Certificate>           certificates;
    private List<X509CertificateAndTrust>   certificatesAndTrusts;
    private String                          trustDomain;
    private Exception                       validationException;
    private RevocationValuesType            revocationValues;
    private List<String>                    invalidReasons;
    private boolean                         validating, validated,trusted;

    public X509CertificateChainAndTrust(String trustDomain, List<X509Certificate> certificates)
    {
        this.trustDomain=trustDomain;
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

    public boolean isValidating()
    {
        return validating;
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
        this.validating=false;
        this.validated=true;
        this.trusted=false;
        propagate();
    }

    void setTrustServiceException(Exception trustServiceException)
    {
        this.validationException = trustServiceException;
        this.validating=false;
        this.validated=false;
        this.trusted=false;
        propagate();
    }

    public void setInvalidReasons(List<String> invalidReasons)
    {
        this.invalidReasons = invalidReasons;
        this.validating=false;
        this.validated=true;
        this.trusted=false;
        propagate();
    }

    public void setRevocationValues(RevocationValuesType revocationValues)
    {
        this.revocationValues=revocationValues;
        this.validating=false;
        this.validated=true;
        this.trusted=false;
        propagate();
    }

    public void setValidating()
    {
        this.validating=true;
        propagate();
    }
    
    public void setTrusted()
    {
        this.validating=false;
        this.validated=true;
        this.trusted=true;
        propagate();
    }

    private void propagate()
    {
        for(X509CertificateAndTrust certificate : certificatesAndTrusts)
        {
            certificate.setValidating(this.isValidating());
            certificate.setValidated(this.isValidated());
            certificate.setValidationException(this.getValidationException());
            certificate.setInvalidReasons(this.getInvalidReasons());
            certificate.setTrusted(this.isTrusted());
        }
    }

    @Override
    public String toString()
    {
        return IdFormatHelper.join(certificatesAndTrusts," signed by ");
    }
}
