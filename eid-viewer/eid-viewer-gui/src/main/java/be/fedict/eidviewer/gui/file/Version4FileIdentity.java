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

import be.fedict.eid.applet.service.Gender;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eid.applet.service.SpecialStatus;
import be.fedict.eidviewer.gui.helper.IdFormatHelper;
import java.io.IOException;
import java.nio.charset.Charset;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.GregorianCalendar;
import java.util.List;
import java.util.zip.ZipOutputStream;
import org.simpleframework.xml.Attribute;
import org.simpleframework.xml.Element;
import org.apache.commons.codec.binary.Base64;

/**
 *
 * @author Frank Marien
 */
public final class Version4FileIdentity
{
    @Attribute(name="nationalnumber")
    private String nationalNumber;
    @Attribute(name="dateofbirth")
    private String dateOfBirth;
    @Attribute(name="gender")
    private String gender;
    
    @Attribute(name="noblecondition",required=false)
    private String nobleCondition;
    @Attribute(name="specialstatus",required=false)
    private String specialStatus;
    @Attribute(name="duplicate",required=false)
    private String duplicate;

    @Element(name="name")
    private String name;
    @Element(name="firstname")
    private String firstName;
    @Element(name="middlename")
    private String middleName;
    @Element(name="nationality")
    private String nationality;
    @Element(name="placeofbirth")
    private String placeOfBirth;
    @Element(name="photo")
    private String photo;


    public Version4FileIdentity(Identity identity, byte[] photo)
    {
        super();
        fromIdentityAndPhoto(identity,photo);
    }

    public Version4FileIdentity()
    {
        super();
    }

    public void fromIdentityAndPhoto(Identity eidIdentity, byte[] eidPhoto)
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
        setNationalNumber(eidIdentity.getNationalNumber());
        setDateOfBirth(dateFormat.format(eidIdentity.getDateOfBirth().getTime()));
        setGender(eidIdentity.getGender()==Gender.MALE?"male":"female");

        if(eidIdentity.getNobleCondition() != null && (!eidIdentity.getNobleCondition().equals("")))
            setNobleCondition(eidIdentity.getNobleCondition());
           
        if(eidIdentity.getSpecialStatus() != null && eidIdentity.getSpecialStatus() != SpecialStatus.NO_STATUS)
        {
            SpecialStatus sStatus = eidIdentity.getSpecialStatus();
            List specials = new ArrayList();
            if (sStatus.hasWhiteCane())
                specials.add("whitecane");
            if (sStatus.hasYellowCane())
                specials.add("yellowcane");
            if (sStatus.hasExtendedMinority())
                specials.add("extendedminority");
            setSpecialStatus(IdFormatHelper.join(specials, ","));
        }
        
        if(eidIdentity.getDuplicate() != null && (!eidIdentity.getDuplicate().equals("")))
                setDuplicate(eidIdentity.getDuplicate());

