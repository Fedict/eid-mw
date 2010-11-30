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

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Gender;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eid.applet.service.SpecialStatus;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.awt.print.*;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;
import javax.swing.ImageIcon;

/**
 *
 * @author frank
 */
public class IDPrintout implements Printable
{

    private static final int MINIMAL_FONT_SIZE = 6;
    private static final int MAXIMAL_FONT_SIZE = 48;
    private static final String ICONS = "resources/icons/";
    private static final String UNKNOWN_VALUE_TEXT = "-";
    private static final float SPACE_BETWEEN_ITEMS = 16;
    private static final String FONT = "Lucida";
    private static final IdentityAttribute SEPARATOR = null;
    private ResourceBundle bundle;
    private DateFormat dateFormat;
    private Identity identity;
    private Address address;
    private BufferedImage photo, coatOfArms;

    /** Creates new form IDPrintout */
    public IDPrintout()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/IDPrintout");
        dateFormat = DateFormat.getDateInstance(DateFormat.DEFAULT, Locale.getDefault());
        coatOfArms = getImage(bundle.getString("coatOfArms"));
    }

    public void setIdentity(Identity identity)
    {
        this.identity = identity;
    }

    public void setAddress(Address address)
    {
        this.address = address;
    }

    public void setPhoto(Image photo)
    {
        this.photo = getBufferedImage(photo);
    }

    public int print(Graphics graphics, PageFormat pageFormat, int pageNumber) throws PrinterException
    {
        if (pageNumber > 0)
            return Printable.NO_SUCH_PAGE;

        Graphics2D graphics2D = (Graphics2D) graphics;
        graphics2D.translate(pageFormat.getImageableX(), pageFormat.getImageableY());

        float imageableWidth = (float) pageFormat.getImageableWidth();
        float imageableHeight = (float) pageFormat.getImageableHeight();

        AffineTransform coatOfArmsTransform = new AffineTransform();
        coatOfArmsTransform.scale(0.5, 0.5);

        AffineTransform photoTransform = new AffineTransform();
        photoTransform.scale(0.5, 0.5);
        photoTransform.translate((imageableWidth*2)-(photo.getWidth()),0);

        graphics2D.setColor(Color.BLACK);
        graphics2D.drawImage(coatOfArms, coatOfArmsTransform, null);
        graphics2D.drawImage(photo, photoTransform, null);

        float headerHeight=coatOfArms.getHeight()/2;
        float coatOfArmsWidth=coatOfArms.getWidth()/2;
        float photoWidth=photo.getWidth()/2;
        float headerSpaceBetweenImages = imageableWidth - (coatOfArmsWidth + photoWidth + (SPACE_BETWEEN_ITEMS * 2));
        float headerSpaceHeight = (coatOfArms.getHeight() / 2);

        String[] cardTypeStr = (bundle.getString("type_" + this.identity.getDocumentType().toString())).split(";");


        boolean sizeFound = false;
        int fontSize;

        for (fontSize = MAXIMAL_FONT_SIZE; (fontSize >= MINIMAL_FONT_SIZE) && (!sizeFound); fontSize--)  // count down slowly until we find one that fits nicely
        {
            System.err.println("fontSize=" + fontSize);
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));
            sizeFound = (getTotalStringWidth(graphics2D, cardTypeStr) < headerSpaceBetweenImages)
                        && (getTotalStringHeight(graphics2D, cardTypeStr) < headerHeight);
        }

        if (sizeFound)
        {
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize + 1));
            float cardTypeHeight = cardTypeStr.length * getStringHeight(graphics2D);
            float cardTypeBaseLine = ((headerHeight - cardTypeHeight) / 2) + getAscent(graphics2D);
            float cardTypeLineHeight = getStringHeight(graphics2D);

            for (int i = 0; i < cardTypeStr.length; i++)
            {
                float left = (coatOfArmsWidth + SPACE_BETWEEN_ITEMS + (headerSpaceBetweenImages - getStringWidth(graphics2D, cardTypeStr[i])) / 2);
                float leading = (float) cardTypeLineHeight * i;
                graphics2D.drawString(cardTypeStr[i], left, cardTypeBaseLine + leading);
            }
        }
        ArrayList<IdentityAttribute> idAttributes = populateAttributeList();


        graphics2D.drawLine(0, (int) headerHeight, (int) imageableWidth, (int) headerHeight);


        float imageableDataHeight = imageableHeight - headerHeight;
        float totalDataWidth = 0, totalDataHeight = 0;
        float labelWidth, widestLabelWidth = 0;
        float valueWidth, widestValueWidth = 0;

        sizeFound = false;

        for (fontSize = MAXIMAL_FONT_SIZE; (fontSize >= MINIMAL_FONT_SIZE) && (!sizeFound); fontSize--)  // count down slowly until we find one that fits nicely
        {
            System.err.println("fontSize=" + fontSize);
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));

            widestLabelWidth = 0;
            widestValueWidth = 0;

            for (IdentityAttribute attribute : idAttributes)
            {
                if (attribute == SEPARATOR)
                    continue;

                labelWidth = getStringWidth(graphics2D, attribute.getLabel());
                valueWidth = getStringWidth(graphics2D, attribute.getValue());
                if (labelWidth > widestLabelWidth)
                    widestLabelWidth = labelWidth;
                if (valueWidth > widestValueWidth)
                    widestValueWidth = valueWidth;
            }

            totalDataWidth = widestLabelWidth + SPACE_BETWEEN_ITEMS + widestValueWidth;
            totalDataHeight = getStringHeight(graphics2D) * idAttributes.size();

            if ((totalDataWidth < imageableWidth) && (totalDataHeight < imageableDataHeight))
                sizeFound = true;
        }

        if (sizeFound)
        {
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));
            float labelsLeft = (imageableWidth - totalDataWidth) / 2;
            float valuesLeft = labelsLeft + widestLabelWidth + SPACE_BETWEEN_ITEMS;
            float dataTop = headerHeight + (((imageableHeight - headerHeight) - totalDataHeight) / 2);
            float dataLineHeight = getStringHeight(graphics2D);
            float lineNumber = 0;

            for (IdentityAttribute attribute : idAttributes)
            {
                if(attribute != SEPARATOR) // data
                {
                    graphics2D.setColor(attribute.isRelevant() ? Color.BLACK : Color.LIGHT_GRAY);
                    graphics2D.drawString(attribute.getLabel(), labelsLeft, dataTop + (lineNumber * dataLineHeight));
                    graphics2D.drawString(attribute.getValue(), valuesLeft, dataTop + (lineNumber * dataLineHeight));
                }
                else // separator
                {
                    int y = (int) (((dataTop + (lineNumber * dataLineHeight) + (dataLineHeight / 2))) - getAscent(graphics2D));
                    graphics2D.setColor(Color.BLACK);
                    graphics2D.drawLine((int) labelsLeft, y, (int) totalDataWidth, y);
                }
                lineNumber++;
            }
        }

        graphics2D.setColor(Color.BLACK);
        return Printable.PAGE_EXISTS;
    }
   
    private ArrayList<IdentityAttribute> populateAttributeList()
    {
        ArrayList<IdentityAttribute> idAttributes = new ArrayList();
        addIdAttribute(idAttributes, "nameLabel", identity.getName());
        addIdAttribute(idAttributes, "givenNamesLabel", identity.getFirstName() + " " + identity.getMiddleName());
        addIdAttribute(idAttributes, "placeOfBirthLabel", identity.getPlaceOfBirth());
        addIdAttribute(idAttributes, "dateOfBirthLabel", dateFormat.format(identity.getDateOfBirth().getTime()));
        addIdAttribute(idAttributes, "sexLabel", identity.getGender() == Gender.FEMALE ? bundle.getString("genderFemale") : bundle.getString("genderMale"));
        addIdAttribute(idAttributes, "nationalityLabel", identity.getNationality());
        addIdAttribute(idAttributes, "nationalNumberLabel", identity.getNationalNumber());
        idAttributes.add(SEPARATOR);
        String nobleCondition = identity.getNobleCondition();
        if (!nobleCondition.isEmpty())
        {
            addIdAttribute(idAttributes, "titleLabel", identity.getNationalNumber());
        }
        else
        {
            addIdAttribute(idAttributes, "titleLabel", UNKNOWN_VALUE_TEXT, false);
        }
        String specialStatusStr = getSpecialStatusString(identity.getSpecialStatus());
        if (!specialStatusStr.isEmpty())
        {
            addIdAttribute(idAttributes, "specialStatusLabel", specialStatusStr);
        }
        else
        {
            addIdAttribute(idAttributes, "specialStatusLabel", UNKNOWN_VALUE_TEXT, false);
        }
        idAttributes.add(SEPARATOR);
        addIdAttribute(idAttributes, "streetLabel", address.getStreetAndNumber());
        addIdAttribute(idAttributes, "postalCodeLabel", address.getZip());
        addIdAttribute(idAttributes, "municipalityLabel", address.getMunicipality());
        idAttributes.add(SEPARATOR);
        addIdAttribute(idAttributes, "cardNumberLabel", identity.getCardNumber());
        addIdAttribute(idAttributes, "placeOfIssueLabel", identity.getCardDeliveryMunicipality());
        addIdAttribute(idAttributes, "validFromLabel", dateFormat.format(identity.getCardValidityDateBegin().getTime()));
        addIdAttribute(idAttributes, "validUntilLabel", dateFormat.format(identity.getCardValidityDateEnd().getTime()));
        return idAttributes;
    }

    private String join(Collection s, String delimiter)
    {
        StringBuilder buffer = new StringBuilder();
        Iterator iter = s.iterator();
        if (iter.hasNext())
        {
            buffer.append(iter.next());
            while (iter.hasNext())
            {
                buffer.append(delimiter);
                buffer.append(iter.next());
            }
        }
        return buffer.toString();
    }

    private String getSpecialStatusString(SpecialStatus specialStatus)
    {
        List specials = new ArrayList();
        if (specialStatus.hasWhiteCane())
        {
            specials.add(bundle.getString("special_status_white_cane"));
        }
        if (specialStatus.hasYellowCane())
        {
            specials.add(bundle.getString("special_status_yellow_cane"));
        }
        if (specialStatus.hasExtendedMinority())
        {
            specials.add(bundle.getString("special_status_extended_minority"));
        }
        return join(specials, ",");
    }

    private BufferedImage getImage(String name)
    {
        Image image = new ImageIcon(Toolkit.getDefaultToolkit().getImage(BelgianEidViewer.class.getResource(ICONS + name))).getImage();
        return getBufferedImage(image);
    }

    private BufferedImage getBufferedImage(Image image)
    {
        int width = image.getWidth(null);
        int height = image.getHeight(null);
        BufferedImage bufferedImage = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        Graphics2D g2d = bufferedImage.createGraphics();
        g2d.drawImage(image, 0, 0, width, height, null);
        return bufferedImage;
    }

    private int getTotalStringWidth(Graphics2D graphics2D, String[] strings)
    {
        FontMetrics fontMetrics = graphics2D.getFontMetrics();
        int widest = 0;
        for (int i = 0; i < strings.length; i++)
        {
            int width = fontMetrics.stringWidth(strings[i]);
            if (width > widest)
            {
                widest = width;
            }
        }
        return widest;
    }

    private int getStringWidth(Graphics2D graphics2D, String string)
    {
        FontMetrics fontMetrics = graphics2D.getFontMetrics();
        return fontMetrics.stringWidth(string);
    }

    private float getTotalStringHeight(Graphics2D graphics2D, String[] strings)
    {
        FontMetrics fontMetrics = graphics2D.getFontMetrics();
        return fontMetrics.getHeight() * strings.length;
    }

    private float getStringHeight(Graphics2D graphics2D)
    {
        FontMetrics fontMetrics = graphics2D.getFontMetrics();
        return fontMetrics.getHeight();
    }

    private float getAscent(Graphics2D graphics2D)
    {
        FontMetrics fontMetrics = graphics2D.getFontMetrics();
        return fontMetrics.getAscent();
    }

    private void addIdAttribute(List list, String labelName, String value)
    {
        String labelStr = bundle.getString(labelName);
        list.add(new IdentityAttribute(labelStr, value));
    }

    private void addIdAttribute(List list, String labelName, String value, boolean relevant)
    {
        String labelStr = bundle.getString(labelName);
        list.add(new IdentityAttribute(labelStr, value).setRelevant(relevant));
    }
}
