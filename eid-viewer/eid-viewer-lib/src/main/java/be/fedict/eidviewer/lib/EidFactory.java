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