        setName(                eidIdentity.getName());
        setFirstName(           eidIdentity.getFirstName());
        setMiddleName(          eidIdentity.getMiddleName());
        setNationality(         eidIdentity.getNationality());
        setPlaceOfBirth(        eidIdentity.getPlaceOfBirth());
        setPhotoJPEG(           eidPhoto);
    }

    public void toIdentity(Identity eidIdentity) throws ParseException
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
        eidIdentity.nationalNumber=getNationalNumber();

        GregorianCalendar       birthDateCalendar=new GregorianCalendar();
                                birthDateCalendar.setTime(dateFormat.parse(getDateOfBirth()));
        eidIdentity.dateOfBirth=birthDateCalendar;
        eidIdentity.gender=getGender().equals("male")?Gender.MALE:Gender.FEMALE;

        if(getNobleCondition()!=null)
            eidIdentity.nobleCondition=getNobleCondition();

        if(getSpecialStatus()!=null)
            eidIdentity.specialStatus=SpecialStatus.valueOf(getSpecialStatus());
        else
            eidIdentity.specialStatus=SpecialStatus.NO_STATUS;

        if(getDuplicate()!=null)
            eidIdentity.duplicate=getDuplicate();

        eidIdentity.name=getName();
        eidIdentity.firstName=getFirstName();
        eidIdentity.middleName=getMiddleName();
        eidIdentity.nationality=getNationality();
        eidIdentity.placeOfBirth=getPlaceOfBirth();
    }

    public byte[] toPhoto()
    {
        return getPhotoJPEG();
    }

    public void writeToZipOutputStream(ZipOutputStream zos) throws IOException
    {
        Charset utf8=Charset.forName("utf-8");
        
        Version4File.writeZipEntry(zos,utf8,"nationalnumber",getNationalNumber());
        Version4File.writeZipEntry(zos,utf8,"dateofbirth",getDateOfBirth());
        Version4File.writeZipEntry(zos,utf8,"gender",getGender());
       
        if(getNobleCondition()!=null)
            Version4File.writeZipEntry(zos,utf8,"noblecondition",getNobleCondition());

        if(getSpecialStatus()!=null)
            Version4File.writeZipEntry(zos,utf8,"specialstatus",getSpecialStatus());

        if(getDuplicate()!=null)
            Version4File.writeZipEntry(zos,utf8,"duplicate", getDuplicate());

        Version4File.writeZipEntry(zos,utf8,"name",          getName());
        Version4File.writeZipEntry(zos,utf8,"firstname",     getFirstName());
        Version4File.writeZipEntry(zos,utf8,"middlename",    getMiddleName());
        Version4File.writeZipEntry(zos,utf8,"nationality",   getNationality());
        Version4File.writeZipEntry(zos,utf8,"placeofbirth",  getPlaceOfBirth());
        Version4File.writeZipEntry(zos,     "photo",         getPhotoJPEG());
    }

   
    
    public String getDateOfBirth()
    {
        return dateOfBirth;
    }

    public void setDateOfBirth(String dateOfBirth)
    {
        this.dateOfBirth = dateOfBirth;
    }

    public String getDuplicate()
    {
        return duplicate;
    }

    public void setDuplicate(String duplicate)
    {
        this.duplicate = duplicate;
    }

    public String getFirstName()
    {
        return firstName;
    }

    public void setFirstName(String firstName)
    {
        this.firstName = firstName;
    }

    public String getGender()
    {
        return gender;
    }

    public void setGender(String gender)
    {
        this.gender = gender;
    }

    public String getMiddleName()
    {
        return middleName;
    }

    public void setMiddleName(String middleName)
    {
        this.middleName = middleName;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public String getNationalNumber()
    {
        return nationalNumber;
    }

    public void setNationalNumber(String nationalNumber)
    {
        this.nationalNumber = nationalNumber;
    }

    public String getNationality()
    {
        return nationality;
    }

    public void setNationality(String nationality)
    {
        this.nationality = nationality;
    }

    public String getNobleCondition()
    {
        return nobleCondition;
    }

    public void setNobleCondition(String nobleCondition)
    {
        this.nobleCondition = nobleCondition;
    }

    public String getPlaceOfBirth()
    {
        return placeOfBirth;
    }

    public void setPlaceOfBirth(String placeOfBirth)
    {
        this.placeOfBirth = placeOfBirth;
    }

    public String getSpecialStatus()
    {
        return specialStatus;
    }

    public void setSpecialStatus(String specialStatus)
    {
        this.specialStatus = specialStatus;
    }

    public String getPhoto()
    {
        return photo;
    }

    public void setPhoto(String photo)
    {
        this.photo = photo;
    }

    public byte[] getPhotoJPEG()
    {
        return Base64.decodeBase64(getPhoto());
    }

    public void setPhotoJPEG(byte[] photo)
    {
        setPhoto(new String(Base64.encodeBase64(photo, false, false)));
    }
}
