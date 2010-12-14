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

import be.fedict.eidviewer.gui.helper.X509Utilities;
import java.security.Principal;
import java.security.cert.X509Certificate;
import java.util.List;

public class X509CertificateAndTrust
{

    private X509Certificate certificate;
    private String          trustDomain;
    private List<String>    invalidReasons;
    private Exception       validationException;
    private boolean         validating, validated, trusted;

    public X509CertificateAndTrust(X509Certificate certificate, String trustDomain)
    {
        this.certificate = certificate;
        this.trustDomain = trustDomain;
    }

    public X509Certificate getCertificate()
    {
        return certificate;
    }

    public void setCertificate(X509Certificate certificate)
    {
        this.certificate = certificate;
    }

    public String getTrustDomain()
    {
        return trustDomain;
    }

    public void setTrustDomain(String trustDomain)
    {
        this.trustDomain = trustDomain;
    }

    public boolean isValidating()
    {
        return validating;
    }

    public void setValidating(boolean validating)
    {
        this.validating = validating;
    }

    public boolean isTrusted()
    {
        return trusted;
    }

    public void setTrusted(boolean trusted)
    {
        this.trusted = trusted;
    }

    public boolean isValidated()
    {
        return validated;
    }

    public void setValidated(boolean validated)
    {
        this.validated = validated;
    }

    public Principal getSubjectDN()
    {
        return certificate.getSubjectDN();
    }

    public Principal getIssuerDN()
    {
        return certificate.getIssuerDN();
    }

    public List<String> getInvalidReasons()
    {
        return invalidReasons;
    }

    public void setInvalidReasons(List<String> invalidReasons)
    {
        this.invalidReasons = invalidReasons;
    }

    public Exception getValidationException()
    {
        return validationException;
    }

    public void setValidationException(Exception validationException)
    {
        this.validationException = validationException;
    }

    @Override
    public String toString()
    {
        return X509Utilities.getCN(certificate);
    }
}
