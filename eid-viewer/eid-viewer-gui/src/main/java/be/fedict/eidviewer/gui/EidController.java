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

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eidviewer.gui.helper.CloseResistantZipInputStream;
import be.fedict.eidviewer.gui.helper.CloseResistantZipOutputStream;
import be.fedict.eidviewer.lib.Eid;
import be.fedict.trust.client.TrustServiceDomains;
import java.awt.Image;
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
import java.util.Observable;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Observer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.zip.ZipEntry;
import javax.smartcardio.CardException;
import javax.swing.ImageIcon;

/**
 *
 * @author Frank Marien
 */
public class EidController extends Observable implements Runnable, Observer
{
    private static final String ZIPFILE_ADDRESS_FILENAME = "address.ser";
    private static final String ZIPFILE_AUTHCERT_FILENAME = "authcert.der";
    private static final String ZIPFILE_CITIZENCERT_FILENAME = "citicert.der";
    private static final String ZIPFILE_IDENTITY_FILENAME = "identity.ser";
    private static final String ZIPFILE_PHOTO_FILENAME = "photo.ser";
    private static final String ZIPFILE_ROOTCERT_FILENAME = "rootcert.der";
    private static final String ZIPFILE_SIGNCERT_FILENAME = "signcert.der";

    private boolean running = false;
    private Eid eid;
    private STATE state;
    private ACTIVITY activity;
    private ACTION runningAction;
    private Identity identity;
    private Address address;
    private Image photo;
    private X509CertificateChainAndTrust authCertChain;
    private X509CertificateChainAndTrust signCertChain;
    private TrustServiceController trustServiceController;
    private Timer   yieldLockedTimer;
    private long    yieldConsideredLockedAt;
    private boolean autoValidatingTrust, yielding;
    private boolean loadedFromFile;

    public EidController(Eid eid)
    {
        this.eid = eid;
        setState(STATE.IDLE);
        setActivity(ACTIVITY.IDLE);
        this.runningAction = ACTION.NONE;
        this.autoValidatingTrust = false;
        yieldLockedTimer = new Timer("yieldLockedTimer", true);
        yieldConsideredLockedAt=Long.MAX_VALUE;
    }

    public void start()
    {
        Thread me = new Thread(this);
        me.setDaemon(true);
        me.start();
        
        yieldLockedTimer.schedule(new TimerTask()
        {
            @Override
            public void run()
            {
                if(state==STATE.EID_PRESENT && (System.currentTimeMillis()>yieldConsideredLockedAt))
                {
                   setState(STATE.EID_YIELDED);
                }
                else
                {
                    if(state==STATE.EID_YIELDED && (System.currentTimeMillis()<yieldConsideredLockedAt))
                    {
                        setState(STATE.EID_PRESENT);
                    }
                }
            }   
        },
        1000, 500);
    }

    public void stop()
    {
        running = false;
        if (trustServiceController != null)
        {
            trustServiceController.stop();
        }
    }

    public EidController setTrustServiceController(TrustServiceController trustServiceController)
    {
        this.trustServiceController = trustServiceController;
        this.trustServiceController.addObserver(this);
        this.trustServiceController.start();
        return this;
    }

    public void setAutoValidateTrust(boolean autoValidateTrust)
    {
        if (trustServiceController != null)
        {
            this.autoValidatingTrust = autoValidateTrust;
        }
    }

    private void eid_changePin() throws Exception
    {
        try
        {
            eid.changePin();
        }
        catch (RuntimeException rte)
        {
            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, rte);
        }

