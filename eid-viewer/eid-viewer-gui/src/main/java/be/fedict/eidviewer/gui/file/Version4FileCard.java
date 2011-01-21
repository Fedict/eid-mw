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

import be.fedict.eid.applet.service.DocumentType;
import be.fedict.eid.applet.service.Identity;
import java.io.IOException;
import java.nio.charset.Charset;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.GregorianCalendar;
import java.util.zip.ZipOutputStream;
import org.simpleframework.xml.Attribute;
import org.simpleframework.xml.Element;

/**
 *
 * @author Frank Marien
 */
public final class Version4FileCard
{
    @Attribute(name="documenttype")
    public String documentType;
    @Attribute(name="cardnumber")
    public String cardnumber;
    @Attribute(name="chipnumber")
    public String chipNumber;
    @Attribute(name="validitydatebegin")
    public String validityDateBegin;
    @Attribute(name="validitydateend")
    public String validityDateEnd;
    @Element(name="deliverymunicipality")
    public String deliveryMunicipality;

    public Version4FileCard(Identity identity)
    {
        super();
        fromIdentity(identity);
    }

    public Version4FileCard()
    {
        super();
    }

    public void fromIdentity(Identity identity)
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
        setDocumentType(identity.getDocumentType().toString().toLowerCase());
        setCardNumber(identity.getCardNumber());
        setChipNumber(identity.getChipNumber());
        setValidityDateBegin(dateFormat.format(identity.getCardValidityDateBegin().getTime()));
        setValidityDateEnd(dateFormat.format(identity.getCardValidityDateEnd().getTime()));
        setDeliveryMunicipality(identity.getCardDeliveryMunicipality());
    }

    public void toIdentity(Identity eidIdentity) throws ParseException
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");

        eidIdentity.documentType=DocumentType.valueOf(getDocumentType().toUpperCase());
        eidIdentity.cardNumber=getCardNumber();
        eidIdentity.chipNumber=getChipNumber();

        GregorianCalendar                   validityStartCalendar=new GregorianCalendar();
        
                                            validityStartCalendar.setTime(dateFormat.parse(getValidityDateBegin()));
        eidIdentity.cardValidityDateBegin=  validityStartCalendar;

        GregorianCalendar                   validityEndCalendar=new GregorianCalendar();
                                            validityEndCalendar.setTime(dateFormat.parse(getValidityDateEnd()));
        eidIdentity.cardValidityDateEnd=    validityEndCalendar;

        eidIdentity.cardDeliveryMunicipality=getDeliveryMunicipality();
    }

    public void writeToZipOutputStream(ZipOutputStream zos) throws IOException
    {
        Charset utf8=Charset.forName("utf-8");
        Version4File.writeZipEntry(zos,utf8,"carddocumenttype",getDocumentType());
        Version4File.writeZipEntry(zos,utf8,"cardnumber",getCardNumber());
        Version4File.writeZipEntry(zos,utf8,"cardchipnumber",getChipNumber());
        Version4File.writeZipEntry(zos,utf8,"cardvaliditydatebegin",getValidityDateBegin());
        Version4File.writeZipEntry(zos,utf8,"cardvaliditydateend",getValidityDateEnd());
        Version4File.writeZipEntry(zos,utf8,"carddeliveringmunicipality",getDeliveryMunicipality());
    }

    public String getDeliveryMunicipality()
    {
        return deliveryMunicipality;
    }

    public void setDeliveryMunicipality(String deliveryMunicipality)
    {
        this.deliveryMunicipality = deliveryMunicipality;
    }

    public String getCardNumber()
    {
        return cardnumber;
    }

    public void setCardNumber(String cardNumber)
    {
        this.cardnumber = cardNumber;
    }

    public String getValidityDateBegin()
    {
        return validityDateBegin;
    }

    public void setValidityDateBegin(String validityDateBegin)
    {
        this.validityDateBegin = validityDateBegin;
    }

    public String getValidityDateEnd()
    {
        return validityDateEnd;
    }

    public void setValidityDateEnd(String validityDateEnd)
    {
        this.validityDateEnd = validityDateEnd;
    }

    public String getChipNumber()
    {
        return chipNumber;
    }

    public void setChipNumber(String chipNumber)
    {
        this.chipNumber = chipNumber;
    }

    public String getDocumentType()
    {
        return documentType;
    }

    public void setDocumentType(String documentType)
    {
        this.documentType = documentType;
    }
}
