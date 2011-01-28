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

import be.fedict.eidviewer.gui.EidData;
import be.fedict.eidviewer.gui.X509CertificateChainAndTrust;
import be.fedict.trust.client.TrustServiceDomains;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.cert.CertificateException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.ZipOutputStream;

/**
 *
 * @author Frank Marien
 */
public class EidFiles
{
    private static final Logger logger = Logger.getLogger(EidFiles.class.getName());
    
    public static void loadFromFile(File file, EidData eidData) throws Exception
    {
        try
        {
            if(file.getName().toLowerCase().endsWith(".eid"))
            {
                Version35EidFile.load(file, eidData);
            }
            else if(file.getName().toLowerCase().endsWith(".csv"))
            {
                Version35CSVFile.load(file, eidData);
            }
            else if(file.getName().toLowerCase().endsWith(".xml"))
            {
                loadFromXMLFile(file, eidData);
            }
        }
        catch (CertificateException ex)
        {
            Logger.getLogger(EidFiles.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch (FileNotFoundException ex)
        {
            Logger.getLogger(EidFiles.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch (IOException ex)
        {
            Logger.getLogger(EidFiles.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public static void loadFromXMLFile(File file, EidData eidData) throws FileNotFoundException, IOException, Exception
    {
        switch(getXMLFileVersion(file))
        {
            case 4:
            logger.fine("parsing as 4.0 .XML file");
            Version4File v4File=Version4File.fromXML(new FileInputStream(file));
            logger.fine("parsed as 4.0 .XML file");
            eidData.setIdentity(v4File.toIdentity());
            eidData.setAddress(v4File.toAddress());
            eidData.setPhoto(v4File.toPhoto());
            eidData.setAuthCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, v4File.toAuthChain()));
            eidData.setSignCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, v4File.toSignChain()));
            eidData.setRRNCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NATIONAL_REGISTRY_TRUST_DOMAIN, v4File.toRRNChain()));
            break;

            case 3:
            logger.fine("parsing as 3.5.X .XML file");
            Version35XMLFile v35xmlFile = new Version35XMLFile(eidData);
            v35xmlFile.load(file);
            logger.fine("3.5.x XML data loaded ok");
            break;
        }
    }

    public static void saveToXMLFile(File file, EidData eidData)
    {
        try
        {
            Version4File version4file=new Version4File();
                         version4file.fromIdentityAddressPhotoAndCertificates(  eidData.getIdentity(),eidData.getAddress(),eidData.getPhoto(),
                                                                                eidData.getAuthCertChain().getCertificates(),
                                                                                eidData.getSignCertChain().getCertificates(),
                                                                                eidData.getRRNCertChain().getCertificates());
                         Version4File.toXML(version4file, new FileOutputStream(file));
        }
        catch (Exception ex)
        {
            logger.log(Level.SEVERE, "Failed To Save To Version 4.x.x XML-Based .eid File", ex);
        }
    }

    public static void saveToZIPFile(File file, EidData eidData)
    {
        try
        {
            ZipOutputStream zipOutputStream = new ZipOutputStream(new FileOutputStream(file));
            Version4File    version4file=new Version4File();
                            version4file.fromIdentityAddressPhotoAndCertificates(eidData.getIdentity(),eidData.getAddress(),eidData.getPhoto(),
                                                                                eidData.getAuthCertChain().getCertificates(),
                                                                                eidData.getSignCertChain().getCertificates(),
                                                                                eidData.getRRNCertChain().getCertificates());
                            version4file.writeToZipOutputStream(zipOutputStream);
                            zipOutputStream.flush();
                            zipOutputStream.close();

        }
        catch (Exception ex)
        {
            logger.log(Level.SEVERE, "Failed To Save To Version 4.x.x ZIP-Based .eid File", ex);
        }
    }

    public static int getXMLFileVersion(File file)
    {
        int             version =0;
        FileInputStream fis     =null;

        try
        {
            if (!file.canRead())
                return 0;

            if ((!file.getName().endsWith(".xml")) && (!file.getName().endsWith(".XML")))
                return 0;

            byte[] buffer = new byte[512];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);
            if (headStr.contains("<eid>"))
            {
                version = 4;
                logger.finest("Found Version 4.x XML file");
            }
            else if (headStr.contains("<BelPicDirectory>"))
            {
                version = 3;
                logger.finest("Found Version 3.x.x XML file");
            }
            return version;
        }
        catch (IOException ex)
        {
            logger.log(Level.SEVERE, "Failed to get XML Version", ex);
        }
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                logger.log(Level.SEVERE, "Failed to close XML File", ex);
            }
        }

        return version;
    }

    public static int getCSVFileVersion(File file)
    {
        int             version = 0;
        FileInputStream fis     = null;

        try
        {
            if (!file.canRead())
                return 0;

            if ((!file.getName().endsWith(".csv")) && (!file.getName().endsWith(".CSV")))
                return 0;

            byte[] buffer = new byte[16];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);
            String[] fields = headStr.split(";");
            if (fields.length >= 2 && fields[1].equalsIgnoreCase("eid"))
            {
                try
                {
                    version = Integer.parseInt(fields[0]);
                }
                catch (NumberFormatException nfe)
                {
                    logger.log(Level.FINE, "CSV File Failed To Parse Version", nfe);
                }
            }
            return version;
        }
        catch (IOException ex)
        {
            logger.log(Level.SEVERE, "Failed to get determine CSV Version", ex);
        }
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                logger.log(Level.SEVERE, "Failed to close CSV file", ex);
            }
        }

        return version;
    }

    public static boolean isTLVEidFile(File file)
    {
        FileInputStream fis = null;
        boolean isTLVEid = false;

        if (!file.canRead())
            return false;

        if ((!file.getName().endsWith(".eid")) && (!file.getName().endsWith(".EID")))
            return false;

        try
        {
            byte[] buffer = new byte[128];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);

            isTLVEid = (buffer[0] == 0 && buffer[1] == 1 && headStr != null && headStr.length() > 0 && headStr.contains("Belgium Root CA"));
        }
        catch (IOException ex)
        {
            logger.log(Level.SEVERE, "Failed to get determine TLV format", ex);
        }
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                logger.log(Level.SEVERE, "Failed to close TLV file", ex);
            }
        }

        return isTLVEid;
    }
}
