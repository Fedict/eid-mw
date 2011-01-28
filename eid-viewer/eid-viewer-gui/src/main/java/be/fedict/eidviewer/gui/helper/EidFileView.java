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

import java.io.File;
import java.util.ResourceBundle;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.filechooser.FileView;

/**
 *
 * @author Frank Marien
 */
public class EidFileView extends FileView
{
    private static final String ICONS = "resources/icons/";
    private ResourceBundle bundle;
    private ImageIcon      eidIcon;

    public EidFileView(ResourceBundle bundle)
    {
        super();
        this.bundle = bundle;
        eidIcon=ImageUtilities.getIcon(this.getClass(),ICONS+"eid_small.png");
    }

    @Override
    public Icon getIcon(File file)
    {
        if(!file.isFile())
            return null;
        return eidIcon;
    }

    @Override
    public String getTypeDescription(File file)
    {
        if(!file.isFile())
            return null;
        return bundle.getString("eidFileDescription");
    }
}
