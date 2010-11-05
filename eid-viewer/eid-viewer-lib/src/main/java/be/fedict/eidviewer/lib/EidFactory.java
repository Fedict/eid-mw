package be.fedict.eidviewer.lib;

import be.fedict.eid.applet.Messages;
import be.fedict.eid.applet.View;
import java.util.logging.Level;
import java.util.logging.Logger;

public class EidFactory
{
    public static Eid getPCSCEidImpl(View view, Messages messages)
    {
        Eid eid=new PCSCEidImpl(view, messages);
        Logger.getLogger(EidFactory.class.getName()).log(Level.INFO, "PCSC Eid Implementation Selected");
        return eid;
    }

    public static Eid getPKCS11EidImpl(View view, Messages messages)
    {
        Eid eid=new PKCS11EidImpl(view, messages);
        Logger.getLogger(EidFactory.class.getName()).log(Level.INFO, "PKCS#11 Eid Implementation Selected");
        return eid;
    }

    public static Eid getEidImpl(View view, Messages messages)
    {
        Eid eid=null;
        
        try
        {
            eid=getPCSCEidImpl(view, messages);
        }
        catch (RuntimeException rte)
        {
            eid=getPKCS11EidImpl(view, messages);  
        }

        return eid;
    }
}
