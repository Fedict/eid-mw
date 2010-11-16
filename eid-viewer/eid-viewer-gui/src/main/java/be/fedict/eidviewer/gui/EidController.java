package be.fedict.eidviewer.gui;

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eidviewer.lib.Eid;
import java.awt.Image;
import java.util.Observable;
import java.util.Observer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author frank
 */
public class EidController extends Observable implements Runnable
{

    private boolean     mRunning = false;
    private Eid         mEid;
    private STATE       mState;
    private Identity    mIdentity;
    private Address     mAddress;
    private Image       mPhoto;

    private void clearCardInformation()
    {
        mIdentity=null;
        mAddress=null;
        mPhoto=null;
    }

    public static enum STATE
    { 
        IDLE("idle"), NO_READERS("noreaders"), NO_EID_PRESENT("noeidpresent"),EID_PRESENT("eidpresent");

        private final String state;
        private STATE(String state) { this.state = state; }
        public String getState() { return this.state; }
    };

    public EidController(Eid eid)
    {
        mEid = eid;
        setState(STATE.IDLE);
    }

    private void setState(STATE newState)
    {
        mState=newState;
        setState();
    }

    private void setState()
    {
        setChanged();
        notifyObservers();
    }

    public void start()
    {
        new Thread(this).start();
    }

    public void stop()
    {
        mRunning = false;
    }

    public void run()
    {
        mRunning = true;
        while(mRunning)
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
          
                setState(STATE.EID_PRESENT);

                mIdentity=mEid.getIdentity();
                setState();
                
                mAddress=mEid.getAddress();
                setState();

                mPhoto=mEid.getPhoto();
                setState();

                mEid.removeCard();
                clearCardInformation();
            }
            catch(Exception ex)
            {
                Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex);
                try
                {
                    Thread.sleep(500);
                }
                catch (InterruptedException ex1)
                {
                    Logger.getLogger(EidController.class.getName()).log(Level.SEVERE, null, ex1);
                }
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
    
    public boolean hasAddress()
    {
        return (mAddress!=null);
    }

    public boolean hasIdentity()
    {
        return (mIdentity!=null);
    }

    public boolean hasPhoto()
    {
        return (mPhoto!=null);
    }
}
