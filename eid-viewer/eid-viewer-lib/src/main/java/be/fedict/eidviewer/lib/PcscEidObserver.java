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

import java.util.Observable;
import java.util.Observer;

/**
 *
 * @author frank
 */
class PcscEidObserver implements Observer
{
    public PcscEidObserver()
    {
    }

    public void update(Observable o, Object o1)
    {
        String oStr=(o!=null?o.toString():"null");
        String o1Str=(o1!=null?o1.toString():"null");
        System.err.println("update [" + oStr + "," + o1Str + "]");
    }
}
