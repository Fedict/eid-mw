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
import java.io.InputStream;

/**
 *
 * @author Frank Marien
 */
public class TLVEntry
{
    public byte    tag;
    public int     length;
    public byte[]  data;

    public static TLVEntry next(InputStream is) throws IOException
    {
        byte tag = (byte) is.read();

        if(tag==-1)
            return null;

        byte lengthByte = (byte) is.read();

        int length = lengthByte & 0x7f;
        while ((lengthByte & 0x80) == 0x80)
        {
                lengthByte = (byte) is.read();
                length = (length << 7) + (lengthByte & 0x7f);
        }

        TLVEntry entry=new TLVEntry();
                 entry.tag=tag;
                 entry.length=length;
                 entry.data=new byte[length];

        is.read(entry.data);

       return entry;
    }
}
