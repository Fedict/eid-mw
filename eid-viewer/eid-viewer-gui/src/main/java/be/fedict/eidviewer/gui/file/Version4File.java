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

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.text.ParseException;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;
import org.simpleframework.xml.Element;
import org.simpleframework.xml.Root;
import org.simpleframework.xml.Serializer;
import org.simpleframework.xml.core.Persister;

/**
 *
 * @author Frank Marien
 */

@Root (name="eid")
public class Version4File
{
    @Element (name="identity")
    private Version4FileIdentity        identity;
    @Element (name="card")
    private Version4FileCard            card;
    @Element (name="address")
    private Version4FileAddress         address;
    @Element (name="certificates")
    private Version4FileCertificates    certificates;

    public static void toXML(Version4File v4File, OutputStream outputStream) throws Exception
    {
        Serializer  serializer = new Persister();
                    serializer.write(v4File, outputStream);
    }

    public static Version4File fromXML(InputStream inputStream) throws Exception
    {
        Serializer  serializer = new Persister();
             return serializer.read(Version4File.class, inputStream);

    }

    public void fromIdentityAddressPhotoAndCertificates(Identity eidIdentity, Address eidAddress, byte[] photo,List<X509Certificate> authChain, List<X509Certificate> signChain, List<X509Certificate> rrnChain) throws Exception
    {
        identity    =new Version4FileIdentity(eidIdentity,photo);
        card        =new Version4FileCard(eidIdentity);
        address     =new Version4FileAddress(eidAddress);
        certificates=new Version4FileCertificates(authChain, signChain, rrnChain);
    }

    public Identity toIdentity() throws ParseException
    {
        Identity eidIdentity=new Identity();
        identity.toIdentity(eidIdentity);
        card.toIdentity(eidIdentity);
        return eidIdentity;
    }

    public Address toAddress()
    {
        Address eidAddress=new Address();
        address.toAddress(eidAddress);
        return eidAddress;
    }

    public List<X509Certificate> toAuthChain() throws CertificateException
    {
        return getCertificates().toAuthChain();
    }

    public List<X509Certificate> toSignChain() throws CertificateException
    {
        return getCertificates().toSignChain();
    }

    public List<X509Certificate> toRRNChain() throws CertificateException
    {
        return getCertificates().toRRNChain();
    }

    public void writeToZipOutputStream(ZipOutputStream zos) throws IOException
    {
        identity.writeToZipOutputStream(zos);
        card.writeToZipOutputStream(zos);
        address.writeToZipOutputStream(zos);
        certificates.writeToZipOutputStream(zos);
    }

    public byte[] toPhoto()
    {
        return identity.toPhoto();
    }

    public Version4FileAddress getAddress()
    {
        return address;
    }

    public void setAddress(Version4FileAddress address)
    {
        this.address = address;
    }

    public Version4FileCard getCard()
    {
        return card;
    }

    public void setCard(Version4FileCard card)
    {
        this.card = card;
    }

    public Version4FileIdentity getIdentity()
    {
        return identity;
    }

    public void setIdentity(Version4FileIdentity identity)
    {
        this.identity = identity;
    }

    public Version4FileCertificates getCertificates()
    {
        return certificates;
    }

    public void setCertificates(Version4FileCertificates certificates)
    {
        this.certificates = certificates;
    }

    public static void writeZipEntry(ZipOutputStream zos, Charset charSet, String label, String value) throws IOException
    {
        writeZipEntry(zos,label, value.getBytes(charSet));
    }

    public static void writeZipEntry(ZipOutputStream zos, String label, byte[] value) throws IOException
    {
        zos.putNextEntry(new ZipEntry(label));
        zos.write(value);
        zos.closeEntry();
    }
}
