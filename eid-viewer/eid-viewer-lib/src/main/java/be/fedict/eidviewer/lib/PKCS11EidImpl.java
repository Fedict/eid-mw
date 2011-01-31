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

import be.fedict.eid.applet.Messages;
import be.fedict.eid.applet.View;
import be.fedict.eid.applet.sc.Pkcs11Eid;
import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import java.awt.Image;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.X509Certificate;
import java.util.List;
import java.util.Observer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.smartcardio.CardException;

/**
 *
 * @author frank
 */
class PKCS11EidImpl implements Eid
{

    private final View mView;
    private final Messages mMessages;
    private final Pkcs11Eid mPkcs11Eid;

    public PKCS11EidImpl(View view, Messages messages)
    {
        this.mView = view;
        this.mMessages = messages;

        try
        {
            mPkcs11Eid = new Pkcs11Eid(mView, mMessages);
        }
        catch (NoClassDefFoundError e)
        {
            /*
             * sun/security/pkcs11/SunPKCS11 is not available on Windows 64 bit
             * JRE 1.6.0_20.
             */
            mView.addDetailMessage("class not found: " + e.getMessage());
            throw e;
        }
    }

    public List<String> getReaderList() throws Exception
    {
        try
        {
            return mPkcs11Eid.getReaderList();
        }
        catch (Exception ex)
        {
            Logger.getLogger(PKCS11EidImpl.class.getName()).log(Level.SEVERE, null, ex);
            mView.addDetailMessage(PKCS11EidImpl.class.getName() + ": " + ex.toString());
            throw new Exception(ex.toString());
        }
    }

    public byte[] readFile(byte[] fileId) throws Exception
    {
        throw new UnsupportedOperationException("Not supported.");
    }

    public Address getAddress() throws Exception
    {
       throw new UnsupportedOperationException("Not supported.");
    }

    public Identity getIdentity() throws Exception
    {
        throw new UnsupportedOperationException("Not supported.");
    }

    public Image getPhotoImage() throws Exception
    {
       throw new UnsupportedOperationException("Not supported.");
    }

    public void close() throws Exception
    {
        try
        {
            mPkcs11Eid.close();
        }
        catch (Exception ex)
        {
            Logger.getLogger(PKCS11EidImpl.class.getName()).log(Level.SEVERE, null, ex);
            mView.addDetailMessage(PKCS11EidImpl.class.getName() + ": " + ex.toString());
            throw new Exception(ex.toString());
        }
    }

    public boolean isEidPresent() throws Exception
    {
        return mPkcs11Eid.isEidPresent();
    }

    public boolean hasCardReader() throws Exception
    {
        throw new UnsupportedOperationException("Not supported.");
    }

    public void waitForCardReader() throws Exception
    {
        throw new UnsupportedOperationException("Not supported.");
    }

    public void waitForEidPresent() throws Exception
    {
        mPkcs11Eid.waitForEidPresent();
    }

    public void removeCard() throws Exception
    {
        mPkcs11Eid.removeCard();
    }

    public void changePin() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void unblockPin() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void changePin(boolean requireSecureReader) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void unblockPin(boolean requireSecureReader) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public byte[] signAuthn(byte[] toBeSigned) throws Exception
    {
        try
        {
            return mPkcs11Eid.signAuthn(toBeSigned);
        }
        catch (Exception ex)
        {
            Logger.getLogger(PKCS11EidImpl.class.getName()).log(Level.SEVERE, null, ex);
            mView.addDetailMessage(PKCS11EidImpl.class.getName() + ": " + ex.toString());
            throw new Exception(ex.toString());
        }
    }

    public byte[] signAuthn(byte[] toBeSigned, boolean requireSecureReader) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public byte[] sign(byte[] digestValue, String digestAlgo, boolean requireSecureReader) throws NoSuchAlgorithmException, CardException, IOException, InterruptedException
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public List<X509Certificate> getAuthnCertificateChain() throws Exception
    {
        try
        {
            return mPkcs11Eid.getAuthnCertificateChain();
        }
        catch (Exception ex)
        {
            Logger.getLogger(PKCS11EidImpl.class.getName()).log(Level.SEVERE, null, ex);
            mView.addDetailMessage(PKCS11EidImpl.class.getName() + ": " + ex.toString());
            throw new Exception(ex.toString());
        }
    }

    public List<X509Certificate> getSignCertificateChain() throws Exception
    {
        return mPkcs11Eid.getSignCertificateChain();
    }

    public void logoff() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void addObserver(Observer observer)
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void logoff(String readerName) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void selectBelpicJavaCardApplet()
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public boolean isCardStillPresent() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void yieldExclusive(boolean yield) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public byte[] getPhotoJPEG() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public X509Certificate getNRNCertificate() throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public List<X509Certificate> getRRNCertificateChain()
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public boolean isIdentityTrusted()
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public boolean isAddressTrusted()
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void verifyPin(boolean requireSecureReader) throws Exception
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
