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
import be.fedict.eidviewer.gui.EidData;
import be.fedict.eidviewer.gui.X509CertificateChainAndTrust;
import be.fedict.trust.client.TrustServiceDomains;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.LinkedList;
import java.util.List;
import java.util.zip.ZipEntry;

/**
 *
 * @author Frank Marien
 */
public class Version4EidFile
{
    private static final String ZIPFILE_ADDRESS_FILENAME                    = "address.ser";
    private static final String ZIPFILE_AUTHCERT_FILENAME                   = "authcert.der";
    private static final String ZIPFILE_CITIZENCERT_FILENAME                = "citicert.der";
    private static final String ZIPFILE_IDENTITY_FILENAME                   = "identity.ser";
    private static final String ZIPFILE_PHOTO_FILENAME                      = "photo.ser";
    private static final String ZIPFILE_ROOTCERT_FILENAME                   = "rootcert.der";
    private static final String ZIPFILE_SIGNCERT_FILENAME                   = "signcert.der";
    
    public static void load(File file, EidData controller) throws FileNotFoundException, CertificateException, IOException, ClassNotFoundException
    {
        CloseResistantZipInputStream    zipInputStream      = null;
        ObjectInputStream               objectInputStream   = null;
        X509Certificate                 rootCert            = null;
        X509Certificate                 citizenCert         = null;
        X509Certificate                 authenticationCert  = null;
        X509Certificate                 signingCert         = null;
        CertificateFactory              certificateFactory  = null;

        try
        {
            zipInputStream=new CloseResistantZipInputStream(new FileInputStream(file));
            certificateFactory = CertificateFactory.getInstance("X.509");
            ZipEntry entry;

            while((entry=zipInputStream.getNextEntry())!=null)
            {
                if(entry.getName().equals(ZIPFILE_IDENTITY_FILENAME))
                {
                    objectInputStream=new ObjectInputStream(zipInputStream);
                    controller.setIdentity((Identity)objectInputStream.readObject());
                    objectInputStream.close();

                }
                else if(entry.getName().equals(ZIPFILE_ADDRESS_FILENAME))
                {
                    objectInputStream=new ObjectInputStream(zipInputStream);
                    controller.setAddress((Address)objectInputStream.readObject());
                    objectInputStream.close();

                }
                else if(entry.getName().equals(ZIPFILE_PHOTO_FILENAME))
                {
                    objectInputStream=new ObjectInputStream(zipInputStream);
                    controller.setPhoto((byte[])objectInputStream.readObject());
                    objectInputStream.close();
                }
                else if(entry.getName().equals(ZIPFILE_AUTHCERT_FILENAME))
                {
                    authenticationCert=(X509Certificate) certificateFactory.generateCertificate(zipInputStream);
                }
                else if(entry.getName().equals(ZIPFILE_SIGNCERT_FILENAME))
                {
                    signingCert=(X509Certificate) certificateFactory.generateCertificate(zipInputStream);
                }
                else if(entry.getName().equals(ZIPFILE_CITIZENCERT_FILENAME))
                {
                    citizenCert=(X509Certificate) certificateFactory.generateCertificate(zipInputStream);
                }
                else if(entry.getName().equals(ZIPFILE_ROOTCERT_FILENAME))
                {
                    rootCert=(X509Certificate) certificateFactory.generateCertificate(zipInputStream);
                }

                zipInputStream.closeEntry();
            }

            if(rootCert!=null && citizenCert!=null)
            {
                if(authenticationCert!=null)
                {
                    List authChain=new LinkedList<X509Certificate>();
                         authChain.add(authenticationCert);
                         authChain.add(citizenCert);
                         authChain.add(rootCert);
                    controller.setAuthCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, authChain));
                }

                if(signingCert!=null)
                {
                    List signChain=new LinkedList<X509Certificate>();
                         signChain.add(signingCert);
                         signChain.add(citizenCert);
                         signChain.add(rootCert);

                    controller.setSignCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, signChain));
                }
            }
        }
        finally
        {
            zipInputStream.setCloseAllowed(true);
            zipInputStream.close();
        }
    }

    
    public static void save(File file, EidData eidData) throws IOException, CertificateEncodingException
    {
        CloseResistantZipOutputStream   zipOutputStream     =null;
        ObjectOutputStream              objectOutputStream  =null;

        try
        {
            zipOutputStream = new CloseResistantZipOutputStream(new FileOutputStream(file));

            if(eidData.hasIdentity())
            {
                zipOutputStream.putNextEntry(new ZipEntry("nationalnumber"));
                zipOutputStream.write(eidData.getIdentity().getNationalNumber().getBytes("utf-8"));
                zipOutputStream.closeEntry();
                
                zipOutputStream.putNextEntry(new ZipEntry("nationalnumber"));
                zipOutputStream.write(eidData.getIdentity().getNationalNumber().getBytes("utf-8"));
                zipOutputStream.closeEntry();
            }

            if(eidData.hasAddress())
            {
                zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_ADDRESS_FILENAME));
                objectOutputStream = new ObjectOutputStream(zipOutputStream);
                objectOutputStream.writeObject(eidData.getAddress());
                objectOutputStream.close();
                zipOutputStream.closeEntry();
            }

            if(eidData.hasPhoto())
            {
                zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_PHOTO_FILENAME));
                objectOutputStream = new ObjectOutputStream(zipOutputStream);
                objectOutputStream.writeObject(eidData.getPhoto());
                objectOutputStream.close();
                zipOutputStream.closeEntry();
            }

            if(eidData.hasAuthCertChain())
            {
                List<X509Certificate> authChain=eidData.getAuthCertChain().getCertificates();
                if(authChain.size()==3)
                {
                    X509Certificate authCert=authChain.get(0);
                    zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_AUTHCERT_FILENAME));
                    byte[] derEncoded=authCert.getEncoded();
                    zipOutputStream.write(derEncoded);
                    zipOutputStream.closeEntry();

                    authCert=authChain.get(1);
                    zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_CITIZENCERT_FILENAME));
                    derEncoded=authCert.getEncoded();
                    zipOutputStream.write(derEncoded);
                    zipOutputStream.closeEntry();

                    authCert=authChain.get(2);
                    zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_ROOTCERT_FILENAME));
                    derEncoded=authCert.getEncoded();
                    zipOutputStream.write(derEncoded);
                    zipOutputStream.closeEntry();
                }
            }

            if(eidData.hasSignCertChain())
            {
                List<X509Certificate> signChain=eidData.getSignCertChain().getCertificates();
                if(signChain.size()==3)
                {
                    X509Certificate signCert=signChain.get(0);
                    zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_SIGNCERT_FILENAME));
                    byte[] derEncoded=signCert.getEncoded();
                    zipOutputStream.write(derEncoded);
                    zipOutputStream.closeEntry();
                }
            }
        }
        finally
        {
            zipOutputStream.setCloseAllowed(true);
            zipOutputStream.close();
        }
    }
}
