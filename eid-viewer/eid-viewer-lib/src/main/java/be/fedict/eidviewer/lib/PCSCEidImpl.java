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

import java.lang.reflect.Constructor;
import be.fedict.eid.applet.sc.PcscEidSpi;
import be.fedict.eid.applet.Messages;
import be.fedict.eid.applet.View;
import be.fedict.eid.applet.sc.PcscEid;
import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import java.security.cert.X509Certificate;
import java.util.List;
import java.util.Observer;
import be.fedict.eid.applet.service.impl.tlv.TlvParser;
import java.awt.Image;
import java.io.ByteArrayInputStream;
import java.security.cert.CertificateFactory;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

public class PCSCEidImpl implements Eid
{
    private static final Logger logger = Logger.getLogger(PCSCEidImpl.class.getName());
    private final View mView;
    private final Messages mMessages;
    private final PcscEidSpi mPcscEidSpi;
    private Map<byte[], byte[]> fileCache;
    private Map<byte[], X509Certificate> certCache;

    @SuppressWarnings("unchecked")
    public PCSCEidImpl(View view, Messages messages)
    {
        mView = view;
        mMessages = messages;
        if (!System.getProperty("java.version").startsWith("1.5"))
        {
            /*
             * Java 1.6 and later. Loading the PcscEid class via reflection
             * avoids a hard reference to Java 1.6 specific code. This is
             * required in order to be able to let a Java 1.5 runtime load this
             * Controller class without exploding on unsupported 1.6 types.
             */
            try
            {
                Class<? extends PcscEidSpi> pcscEidClass = (Class<? extends PcscEidSpi>) Class.forName("be.fedict.eid.applet.sc.PcscEid");
                Constructor<? extends PcscEidSpi> pcscEidConstructor = pcscEidClass.getConstructor(View.class, Messages.class);
                mPcscEidSpi = pcscEidConstructor.newInstance(mView, mMessages);
            }
            catch (Exception e)
            {
                String msg = "error loading PC/SC eID component: " + e.getMessage();
                this.mView.addDetailMessage(msg);
                throw new EidException(msg, e);
            }

            mPcscEidSpi.addObserver(new PcscEidObserver());
        }
        else
        {
            /*
             * No PC/SC Java 6 available.
             */
            mPcscEidSpi = null;
        }

        if (mPcscEidSpi == null)
        {
            String msg = "no PKCS11 interface available";
            this.mView.addDetailMessage(msg);
            throw new EidException(msg);
        }

        fileCache = new HashMap<byte[], byte[]>();
        certCache = new HashMap<byte[], X509Certificate>();
    }

    public List<String> getReaderList()
    {
        return mPcscEidSpi.getReaderList();
    }

