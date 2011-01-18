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

import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

/*
 * PositiveIntegerDocument implements a PlainDocument, overriding insertString
 * to pre-process inputs, limiting input to maxSize digits. Set this as Document for
 * any text components ot limit their input.
 */
public class PositiveIntegerDocument extends PlainDocument
{
    private int maxSize;

    /*
     * PositiveIntegerDocument Constructor, takes maxSize, which is the maximum number
     * of digits this Document will allow.
     */
    public PositiveIntegerDocument(int maxSize)
    {
        super();
        this.maxSize=maxSize;
    }

    /*
     * insertString is called when characters are about to be inserted, this one limits input to
     * digits, and maximum maxSize of them. It will simple not call the superclass's insertString
     * otherwise, causing the insert to simply not happen.
     */
    @Override
    public void insertString(int offset, String str, AttributeSet attr) throws BadLocationException
    {
        if(isNumericString(str))
        {
            int currentSize=getLength();
            if(currentSize+str.length()>maxSize)
                str=str.substring(0,maxSize-currentSize);
            if(!str.isEmpty())
                super.insertString(offset, str, attr);
        }
    }

    // helper method returns true if only digits were found in string
    private boolean isNumericString(String string)
    {
        for(int i = 0; i < string.length(); i++)
            if(!Character.isDigit(string.charAt(i)))
                return false;
        return true;
    }
}

