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

package be.fedict.eidviewer.gui.file;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.charset.Charset;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.LinkedList;
import java.util.List;
import java.util.zip.ZipOutputStream;
import org.apache.commons.codec.binary.Base64;
import org.simpleframework.xml.Element;

/**
 *
 * @author Frank Marien
 */
public final class Version4FileCertificates
{
    @Element(name="root",required=false)
    private String rootCertificate;
    @Element(name="citizenca",required=false)
    private String citizenCACertificate;
    @Element(name="authentication",required=false)
    private String authenticationCertificate;
    @Element(name="signing",required=false)
    private String signingCertificate;
    @Element(name="rrn",required=false)
    private String rrnCertificate;

    public Version4FileCertificates(List<X509Certificate> authChain, List<X509Certificate> signChain, List<X509Certificate> rrnChain) throws CertificateEncodingException
    {
        super();
        fromCertChains(authChain, signChain,rrnChain);
    }

    public Version4FileCertificates()
    {
        super();
    }

    public void fromCertChains(List<X509Certificate> authChain, List<X509Certificate> signChain, List<X509Certificate> rrnChain) throws CertificateEncodingException
    {
        if(authChain!=null && authChain.size()==3)
        {   
            setAuthenticationCertificate(new String(Base64.encodeBase64(authChain.get(0).getEncoded(), false, false, 4096)).trim());
            setCitizenCACertificate     (new String(Base64.encodeBase64(authChain.get(1).getEncoded(), false, false, 4096)).trim());
            setRootCertificate          (new String(Base64.encodeBase64(authChain.get(2).getEncoded(), false, false, 4096)).trim());
        }

        if(signChain!=null && signChain.size()==3)
            setSigningCertificate(new String(Base64.encodeBase64(signChain.get(0).getEncoded(), false, false, 4096)).trim());

        if(rrnChain!=null && rrnChain.size()==2)
            setRRNCertificate(new String(Base64.encodeBase64(rrnChain.get(0).getEncoded(), false, false, 4096)).trim());
    }

    public List<X509Certificate> toAuthChain() throws CertificateException
    {
        CertificateFactory certificateFactory = null;
        X509Certificate rootCert = null;
        X509Certificate citizenCert = null;
        X509Certificate authenticationCert = null;
        List authChain=null;
        
        if(getRootCertificate()==null || getCitizenCACertificate()==null || getAuthenticationCertificate()==null)
            return null;

        certificateFactory = CertificateFactory.getInstance("X.509");

       rootCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getRootCertificate())));
       citizenCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getCitizenCACertificate())));
       authenticationCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getAuthenticationCertificate())));
    
       authChain = new LinkedList<X509Certificate>();
       authChain.add(authenticationCert);
       authChain.add(citizenCert);
       authChain.add(rootCert);

        return authChain;
    }

    public List<X509Certificate> toSignChain() throws CertificateException
    {
        CertificateFactory      certificateFactory = null;
        X509Certificate         rootCert = null;
        X509Certificate         citizenCert = null;
        X509Certificate         signingCert = null;
        List                    signChain=null;

        if(getRootCertificate()==null || getCitizenCACertificate()==null || getSigningCertificate()==null)
            return null;

        certificateFactory = CertificateFactory.getInstance("X.509");

       rootCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getRootCertificate())));
       citizenCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getCitizenCACertificate())));
       signingCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getSigningCertificate())));

       signChain = new LinkedList<X509Certificate>();
       signChain.add(signingCert);
       signChain.add(citizenCert);
       signChain.add(rootCert);

       return signChain;
    }

    public List<X509Certificate> toRRNChain() throws CertificateException
    {
        CertificateFactory      certificateFactory = null;
        X509Certificate         rootCert = null;
        X509Certificate         rrnCert = null;
        List                    rrnChain=null;

        if(getRootCertificate()==null || getCitizenCACertificate()==null || getSigningCertificate()==null || getRRNCertificate()==null)
            return null;

       certificateFactory = CertificateFactory.getInstance("X.509");

       rootCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getRootCertificate())));
       rrnCert=(X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(getRRNCertificate())));

       rrnChain = new LinkedList<X509Certificate>();
       rrnChain.add(rrnCert);
       rrnChain.add(rootCert);

       return rrnChain;
    }

    public void writeToZipOutputStream(ZipOutputStream zos) throws IOException
    {
        Version4File.writeZipEntry(zos,"rootcertificate",Base64.decodeBase64(getRootCertificate()));
        Version4File.writeZipEntry(zos,"cacertificate",Base64.decodeBase64(getCitizenCACertificate()));
        Version4File.writeZipEntry(zos,"authenticationcertificate",Base64.decodeBase64(getAuthenticationCertificate()));
        Version4File.writeZipEntry(zos,"signingcertificate",Base64.decodeBase64(getSigningCertificate())); 
    }

    public String getAuthenticationCertificate()
    {
        return authenticationCertificate;
    }

    public void setAuthenticationCertificate(String authenticationCertificate)
    {
        this.authenticationCertificate = authenticationCertificate;
    }

    public String getCitizenCACertificate()
    {
        return citizenCACertificate;
    }

    public void setCitizenCACertificate(String citizenCACertificate)
    {
        this.citizenCACertificate = citizenCACertificate;
    }

    public String getRootCertificate()
    {
        return rootCertificate;
    }

    public void setRootCertificate(String rootCertificate)
    {
        this.rootCertificate = rootCertificate;
    }

    public String getRRNCertificate()
    {
        return rrnCertificate;
    }

    public void setRRNCertificate(String rrnCertificate)
    {
        this.rrnCertificate = rrnCertificate;
    }

    public String getSigningCertificate()
    {
        return signingCertificate;
    }

    public void setSigningCertificate(String signingCertificate)
    {
        this.signingCertificate = signingCertificate;
    }
}
