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
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.GregorianCalendar;
import java.util.List;
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
    public String nationalNumber;
    @Attribute(name="dateofbirth")
    public String dateOfBirth;
    @Attribute(name="gender")
    public String gender;
    
    @Attribute(name="noblecondition",required=false)
    public String nobleCondition;
    @Attribute(name="specialstatus",required=false)
    public String specialStatus;
    @Attribute(name="duplicate",required=false)
    public String duplicate;

    @Element(name="name")
    public String name;
    @Element(name="firstname")
    public String firstName;
    @Element(name="middlename")
    public String middleName;
    @Element(name="nationality")
    public String nationality;
    @Element(name="placeofbirth")
    public String placeOfBirth;
    @Element(name="photo")
    public String photo;
    @Element(name="photodigest")
    public String photoDigest;

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

        setName(eidIdentity.getName());
        setFirstName(eidIdentity.getFirstName());
        setMiddleName(eidIdentity.getMiddleName());
        setNationality(eidIdentity.getNationality());
        setPlaceOfBirth(eidIdentity.getPlaceOfBirth());
        setPhotoDigest(Base64.encodeBase64String(eidIdentity.getPhotoDigest()).trim());
        setPhoto(new String(Base64.encodeBase64(eidPhoto,false,false,8192)).trim());
    }

    public void toIdentity(Identity eidIdentity) throws ParseException
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
        eidIdentity.nationalNumber=getNationalNumber();

        GregorianCalendar       birthDateCalendar=new GregorianCalendar();
                                birthDateCalendar.setTime(dateFormat.parse(getDateOfBirth()));
        eidIdentity.dateOfBirth=birthDateCalendar;
        eidIdentity.gender=getGender().equals("M")?Gender.MALE:Gender.FEMALE;

        if(getNobleCondition()!=null)
            eidIdentity.nobleCondition=getNobleCondition();

        if(getSpecialStatus()!=null)
            eidIdentity.specialStatus=SpecialStatus.valueOf(getSpecialStatus());
        else
            eidIdentity.specialStatus=SpecialStatus.NO_STATUS;

        if(getDuplicate()!=null)
            eidIdentity.duplicate=getDuplicate();

        eidIdentity.name=getName();
        eidIdentity.firstName=getName();
        eidIdentity.middleName=getMiddleName();
        eidIdentity.nationality=getNationality();
        eidIdentity.placeOfBirth=getPlaceOfBirth();
        eidIdentity.photoDigest=Base64.decodeBase64(getPhotoDigest());
    }

    public byte[] toPhoto()
    {
        return Base64.decodeBase64(getPhoto());
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

    public String getPhotoDigest()
    {
        return photoDigest;
    }

    public void setPhotoDigest(String photoDigest)
    {
        this.photoDigest = photoDigest;
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
}
