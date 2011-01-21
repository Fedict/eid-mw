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

package be.fedict.eidviewer.gui.file;

import java.io.IOException;
import java.io.OutputStream;
import java.util.zip.ZipOutputStream;

/**
 *
 * @author Frank Marien
 */
public class CloseResistantZipOutputStream extends ZipOutputStream
{
    private boolean closeAllowed;

    public CloseResistantZipOutputStream(OutputStream out)
    {
        super(out);
    }

    @Override
    public void close() throws IOException
    {
        if(closeAllowed)
            super.close();
    }

    public void setCloseAllowed(boolean closeAllowed)
    {
        this.closeAllowed = closeAllowed;
    }
}
