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
import be.fedict.eidviewer.gui.file.EidFiles;
import be.fedict.eidviewer.lib.Eid;
import be.fedict.trust.client.TrustServiceDomains;
import java.awt.Image;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.Observable;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Observer;
import java.util.Timer;
import java.util.TimerTask;
import javax.imageio.ImageIO;
import javax.smartcardio.CardException;

/**
 *
 * @author Frank Marien
 */
public class EidController extends Observable implements Runnable, Observer, EidData
{
    private static final Logger logger = Logger.getLogger(EidController.class.getName());
    private boolean running = false;
    private Eid eid;
    private STATE state;
    private ACTIVITY activity;
    private ACTION runningAction;
    private Identity identity;
    private Address address;
    private byte[] photo;
    private boolean identityTrusted,addressTrusted,identityValidated,addressValidated;
    private X509CertificateChainAndTrust rrnCertChain;
    private X509CertificateChainAndTrust authCertChain;
    private X509CertificateChainAndTrust signCertChain;
    private TrustServiceController trustServiceController;
    private Timer   yieldLockedTimer;
    private long    yieldConsideredLockedAt;
    private boolean autoValidatingTrust, yielding,loadedFromFile;

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
        logger.fine("starting..");
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
        logger.fine("stopping..");
        running = false;
        yieldLockedTimer.cancel();
        if (trustServiceController != null)
            trustServiceController.stop();
    }

    public EidController setTrustServiceController(TrustServiceController trustServiceController)
    {
        logger.fine("setting TrustServiceController");
        this.trustServiceController = trustServiceController;
        this.trustServiceController.addObserver(this);
        this.trustServiceController.start();
        return this;
    }

    public void setAutoValidateTrust(boolean autoValidateTrust)
    {
        if (trustServiceController != null)
            this.autoValidatingTrust = autoValidateTrust;
    }

    private void eid_changePin() throws Exception
    {
        logger.fine("eid_changePin");
        
        try
        {
            eid.changePin();
        }
        catch (RuntimeException rte)
        {
            logger.log(Level.SEVERE, "ChangePin Operation Failed", rte);
        }

        runningAction = ACTION.NONE;
    }

    private void eid_verifyPin() throws Exception
    {
        logger.fine("eid_verifyPin");

        try
        {
            eid.verifyPin(false);
        }
        catch (RuntimeException rte)
        {
            logger.log(Level.SEVERE, "VerifyPin Operation Failed", rte);
        }

        runningAction = ACTION.NONE;
    }

    private void trustController_validateTrust() throws Exception
    {
        logger.fine("trustController_validateTrust");
        
        if (trustServiceController == null)
            return;

        try
        {
            trustServiceController.validateLater(rrnCertChain);
            trustServiceController.validateLater(authCertChain);
            trustServiceController.validateLater(signCertChain);
            setState();
        }
        catch (RuntimeException rte)
        {
            logger.log(Level.SEVERE, "Failed To Enqueue Trust Validations", rte);
        }

        runningAction = ACTION.NONE;
    }

    public void securityClear()
    {
        logger.fine("securityClear");

        identity = null;
        address = null;
        photo = null;
        authCertChain = null;
        signCertChain = null;
        rrnCertChain = null;
        if (trustServiceController != null)
            trustServiceController.clear();
        identityValidated=false;
        addressValidated=false;
        identityTrusted=false;
        addressTrusted=false;
        setState();
    }

    @Override
    public void update(Observable o, Object o1)
    {
        setState();
    }

    void loadFromFile(File file)
    {
        setState(STATE.FILE_LOADING);
        
        try
        {
            EidFiles.loadFromFile(file, this);
            setLoadedFromFile(true);
            setState(STATE.FILE_LOADED);
        }
        catch(Exception ex)
        {
            logger.log(Level.SEVERE, "Failed To Load EID File", ex);
            securityClear();
            setState(STATE.IDLE);
        }
    }

    void saveToXMLFile(File selectedFile)
    {
        EidFiles.saveToXMLFile(selectedFile, this);
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
        IDLE                ("activity_idle"),
        READING_IDENTITY    ("reading_identity"),
        READING_ADDRESS     ("reading_address"),
        READING_PHOTO       ("reading_photo"),
        READING_RRN_CHAIN   ("reading_rrn_chain"),
        READING_AUTH_CHAIN  ("reading_auth_chain"),
        READING_SIGN_CHAIN  ("reading_sign_chain"),
        VALIDATING_IDENTITY ("validating_identity"),
        VALIDATING_ADDRESS  ("validating_address");
        
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
        NONE("none"), CHANGE_PIN("change_pin"), VALIDATETRUST("validatetrust"), VERIFY_PIN("verify_pin");
        private final String order;

        private ACTION(String order)
        {
            this.order = order;
        }

        @Override
        public String toString()
        {
            return this.order;
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
        logger.log(Level.FINER,"state {0} activity {1} action {2}", new Object[] {getState()!=null?getState().toString():"null", getActivity()!=null?getActivity().toString():"null",runningAction!=null?runningAction.toString():"null"});
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
                    logger.fine("waiting for card readers..");
                    setState(STATE.NO_READERS);
                    eid.waitForCardReader();
                }

                if(!eid.isEidPresent())
                {
                    logger.fine("waiting for eid card..");
                    setState(STATE.NO_EID_PRESENT);
                    eid.waitForEidPresent();
                }

                if(isLoadedFromFile())
                {
                    logger.fine("clearing file-loaded data");
                    securityClear();
                    setState(STATE.IDLE);
                }

                logger.fine("reading identity from card..");
                setStateAndActivity(STATE.EID_PRESENT, ACTIVITY.READING_IDENTITY);
                setLoadedFromFile(false);
                identity = eid.getIdentity();
                setState();

                logger.fine("reading address from card..");
                setActivity(ACTIVITY.READING_ADDRESS);
                address = eid.getAddress();
                setState();
                
                logger.fine("reading photo from card..");
                setActivity(ACTIVITY.READING_PHOTO);
                photo = eid.getPhotoJPEG();
                setState();
                
                logger.fine("reading rrn chain from card..");
                setActivity(ACTIVITY.READING_RRN_CHAIN);
                rrnCertChain = new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NATIONAL_REGISTRY_TRUST_DOMAIN,eid.getRRNCertificateChain());
                if (trustServiceController != null && autoValidatingTrust)
                    trustServiceController.validateLater(rrnCertChain);
                setState();

                logger.fine("validating identity");
                setActivity(ACTIVITY.VALIDATING_IDENTITY);
                identityTrusted=eid.isIdentityTrusted();
                identityValidated=true;
                setState();

                logger.fine("validating address");
                setActivity(ACTIVITY.VALIDATING_ADDRESS);
                addressTrusted=eid.isAddressTrusted();
                addressValidated=true;
                setState();

                logger.fine("reading authentication chain from card..");
                setActivity(ACTIVITY.READING_AUTH_CHAIN);
                authCertChain = new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, eid.getAuthnCertificateChain());
                if (trustServiceController != null && autoValidatingTrust)
                    trustServiceController.validateLater(authCertChain);
                setState();

                logger.fine("reading signing chain from card..");
                setActivity(ACTIVITY.READING_SIGN_CHAIN);
                signCertChain = new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, eid.getSignCertificateChain());
                if (trustServiceController != null && autoValidatingTrust)
                    trustServiceController.validateLater(signCertChain);
                setActivity(ACTIVITY.IDLE);

                logger.fine("waiting for actions or card removal..");

                while (eid.isCardStillPresent())
                {
                    if (runningAction == ACTION.CHANGE_PIN)
                    {
                        logger.fine("requesting change_pin action");
                        eid_changePin();
                    }
                    else if(runningAction == ACTION.VERIFY_PIN)
                    {
                        logger.fine("requesting verify_pin action");
                        eid_verifyPin();
                    }
                    else if (runningAction == ACTION.VALIDATETRUST)
                    {
                        logger.fine("requesting validate_trust action");
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
                            logger.log(Level.SEVERE, "Activity Loop was Interrupted",iex);
                        }
                        catch(CardException cex)
                        {
                            if(eid.isCardStillPresent())
                                logger.log(Level.SEVERE, "CardException in activity loop", cex);
                        }
                        finally
                        {
                            setYielding(false);
                        }
                    }
                }

                logger.fine("card was removed..");

                if(!isLoadedFromFile())
                {
                    logger.fine("clearing data of removed card");
                    securityClear();
                    setState(STATE.IDLE);
                }
            }
            catch (Exception ex)   // something failed. Clear out all data for security
            {
                securityClear();
                runningAction = ACTION.NONE;
                setState(STATE.ERROR);
                logger.log(Level.SEVERE, "Clearing Data for security reasons, due to unexpected problem.", ex);

                try
                {
                    Thread.sleep(1000);
                }
                catch (InterruptedException ex1)
                {
                    logger.log(Level.SEVERE, "Error Grace Time Loop Interruped", ex1);
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

    public byte[] getPhoto()
    {
        return photo;
    }

    public Image getPhotoImage() throws IOException
    {
        return ImageIO.read(new ByteArrayInputStream(getPhoto()));
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

    public boolean hasRRNCertChain()
    {
        return (rrnCertChain != null);
    }

    public X509CertificateChainAndTrust getRRNCertChain()
    {
        return rrnCertChain;
    }

    public EidController changePin()
    {
        runningAction = ACTION.CHANGE_PIN;
        return this;
    }

    public EidController verifyPin()
    {
        runningAction = ACTION.VERIFY_PIN;
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
                logger.fine("validate_trust for data from file..");
                trustController_validateTrust();
            }
            catch (Exception ex)
            {
                logger.log(Level.SEVERE, "Problem Validating Trust From Saved Identity", ex);
            }
        }
        else
        {
            logger.fine("validate_trust for data on inserted card..");
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
            yieldConsideredLockedAt=1000+System.currentTimeMillis();
        else
            yieldConsideredLockedAt=Long.MAX_VALUE;
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

    public boolean isAddressTrusted()
    {
        return addressTrusted;
    }

    public boolean isIdentityTrusted()
    {
        return identityTrusted;
    }

    public boolean isAddressValidated()
    {
        return addressValidated;
    }

    public boolean isIdentityValidated()
    {
        return identityValidated;
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

    public synchronized EidController setSignCertChain(X509CertificateChainAndTrust signCertChain)
    {
        this.signCertChain = signCertChain;
        if (trustServiceController != null && autoValidatingTrust)
            trustServiceController.validateLater(signCertChain);
        return this;
    }

    public synchronized EidController setRRNCertChain(X509CertificateChainAndTrust rrnCertChain)
    {
        this.rrnCertChain = rrnCertChain;
        if (trustServiceController != null && autoValidatingTrust)
            trustServiceController.validateLater(rrnCertChain);
        return this;
    }

    public synchronized EidController setIdentity(Identity identity)
    {
        this.identity = identity;
        return this;
    }

    public synchronized EidController setPhoto(byte[] photo)
    {
        this.photo = photo;
        return this;
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
