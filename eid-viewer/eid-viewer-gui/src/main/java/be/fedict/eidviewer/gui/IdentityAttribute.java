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

/**
 *
 * @author frank
 */
public class IdentityAttribute
{
    private String label;
    private String value;
    private boolean relevant;

    public IdentityAttribute(String label, String value)
    {
        this.label = label;
        this.value = value;
        this.relevant = true;
    }

    public String getLabel()
    {
        return label;
    }

    public IdentityAttribute setLabel(String label)
    {
        this.label = label;
        return this;
    }

    public String getValue()
    {
        return value;
    }

    public IdentityAttribute setValue(String value)
    {
        this.value = value;
        return this;
    }

    public boolean isRelevant()
    {
        return relevant;
    }

    public IdentityAttribute setRelevant(boolean relevant)
    {
        this.relevant = relevant;
        return this;
    }
}
