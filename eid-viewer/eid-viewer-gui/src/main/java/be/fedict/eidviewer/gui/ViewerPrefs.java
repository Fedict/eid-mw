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

import java.util.prefs.Preferences;

/**
 *
 * @author Frank Marien
 */
public class ViewerPrefs
{
    public static final String AUTO_VALIDATE_TRUST              = "autoValidateTrust";
    public static final String TRUSTSERVICE_URL                 = "trustServiceURL";

    public static final boolean DEFAULT_AUTO_VALIDATE_TRUST     = false;
    public static final String  DEFAULT_TRUSTSERVICE_URL        ="http://trust.services.belgium.be"; 
    
    private static Preferences preferences;

    private static Preferences getPreferences()
    {
        if(preferences==null)
            preferences=Preferences.userNodeForPackage(ViewerPrefs.class);
        return preferences;
    }

    public static boolean getIsAutoValidating()
    {
        return getPreferences()!=null?getPreferences().getBoolean(AUTO_VALIDATE_TRUST, DEFAULT_AUTO_VALIDATE_TRUST):DEFAULT_AUTO_VALIDATE_TRUST;
    }

    public static void setAutoValidating(boolean state)
    {
        if(getPreferences()==null)
            return;
        getPreferences().putBoolean(AUTO_VALIDATE_TRUST, state);
    }

    public static String getTrustServiceURL()
    {
        return getPreferences()!=null?getPreferences().get(TRUSTSERVICE_URL, DEFAULT_TRUSTSERVICE_URL):DEFAULT_TRUSTSERVICE_URL;
    }

    public static void setTrustServiceURL(String url)
    {
        if(getPreferences()==null)
            return;
        getPreferences().put(TRUSTSERVICE_URL, url);
    }
}

