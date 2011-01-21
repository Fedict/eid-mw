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

import be.fedict.eid.applet.service.Address;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.zip.ZipOutputStream;
import org.simpleframework.xml.Element;

/**
 *
 * @author Frank Marien
 */
public final class Version4FileAddress
{
    @Element(name = "streetandnumber")
    private String streetAndNumber;
    @Element(name = "zip")
    private String zip;
    @Element(name = "municipality")
    private String municipality;

    public Version4FileAddress(Address address)
    {
        super();
        fromAddress(address);
    }

    public Version4FileAddress()
    {
        super();
    }

    public void fromAddress(Address eidAddress)
    {
        setStreetAndNumber(eidAddress.getStreetAndNumber());
        setZip(eidAddress.getZip());
        setMunicipality(eidAddress.getMunicipality());
    }

    public void toAddress(Address eidAddress)
    {
        eidAddress.streetAndNumber=getStreetAndNumber();
        eidAddress.zip=getZip();
        eidAddress.municipality=getMunicipality();
    }

    public void writeToZipOutputStream(ZipOutputStream zos) throws IOException
    {
        Charset utf8=Charset.forName("utf-8");
        Version4File.writeZipEntry(zos,utf8,"streetandnumber",getStreetAndNumber());
        Version4File.writeZipEntry(zos,utf8,"zip",getZip());
        Version4File.writeZipEntry(zos,utf8,"municipality",getMunicipality());
    }
    
    public String getMunicipality()
    {
        return municipality;
    }

    public void setMunicipality(String municipality)
    {
        this.municipality = municipality;
    }

    public String getStreetAndNumber()
    {
        return streetAndNumber;
    }

    public void setStreetAndNumber(String streetAndNumber)
    {
        this.streetAndNumber = streetAndNumber;
    }

    public String getZip()
    {
        return zip;
    }

    public void setZip(String zip)
    {
        this.zip = zip;
    }
}
