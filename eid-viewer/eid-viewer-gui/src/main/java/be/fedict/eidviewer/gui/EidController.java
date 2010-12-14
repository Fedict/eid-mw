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
import be.fedict.eidviewer.lib.Eid;
import be.fedict.trust.client.TrustServiceDomains;
import java.awt.Image;
import java.util.Observable;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Observer;

/**
 *
 * @author Frank Marien
 */
public class EidController extends Observable implements Runnable,Observer
{
    private boolean                         running = false;
    private Eid                             eid;
    private STATE                           state;
    private ACTIVITY                        activity;
    private ACTION                          runningAction;
    private Identity                        identity;
    private Address                         address;
    private Image                           photo;
    private X509CertificateChainAndTrust    authCertChain;
    private X509CertificateChainAndTrust    signCertChain;
    private TrustServiceController          trustServiceController;
    private boolean                         autoValidatingTrust;

    public EidController(Eid eid)
    {
        this.eid = eid;
        setState(STATE.IDLE);
        setActivity(ACTIVITY.IDLE);
        this.runningAction=ACTION.NONE;
        this.autoValidatingTrust=false;
    }

    public void start()
    {
        Thread me=new Thread(this);
               me.setDaemon(true);
               me.start();
    }

    public void stop()
    {
        running=false;
        if(trustServiceController!=null)
            trustServiceController.stop();
    }

    public EidController setTrustServiceController(TrustServiceController trustServiceController)
    {
        this.trustServiceController=trustServiceController;
        this.trustServiceController.addObserver(this);
        this.trustServiceController.start();
        return this;
    }

    public void setAutoValidateTrust(boolean autoValidateTrust)
    {
        if(trustServiceController!=null)
            this.autoValidatingTrust=autoValidateTrust;
    }

    private void eid_changePin() throws Exception
    {
        try
        {
            eid.changePin();
        }
        catch(RuntimeException rte)
        {
           Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, rte);
        }
        
        runningAction = ACTION.NONE;
    }

    private void trustController_validateTrust() throws Exception
    {
        if(trustServiceController==null)
            return;
        
        try
        {
            trustServiceController.validateLater(authCertChain);
            trustServiceController.validateLater(signCertChain);
            setState();
        }
        catch(RuntimeException rte)
        {
           Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, rte);
        }

        runningAction = ACTION.NONE;
    }
    

    private void securityClear()
    {
        identity = null;
        address = null;
        photo = null;
        authCertChain = null;
        signCertChain = null;

        if(trustServiceController!=null)
            trustServiceController.clear();
    }

    @Override
    public void update(Observable o, Object o1)
    {
        setState();
    }

    public static enum STATE
    {
        IDLE            ("state_idle"),
        ERROR           ("state_error"),
        NO_READERS      ("state_noreaders"),
        NO_EID_PRESENT  ("state_noeidpresent"),
        EID_PRESENT     ("state_eidpresent");
        
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

        IDLE                    ("activity_idle"),
        READING_IDENTITY        ("reading_identity"),
        READING_ADDRESS         ("reading_address"),
        READING_PHOTO           ("reading_photo"),
        READING_AUTH_CHAIN      ("reading_auth_chain"),
        READING_SIGN_CHAIN      ("reading_sign_chain"),
        VERIFYING_AUTH_CHAINS   ("verifying_auth_chains"),
        VERIFYING_SIGN_CHAINS   ("verifying_sign_chains");
        
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
        NONE(0), CHANGE_PIN(1), VALIDATETRUST(2);
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

    private void setStateAndActivity(STATE newState,ACTIVITY newActivity)
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

                setStateAndActivity(STATE.EID_PRESENT,ACTIVITY.READING_IDENTITY);
                identity = eid.getIdentity();
                setState();

                setActivity(ACTIVITY.READING_ADDRESS);
                address = eid.getAddress();
                setState();

                setActivity(ACTIVITY.READING_PHOTO);
                photo = eid.getPhoto();
                setState();

                setActivity(ACTIVITY.READING_AUTH_CHAIN);
                authCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN,eid.getAuthnCertificateChain());
                if(trustServiceController!=null && autoValidatingTrust)
                    trustServiceController.validateLater(authCertChain);
                setState();

                setActivity(ACTIVITY.READING_SIGN_CHAIN);
                signCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN,eid.getSignCertificateChain());
                if(trustServiceController!=null && autoValidatingTrust)
                    trustServiceController.validateLater(signCertChain);
                setActivity(ACTIVITY.IDLE);

                while(eid.isCardStillPresent())
                {
                    if(runningAction == ACTION.CHANGE_PIN)
                    {
                        eid_changePin();
                    }
                    else if(runningAction == ACTION.VALIDATETRUST)
                    {
                        trustController_validateTrust();
                    }
                    else
                    {
                        try
                        {
                            Thread.sleep(200);
                        }
                        catch (InterruptedException ex1)
                        {
                            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex1);
                        }
                    }
                }

                securityClear();
                setState(STATE.IDLE);
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
        if(trustServiceController==null)
            return this;
        runningAction = ACTION.VALIDATETRUST;
        return this;
    }

    public boolean isReadyForCommand()
    {
        return (state==STATE.EID_PRESENT) && (activity==ACTIVITY.IDLE) && (runningAction==ACTION.NONE) && (!isValidatingTrust());
    }

    public boolean isValidatingTrust()
    {
        return trustServiceController!=null?trustServiceController.isValidating():false;
    }

    public boolean isAutoValidatingTrust()
    {
        return autoValidatingTrust;
    }
}
