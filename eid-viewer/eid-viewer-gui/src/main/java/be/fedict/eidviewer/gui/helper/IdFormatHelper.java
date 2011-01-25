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

package be.fedict.eidviewer.gui.helper;

import be.fedict.eid.applet.service.Gender;
import be.fedict.eid.applet.service.SpecialStatus;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ResourceBundle;

public class IdFormatHelper
{
    public static final String UNKNOWN_VALUE_TEXT = "-";
    
    // return whatever text is available for gender in bundle
    public static String getGenderString(ResourceBundle bundle, Gender gender)
    {
        return gender==Gender.FEMALE ? bundle.getString("genderFemale") : bundle.getString("genderMale");
    }

    // return special stati, comma separated
    public static String getSpecialStatusString(ResourceBundle bundle, SpecialStatus specialStatus)
    {
        List specials = new ArrayList();

        if(specialStatus!=null)
        {
            if (specialStatus.hasWhiteCane())
                specials.add(bundle.getString("special_status_white_cane"));
            if (specialStatus.hasYellowCane())
                specials.add(bundle.getString("special_status_yellow_cane"));
            if (specialStatus.hasExtendedMinority())
                specials.add(bundle.getString("special_status_extended_minority"));
        }
        return IdFormatHelper.join(specials, ",");
    }
    
    // join a la python etc..
    public static String join(Collection s, String delimiter)
    {
        StringBuilder buffer = new StringBuilder();
        Iterator iter = s.iterator();
        if (iter.hasNext())
        {
            buffer.append(iter.next());
            while (iter.hasNext())
            {
                buffer.append(delimiter);
                buffer.append(iter.next());
            }
        }
        return buffer.toString();
    }
}
