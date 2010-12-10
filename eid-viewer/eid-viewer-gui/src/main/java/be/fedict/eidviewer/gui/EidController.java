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
 * @author frank
 */
public class EidController extends Observable implements Runnable,Observer
{
    private boolean                         mRunning = false;
    private Eid                             mEid;
    private STATE                           mState;
    private ACTIVITY                        mActivity;
    private ACTION                          mRunningAction;
    private Identity                        mIdentity;
    private Address                         mAddress;
    private Image                           mPhoto;
    private X509CertificateChainAndTrust    mAuthCertChain;
    private X509CertificateChainAndTrust    mSignCertChain;
    private TrustServiceController          mTrustServiceController;

    public EidController(Eid eid)
    {
        mEid = eid;
        setState(STATE.IDLE);
        setActivity(ACTIVITY.IDLE);
        mRunningAction=ACTION.NONE;
        mTrustServiceController =new TrustServiceController("http://www.e-contract.be/eid-trust-service-ws/xkms2");
    }

    public void start()
    {
        mTrustServiceController.addObserver(this);
        mTrustServiceController.start();

        Thread me=new Thread(this);
               me.setDaemon(true);
               me.start();
    }

    public void stop()
    {
        mRunning=false;
        mTrustServiceController.stop();
    }

    private void eid_changePin() throws Exception
    {
        try
        {
            mEid.changePin();
        }
        catch(RuntimeException rte)
        {
           Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, rte);
        }
        
        mRunningAction = ACTION.NONE;
    }
    

    private void clearCardInformation()
    {
        mIdentity = null;
        mAddress = null;
        mPhoto = null;
        mAuthCertChain = null;
        mSignCertChain = null;
    }

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
        READING_AUTH_CHAIN     ("reading_auth_chain"),
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

        NONE(0), CHANGE_PIN(1);
        private final int order;

        private ACTION(int order)
        {
            this.order = order;
        }
    }

    private void setState(STATE newState)
    {
        mState = newState;
        setState();
    }

    private void setActivity(ACTIVITY newActivity)
    {
        mActivity = newActivity;
        setState();
    }

    private void setStateAndActivity(STATE newState,ACTIVITY newActivity)
    {
        mState = newState;
        mActivity = newActivity;
        setState();
    }

    private void setState()
    {
        setChanged();
        notifyObservers();
    }

    public void run()
    {
        mRunning = true;
        while (mRunning)
        {
            try
            {
                if(!mEid.hasCardReader())
                {
                    setState(STATE.NO_READERS);
                    mEid.waitForCardReader();
                }

                if(!mEid.isEidPresent())
                {
                    setState(STATE.NO_EID_PRESENT);
                    mEid.waitForEidPresent();
                }

                setStateAndActivity(STATE.EID_PRESENT,ACTIVITY.READING_IDENTITY);
                mIdentity = mEid.getIdentity();
                setState();

                setActivity(ACTIVITY.READING_ADDRESS);
                mAddress = mEid.getAddress();
                setState();

                setActivity(ACTIVITY.READING_PHOTO);
                mPhoto = mEid.getPhoto();
                setState();

                setActivity(ACTIVITY.READING_AUTH_CHAIN);
                mAuthCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN,mEid.getAuthnCertificateChain());
                mTrustServiceController.validateLater(mAuthCertChain);
                setState();

                setActivity(ACTIVITY.READING_SIGN_CHAIN);
                mSignCertChain=new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN,mEid.getSignCertificateChain());
                mTrustServiceController.validateLater(mSignCertChain);
                setActivity(ACTIVITY.IDLE);

                while(mEid.isCardStillPresent())
                {
                    if(mRunningAction == ACTION.CHANGE_PIN)
                    {
                        eid_changePin();
                    }
                    else
                    {
                        try
                        {
                            Thread.sleep(1000);
                        }
                        catch (InterruptedException ex1)
                        {
                            Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex1);
                        }
                    }
                }

                clearCardInformation();
            }
            catch (Exception ex)   // something failed. Clear out all data for security
            {
                clearCardInformation();
                mRunningAction = ACTION.NONE;
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
        return mAddress;
    }

    public Identity getIdentity()
    {
        return mIdentity;
    }

    public Image getPhoto()
    {
        return mPhoto;
    }

    public STATE getState()
    {
        return mState;
    }

    public ACTIVITY getActivity()
    {
        return mActivity;
    }

    public boolean hasAddress()
    {
        return (mAddress != null);
    }

    public boolean hasIdentity()
    {
        return (mIdentity != null);
    }

    public boolean hasPhoto()
    {
        return (mPhoto != null);
    }

    public boolean hasAuthCertChain()
    {
        return (mAuthCertChain != null);
    }

    public X509CertificateChainAndTrust getAuthCertChain()
    {
        return mAuthCertChain;
    }

    public boolean hasSignCertChain()
    {
        return (mSignCertChain != null);
    }

    public X509CertificateChainAndTrust getSignCertChain()
    {
        return mSignCertChain;
    }

    public EidController changePin()
    {
        mRunningAction = ACTION.CHANGE_PIN;
        return this;
    }

    public boolean isReadyForCommand()
    {
        return (mState==STATE.EID_PRESENT) && (mActivity==ACTIVITY.IDLE) && (mRunningAction==ACTION.NONE);
    }
}