        runningAction = ACTION.NONE;
    }

    private void trustController_validateTrust() throws Exception
    {
        if (trustServiceController == null)
            return;

        try
        {
            trustServiceController.validateLater(authCertChain);
            trustServiceController.validateLater(signCertChain);
            setState();
        }
        catch (RuntimeException rte)
        {
            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, rte);
        }

        runningAction = ACTION.NONE;
    }

    public void securityClear()
    {
        identity = null;
        address = null;
        photo = null;
        authCertChain = null;
        signCertChain = null;
        if (trustServiceController != null)
            trustServiceController.clear();
        setState();
    }

    @Override
    public void update(Observable o, Object o1)
    {
        setState();
    }

    

    public static enum STATE
    {
        IDLE("state_idle"),
        ERROR("state_error"),
        NO_READERS("state_noreaders"),
        NO_EID_PRESENT("state_noeidpresent"),
        EID_PRESENT("state_eidpresent"),
        FILE_LOADING("state_fileloading"),
        FILE_LOADED("state_fileloaded"),
        EID_YIELDED("state_eidyielded");
        private final String state;

        private STATE(String state)
        {
            this.state = state;
        }

        @Override
        public String toString()
        {
            return this.state;
        }
    };

    public static enum ACTIVITY
    {

        IDLE("activity_idle"),
        READING_IDENTITY("reading_identity"),
        READING_ADDRESS("reading_address"),
        READING_PHOTO("reading_photo"),
        READING_AUTH_CHAIN("reading_auth_chain"),
        READING_SIGN_CHAIN("reading_sign_chain");
        private final String state;

        private ACTIVITY(String state)
        {
            this.state = state;
        }

        @Override
        public String toString()
        {
            return this.state;
        }
    }

    public static enum ACTION
    {

        NONE(0), CHANGE_PIN(1), VALIDATETRUST(2), READ_RAW_FILES(3);
        private final int order;

        private ACTION(int order)
        {
            this.order = order;
        }
    }

    private void setState(STATE newState)
    {
        state = newState;
        setState();
    }

    private void setActivity(ACTIVITY newActivity)
    {
        activity = newActivity;
        setState();
    }

    private void setStateAndActivity(STATE newState, ACTIVITY newActivity)
    {
        state = newState;
        activity = newActivity;
        setState();
    }

    private void setState()
    {
        setChanged();
        notifyObservers();
    }

    public void run()
    {
        running = true;
        while (running)
        {
            try
            {
                if(!eid.hasCardReader())
                {
                    setState(STATE.NO_READERS);
                    eid.waitForCardReader();
                }

                if(!eid.isEidPresent())
                {
                    setState(STATE.NO_EID_PRESENT);
                    eid.waitForEidPresent();
                }

                if(isLoadedFromFile())
                {
                    securityClear();
                    setState(STATE.IDLE);
                }

                setStateAndActivity(STATE.EID_PRESENT, ACTIVITY.READING_IDENTITY);
                setLoadedFromFile(false);
                identity = eid.getIdentity();
                setState();

                setActivity(ACTIVITY.READING_ADDRESS);
                address = eid.getAddress();
                setState();

                setActivity(ACTIVITY.READING_PHOTO);
                photo = eid.getPhoto();
                setState();

                setActivity(ACTIVITY.READING_AUTH_CHAIN);
                authCertChain = new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, eid.getAuthnCertificateChain());
                if (trustServiceController != null && autoValidatingTrust)
                    trustServiceController.validateLater(authCertChain);
                setState();

                setActivity(ACTIVITY.READING_SIGN_CHAIN);
                signCertChain = new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, eid.getSignCertificateChain());
                if (trustServiceController != null && autoValidatingTrust)
                    trustServiceController.validateLater(signCertChain);
                setActivity(ACTIVITY.IDLE);

                while (eid.isCardStillPresent())
                {
                    if (runningAction == ACTION.CHANGE_PIN)
                    {
                        eid_changePin();
                    }
                    else if (runningAction == ACTION.VALIDATETRUST)
                    {
                        trustController_validateTrust();
                    }
                    else
                    {
                        try
                        {
                            setYielding(true);
                            eid.yieldExclusive(true);
                            Thread.sleep(200);
                            eid.yieldExclusive(false);   
                        }
                        catch(InterruptedException iex)
                        {
                            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, "Activity Loop was Interrupted");
                        }
                        catch(CardException cex)
                        {
                            if(eid.isCardStillPresent())
                                Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, "CardException in activity loop", cex);
                        }
                        finally
                        {
                            setYielding(false);
                        }
                    }
                }

                if(!isLoadedFromFile())
                {
                    securityClear();
                    setState(STATE.IDLE);
                }
            }
            catch (Exception ex)   // something failed. Clear out all data for security
            {
                securityClear();
                runningAction = ACTION.NONE;
                setState(STATE.ERROR);
                Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex);

                try
                {
                    Thread.sleep(1000);
                }
                catch (InterruptedException ex1)
                {
                    Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex1);
                }

                setState(STATE.IDLE);
            }
        }
    }

    public Address getAddress()
    {
        return address;
    }

    public Identity getIdentity()
    {
        return identity;
    }

    public Image getPhoto()
    {
        return photo;
    }

    public STATE getState()
    {
        return state;
    }

    public ACTIVITY getActivity()
    {
        return activity;
    }

    public boolean hasAddress()
    {
        return (address != null);
    }

    public boolean hasIdentity()
    {
        return (identity != null);
    }

    public boolean hasPhoto()
    {
        return (photo != null);
    }

    public boolean hasAuthCertChain()
    {
        return (authCertChain != null);
    }

    public X509CertificateChainAndTrust getAuthCertChain()
    {
        return authCertChain;
    }

    public boolean hasSignCertChain()
    {
        return (signCertChain != null);
    }

    public X509CertificateChainAndTrust getSignCertChain()
    {
        return signCertChain;
    }

    public EidController changePin()
    {
        runningAction = ACTION.CHANGE_PIN;
        return this;
    }

    public EidController validateTrust()
    {
        if(trustServiceController == null)
            return this;
        
        if(state==STATE.FILE_LOADED)
        {
            try
            {
                trustController_validateTrust();
            }
            catch (Exception ex)
            {
                Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, "Problem Validating Trust From Saved Identity", ex);
            }
        }
        else
        {
            runningAction = ACTION.VALIDATETRUST;
        }
        
        return this;
    }

    public boolean isYielding()
    {
        return yielding;
    }

    public void setYielding(boolean yielding)
    {
        this.yielding = yielding;

        if(yielding)
        {
            yieldConsideredLockedAt=1000+System.currentTimeMillis();
        }
        else
        {
            yieldConsideredLockedAt=Long.MAX_VALUE;
        }
    }

    public boolean isReadyForCommand()
    {
        return (state == STATE.EID_PRESENT || state == STATE.FILE_LOADED) && (activity == ACTIVITY.IDLE) && (runningAction == ACTION.NONE) && (!isValidatingTrust());
    }

    public boolean isValidatingTrust()
    {
        return trustServiceController != null ? trustServiceController.isValidating() : false;
    }

    public boolean isAutoValidatingTrust()
    {
        return autoValidatingTrust;
    }

    public boolean isLoadedFromFile()
    {
        return loadedFromFile;
    }

    public synchronized EidController setLoadedFromFile(boolean loadedFromFile)
    {       
        this.loadedFromFile=loadedFromFile;
        return this;
    }
    
    public synchronized EidController setAddress(Address address)
    {
        this.address = address;
        return this;
    }

    public synchronized EidController setAuthCertChain(X509CertificateChainAndTrust authCertChain)
    {
        this.authCertChain = authCertChain;
        if (trustServiceController != null && autoValidatingTrust)
            trustServiceController.validateLater(authCertChain);
        return this;
    }

    public synchronized EidController setIdentity(Identity identity)
    {
        this.identity = identity;
        return this;
    }

    public synchronized EidController setPhoto(Image photo)
    {
        this.photo = photo;
        return this;
    }

    public synchronized EidController setSignCertChain(X509CertificateChainAndTrust signCertChain)
    {
        this.signCertChain = signCertChain;
        if (trustServiceController != null && autoValidatingTrust)
            trustServiceController.validateLater(signCertChain);
        return this;
    }

    public void loadFromFile(File file)
    {
        CloseResistantZipInputStream    zipInputStream      = null;
        ObjectInputStream               objectInputStream   = null;
        X509Certificate                 rootCert            = null;
        X509Certificate                 citizenCert         = null;
        X509Certificate                 authenticationCert  = null;
        X509Certificate                 signingCert         = null;
        CertificateFactory              certificateFactory  = null;

        setState(STATE.FILE_LOADING);

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
                    setIdentity((Identity)objectInputStream.readObject());
                    objectInputStream.close();

                }
                else if(entry.getName().equals(ZIPFILE_ADDRESS_FILENAME))
                {
                    objectInputStream=new ObjectInputStream(zipInputStream);
                    setAddress((Address)objectInputStream.readObject());
                    objectInputStream.close();

                }
                else if(entry.getName().equals(ZIPFILE_PHOTO_FILENAME))
                {
                    objectInputStream=new ObjectInputStream(zipInputStream);
                    setPhoto(((ImageIcon)objectInputStream.readObject()).getImage());
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
                    authCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, authChain);
                }

                if(signingCert!=null)
                {
                    List signChain=new LinkedList<X509Certificate>();
                         signChain.add(signingCert);
                         signChain.add(citizenCert);
                         signChain.add(rootCert);
                         
                    signCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, signChain);
                }

            }

            setState(STATE.FILE_LOADED);
            setLoadedFromFile(true);
            zipInputStream.setCloseAllowed(true);
            zipInputStream.close();
        }
        catch (CertificateException ex)
        {
            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch(FileNotFoundException ex)
        {
            Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch (IOException ex)
        {
            Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch (ClassNotFoundException cnfe)
        {
            Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, cnfe);
        }
    }

    public void saveToFile(File file)
    {
        CloseResistantZipOutputStream   zipOutputStream     =null;
        ObjectOutputStream              objectOutputStream  =null;
      
        try
        {
            zipOutputStream = new CloseResistantZipOutputStream(new FileOutputStream(file));

            if(hasIdentity())
            {
                zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_IDENTITY_FILENAME));
                objectOutputStream = new ObjectOutputStream(zipOutputStream);
                objectOutputStream.writeObject(getIdentity());
                objectOutputStream.close();
                zipOutputStream.closeEntry();
            }

            if(hasAddress())
            {
                zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_ADDRESS_FILENAME));
                objectOutputStream = new ObjectOutputStream(zipOutputStream);
                objectOutputStream.writeObject(getAddress());
                objectOutputStream.close();
                zipOutputStream.closeEntry();
            }

            if(hasPhoto())
            {
                zipOutputStream.putNextEntry(new ZipEntry(ZIPFILE_PHOTO_FILENAME));
                objectOutputStream = new ObjectOutputStream(zipOutputStream);
                objectOutputStream.writeObject(new ImageIcon(getPhoto()));
                objectOutputStream.close();
                zipOutputStream.closeEntry();
            }

            if(hasAuthCertChain())
            {
                List<X509Certificate> authChain=getAuthCertChain().getCertificates();
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

            if(hasSignCertChain())
            {
                List<X509Certificate> signChain=getSignCertChain().getCertificates();
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
        catch (CertificateEncodingException ex)
        {
            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex);
        }
        catch (IOException ex)
        {
            Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
        finally
        {
            try
            {
                zipOutputStream.setCloseAllowed(true);
                zipOutputStream.close();
            }
            catch (IOException ex)
            {
                Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public void closeFile()
    {
        if(isLoadedFromFile())
        {
            setLoadedFromFile(false);
            securityClear();
            setState(STATE.IDLE);
        }
    }
}
