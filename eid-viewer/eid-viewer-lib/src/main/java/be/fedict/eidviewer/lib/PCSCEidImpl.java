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
import javax.imageio.ImageIO;

public class PCSCEidImpl implements Eid
{
    private final View          mView;
    private final Messages      mMessages;
    private final PcscEidSpi    mPcscEidSpi;

    @SuppressWarnings("unchecked")
    public PCSCEidImpl(View view, Messages messages)
    {
        mView = view;
        mMessages = messages;
        if(!System.getProperty("java.version").startsWith("1.5"))
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
                mPcscEidSpi = pcscEidConstructor.newInstance(mView,mMessages);
            }
            catch (Exception e)
            {
                String msg = "error loading PC/SC eID component: " + e.getMessage();
                this.mView.addDetailMessage(msg);
                throw new EidException(msg,e);
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

        if(mPcscEidSpi==null)
        {
            String msg="no PKCS11 interface available";
            this.mView.addDetailMessage(msg);
            throw new EidException(msg);
        }
    }

    public List<String> getReaderList()
    {
        return mPcscEidSpi.getReaderList();
    }

    public byte[] readFile(byte[] fileId) throws Exception
    {
        return mPcscEidSpi.readFile(fileId);
    }

    public Address getAddress() throws Exception
    {
        byte[] data=readFile(PcscEid.ADDRESS_FILE_ID);
        return TlvParser.parse(data, Address.class);
    }

    public Identity getIdentity() throws Exception
    {
       byte[] data=readFile(PcscEid.IDENTITY_FILE_ID);
       return TlvParser.parse(data, Identity.class);
    }

    public Image getPhoto() throws Exception
    {
        byte[] data=readFile(PcscEid.PHOTO_FILE_ID);
        return ImageIO.read(new ByteArrayInputStream(data));
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
    }

    public boolean isCardStillPresent() throws Exception
    {
        return mPcscEidSpi.isCardStillPresent();
    }

    public void changePin() throws Exception
    {
        mPcscEidSpi.changePin();
    }

    public void changePin(boolean requireSecureReader) throws Exception
    {
        mPcscEidSpi.changePin();
    }

    public List<X509Certificate> getAuthnCertificateChain() throws Exception
    {
        return mPcscEidSpi.getAuthnCertificateChain();
    }

    public List<X509Certificate> getSignCertificateChain() throws Exception
    {
        return mPcscEidSpi.getSignCertificateChain();
    }

    public void addObserver(Observer observer)
    {
        mPcscEidSpi.addObserver(observer);
    }

    public void yieldExclusive(boolean yield) throws Exception
    {
        mPcscEidSpi.yieldExclusive(yield);
    }
}
