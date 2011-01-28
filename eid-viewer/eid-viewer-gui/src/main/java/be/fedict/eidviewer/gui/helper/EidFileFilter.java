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

import be.fedict.eidviewer.gui.file.EidFiles;
import java.io.File;
import javax.swing.filechooser.FileFilter;

/**
 *
 * @author Frank Marien
 */
public class EidFileFilter extends FileFilter
{
    private boolean acceptXML,acceptTLV,acceptCSV;
    private String  description;

    public EidFileFilter(boolean acceptXML, boolean acceptTLV, boolean acceptCSV, String description)
    {
        super();
        this.acceptXML =    acceptXML;
        this.acceptTLV =    acceptTLV;
        this.acceptCSV =    acceptCSV;
        this.description =  description;
    }

    public boolean accept(File file)
    {
        if(file.isDirectory())
            return true;

        if(acceptXML)
        {
            int xmlVersion=EidFiles.getXMLFileVersion(file);
            if(xmlVersion==3 || xmlVersion==4)
                return true;
        }

        if(acceptTLV)
        {
           if(EidFiles.isTLVEidFile(file))
               return true;
        }

        if(acceptCSV)
        {
            if(EidFiles.getCSVFileVersion(file)==1)
                return true;
        }

        return false;
    }

    public String getDescription()
    {
        return description;
    }
}