    public byte[] readFile(byte[] fileId) throws Exception
    {
        logger.finest("readFile");
        return mPcscEidSpi.readFile(fileId);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    public Address getAddress() throws Exception
    {
        logger.fine("getAddress");
        return TlvParser.parse(getFile(PcscEid.ADDRESS_FILE_ID), Address.class);
    }

    public Identity getIdentity() throws Exception
    {
        logger.fine("getIdentity");
        return TlvParser.parse(getFile(PcscEid.IDENTITY_FILE_ID), Identity.class);
    }

    public Image getPhoto() throws Exception
    {
        logger.fine("getPhoto");
        byte[] data = readFile(PcscEid.PHOTO_FILE_ID);
        return ImageIO.read(new ByteArrayInputStream(data));
    }

    public Image getPhotoImage() throws Exception
    {
        logger.fine("getPhotoImage");
        byte[] data = readFile(PcscEid.PHOTO_FILE_ID);
        return ImageIO.read(new ByteArrayInputStream(data));
    }

    public byte[] getPhotoJPEG() throws Exception
    {
        logger.fine("getPhotoJPEG");
        return readFile(PcscEid.PHOTO_FILE_ID);
    }

    public void close()
    {
        mPcscEidSpi.close();
    }

    public boolean isEidPresent() throws Exception
    {
        return mPcscEidSpi.isEidPresent();
    }

    public boolean hasCardReader() throws Exception
    {
        return mPcscEidSpi.hasCardReader();
    }

    public void waitForCardReader() throws Exception
    {
        mPcscEidSpi.waitForCardReader();
    }

    public void waitForEidPresent() throws Exception
    {
        mPcscEidSpi.waitForEidPresent();
    }

    public void removeCard() throws Exception
    {
        mPcscEidSpi.removeCard();
        clear();
    }

    public boolean isCardStillPresent() throws Exception
    {
        if (mPcscEidSpi.isCardStillPresent())
        {
            return true;
        }
        clear();
        return false;
    }

    public void changePin() throws Exception
    {
        mPcscEidSpi.changePin();
    }

    public void changePin(boolean requireSecureReader) throws Exception
    {
        mPcscEidSpi.changePin();
    }

    public void verifyPin(boolean requireSecureReader) throws Exception
    {
        byte[] dummyData=new byte[128];
        logger.fine("Logging Off To Make Sure PIN Cache is Cleared");
        mPcscEidSpi.logoff();
        logger.fine("Signing 128 Zero Bytes to Trigger PIN Check");
        mPcscEidSpi.signAuthn(dummyData);
        logger.fine("Logging Off To Clear PIN Cache");
        mPcscEidSpi.logoff();
        logger.fine("##### PIN Check OK");
    }

    public void addObserver(Observer observer)
    {
        mPcscEidSpi.addObserver(observer);
    }

    public void yieldExclusive(boolean yield) throws Exception
    {
        mPcscEidSpi.yieldExclusive(yield);
    }

    public boolean isIdentityTrusted()
    {
        logger.fine("isIdentityTrusted");
        try
        {
            logger.finest("isValidSignature");
            return Validation.isValidSignature(getRRNCert(), getFile(PcscEid.IDENTITY_FILE_ID), getIdentitySignature());
        }
        catch (Exception ex)
        {
            logger.log(Level.SEVERE, "Identity Signature Validation Failed", ex);
            return false;
        }
    }

    public boolean isAddressTrusted()
    {
        logger.fine("isAddressTrusted");
        try
        {
            logger.finest("isValidSignature");
            return Validation.isValidSignature(getRRNCert(), trimRight(getFile(PcscEid.ADDRESS_FILE_ID)), getIdentitySignature(), getAddressSignature());
        }
        catch (Exception ex)
        {
            logger.log(Level.SEVERE, "Address Signature Validation Failed", ex);
            return false;
        }
    }

    public List<X509Certificate> getRRNCertificateChain() throws Exception
    {
        List<X509Certificate> authnCertificateChain = new LinkedList<X509Certificate>();
        authnCertificateChain.add(getRRNCert());
        authnCertificateChain.add(getRootCACert());
        return authnCertificateChain;
    }

    public List<X509Certificate> getAuthnCertificateChain() throws Exception
    {
        List<X509Certificate> authnCertificateChain = new LinkedList<X509Certificate>();
        authnCertificateChain.add(getAuthCert());
        authnCertificateChain.add(getCitizenCACert());
        authnCertificateChain.add(getRootCACert());
        return authnCertificateChain;
    }

    public List<X509Certificate> getSignCertificateChain() throws Exception
    {
        List<X509Certificate> authnCertificateChain = new LinkedList<X509Certificate>();
        authnCertificateChain.add(getSignCert());
        authnCertificateChain.add(getCitizenCACert());
        authnCertificateChain.add(getRootCACert());
        return authnCertificateChain;
    }

    private X509Certificate getAuthCert() throws Exception
    {
        return getCertificate(PcscEid.AUTHN_CERT_FILE_ID);
    }

    private X509Certificate getSignCert() throws Exception
    {
        return getCertificate(PcscEid.SIGN_CERT_FILE_ID);
    }

    private X509Certificate getRRNCert() throws Exception
    {
        return getCertificate(PcscEid.RRN_CERT_FILE_ID);
    }

    private X509Certificate getRootCACert() throws Exception
    {
        return getCertificate(PcscEid.ROOT_CERT_FILE_ID);
    }

    private X509Certificate getCitizenCACert() throws Exception
    {
        return getCertificate(PcscEid.CA_CERT_FILE_ID);
    }

    public byte[] getIdentitySignature() throws Exception
    {
        return getFile(PcscEid.IDENTITY_SIGN_FILE_ID);
    }

    public byte[] getAddressSignature() throws Exception
    {
        return getFile(PcscEid.ADDRESS_SIGN_FILE_ID);
    }

    private X509Certificate getCertificate(byte[] fileID) throws Exception
    {
        X509Certificate certificate = certCache.get(fileID);
        if (certificate == null)
        {
            CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");
            byte[] data = readFile(fileID);
            certificate = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(data));
            certCache.put(fileID, certificate);
        }
        return certificate;
    }

    private byte[] getFile(byte[] fileID) throws Exception
    {
        byte[] data = fileCache.get(fileID);
        if (data == null)
        {
            data = readFile(fileID);
            fileCache.put(fileID, data);
        }
        return data;
    }


    private byte[] trimRight(byte[] addressFile)
    {
        int idx;
        for (idx = 0; idx < addressFile.length; idx++)
        {
            if (addressFile[idx] == 0)
            {
                break;
            }
        }
        byte[] result = new byte[idx];
        System.arraycopy(addressFile, 0, result, 0, idx);
        return result;
    }

    private void clear()
    {
        fileCache.clear();
        certCache.clear();
    }
}
