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

import be.fedict.eidviewer.gui.helper.IdFormatHelper;
import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eidviewer.gui.helper.ImageUtilities;
import be.fedict.eidviewer.gui.helper.PrintingUtilities;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.geom.AffineTransform;
import java.awt.image.ImageObserver;
import java.awt.print.*;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Frank Marien
 */
public class IDPrintout implements Printable,ImageObserver
{
    private static final Logger             logger=Logger.getLogger(CertificatesPanel.class.getName());
    
    private static final int                MINIMAL_FONT_SIZE = 6;
    private static final int                MAXIMAL_FONT_SIZE = 48;
    private static final int                TITLE_MAXIMAL_FONT_SIZE = 32;
    private static final String             ICONS = "resources/icons/";
    private static final float              SPACE_BETWEEN_ITEMS = 16;
    private static final String             FONT = "Lucida";
    private static final IdentityAttribute  SEPARATOR = null;
    
    private ResourceBundle  bundle;
    private DateFormat      dateFormat;
    private Identity        identity;
    private Address         address;
    private Image           photo, coatOfArms;

    public IDPrintout()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/IDPrintout");
        dateFormat = DateFormat.getDateInstance(DateFormat.LONG, Locale.getDefault());
        coatOfArms = ImageUtilities.getImage(IDPrintout.class, ICONS+bundle.getString("coatOfArms"));
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
        this.photo = photo;
    }

    public int print(Graphics graphics, PageFormat pageFormat, int pageNumber) throws PrinterException
    {
        // we only support printing all in one single page
        if (pageNumber > 0)
            return Printable.NO_SUCH_PAGE;

        // translate graphics2D with origin at top left first imageable location
        Graphics2D graphics2D = (Graphics2D) graphics;
        graphics2D.translate(pageFormat.getImageableX(), pageFormat.getImageableY());

        // keep imageable width and height as variables for clarity (we use them often)
        float imageableWidth = (float) pageFormat.getImageableWidth();
        float imageableHeight = (float) pageFormat.getImageableHeight();

        // Coat of Arms images are stored at approx 36 DPI, scale 1/2 to get to Java default of 72DPI
        AffineTransform coatOfArmsTransform = new AffineTransform();
        coatOfArmsTransform.scale(0.5, 0.5);

        // photo images are stored at approx 36 DPI, scale 1/2 to get to Java default of 72DPI
        AffineTransform photoTransform = new AffineTransform();
        photoTransform.scale(0.5, 0.5);
        photoTransform.translate((imageableWidth*2)-(photo.getWidth(this)),0);

        // make sure foreground is black, and draw coat of Arms and photo at the top of the page
        // using the transforms to scale them to 72DPI.
        graphics2D.setColor(Color.BLACK);
        graphics2D.drawImage(coatOfArms, coatOfArmsTransform, null);
        graphics2D.drawImage(photo, photoTransform, null);

        // calculate some sizes that need to take into account the scaling of the graphics, to avoid dragging
        // those non-intuitive "/2" further along in the code.
        float headerHeight=coatOfArms.getHeight(this)/2;
        float coatOfArmsWidth=coatOfArms.getWidth(this)/2;
        float photoWidth=photo.getWidth(this)/2;
        float headerSpaceBetweenImages = imageableWidth - (coatOfArmsWidth + photoWidth + (SPACE_BETWEEN_ITEMS * 2));

        // get localised strings for card type. We'll take a new line every time a ";" is found in the resource
        String[] cardTypeStr = (bundle.getString("type_" + this.identity.getDocumentType().toString()).toUpperCase()).split(";");

        // iterate from MAXIMAL_FONT_SIZE, calculating how much space would be required to fit the card type strings
        // stop when a font size is found where they all fit the space between the graphics in an orderly manner
        boolean sizeFound = false;
        int fontSize;
        for(fontSize = TITLE_MAXIMAL_FONT_SIZE; (fontSize >= MINIMAL_FONT_SIZE) && (!sizeFound); fontSize--)  // count down slowly until we find one that fits nicely
        {
            logger.log(Level.FINE, "fontSize={0}", fontSize);
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));
            sizeFound = (PrintingUtilities.getTotalStringWidth(graphics2D, cardTypeStr) < headerSpaceBetweenImages)
                        && (PrintingUtilities.getTotalStringHeight(graphics2D, cardTypeStr) < headerHeight);
        }

        // unless with extremely small papers, a size should always have been found.
        // draw the card type strings, centered, between the images at the top of the page
        if(sizeFound)
        {
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize + 1));
            float cardTypeHeight = cardTypeStr.length * PrintingUtilities.getStringHeight(graphics2D);
            float cardTypeBaseLine = ((headerHeight - cardTypeHeight) / 2) + PrintingUtilities.getAscent(graphics2D);
            float cardTypeLineHeight = PrintingUtilities.getStringHeight(graphics2D);

            for (int i = 0; i < cardTypeStr.length; i++)
            {
                float left = (coatOfArmsWidth + SPACE_BETWEEN_ITEMS + (headerSpaceBetweenImages - PrintingUtilities.getStringWidth(graphics2D, cardTypeStr[i])) / 2);
                float leading = (float) cardTypeLineHeight * i;
                graphics2D.drawString(cardTypeStr[i], left, cardTypeBaseLine + leading);
            }
        }

        // populate idAttributes with all the information from identity and address
        // as well as date printed and some separators
        List<IdentityAttribute> idAttributes = populateAttributeList();

        // draw a horizontal line just below the header (images + card type titles)
        graphics2D.drawLine(0, (int) headerHeight, (int) imageableWidth, (int) headerHeight);

        // calculate how much space is left between the header and the bottom of the imageable area
        float imageableDataHeight = imageableHeight - headerHeight;
        float totalDataWidth = 0, totalDataHeight = 0;
        float labelWidth, widestLabelWidth = 0;
        float valueWidth, widestValueWidth = 0;

        // iterate from MAXIMAL_FONT_SIZE, calculating how much space would be required to fit the information in idAttributes into
        // the space between the header and the bottom of the imageable area
        // stop when a font size is found where it all fits in an orderly manner
        sizeFound = false;
        for (fontSize = MAXIMAL_FONT_SIZE; (fontSize >= MINIMAL_FONT_SIZE) && (!sizeFound); fontSize--)  // count down slowly until we find one that fits nicely
        {
            logger.log(Level.FINE, "fontSize={0}", fontSize);
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));

            widestLabelWidth = 0;
            widestValueWidth = 0;

            for (IdentityAttribute attribute : idAttributes)
            {
                if (attribute == SEPARATOR)
                    continue;

                labelWidth = PrintingUtilities.getStringWidth(graphics2D, attribute.getLabel());
                valueWidth = PrintingUtilities.getStringWidth(graphics2D, attribute.getValue());
                if (labelWidth > widestLabelWidth)
                    widestLabelWidth = labelWidth;
                if (valueWidth > widestValueWidth)
                    widestValueWidth = valueWidth;
            }

            totalDataWidth = widestLabelWidth + SPACE_BETWEEN_ITEMS + widestValueWidth;
            totalDataHeight = PrintingUtilities.getStringHeight(graphics2D) + (PrintingUtilities.getStringHeight(graphics2D) * idAttributes.size());

            if ((totalDataWidth < imageableWidth) && (totalDataHeight < imageableDataHeight))
                sizeFound = true;
        }

        // unless with extremely small papers, a size should always have been found.
        // draw the identity, addess and date printed information, in 2 columns, centered inside the
        // space between the header and the bottom of the imageable area
        if(sizeFound)
        {
            graphics2D.setFont(new Font(FONT, Font.PLAIN, fontSize));
            float labelsLeft = (imageableWidth - totalDataWidth) / 2;
            float valuesLeft = labelsLeft + widestLabelWidth + SPACE_BETWEEN_ITEMS;
            float dataLineHeight = PrintingUtilities.getStringHeight(graphics2D);
            float dataTop =  dataLineHeight+headerHeight + ((imageableDataHeight-totalDataHeight) / 2);
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
                    int y = (int) (((dataTop + (lineNumber * dataLineHeight) + (dataLineHeight / 2))) - PrintingUtilities.getAscent(graphics2D));
                    graphics2D.setColor(Color.BLACK);
                    graphics2D.drawLine((int) labelsLeft, y, (int)(labelsLeft+totalDataWidth), y);
                }
                lineNumber++;
            }
        }

        // tell Java printing that all this makes for a page worth printing :-)
        return Printable.PAGE_EXISTS;
    }

    /*
     * pull the information using getters, store it in a List, so that we can easily iterate over it
     * to determine sizes etc.. we add separators here, because they have to be included in vertical space calculations.
     */
    private List<IdentityAttribute> populateAttributeList()
    {
        ArrayList<IdentityAttribute> idAttributes = new ArrayList();

        addIdAttribute(idAttributes, "nameLabel",           identity.getName());
        addIdAttribute(idAttributes, "givenNamesLabel",     identity.getFirstName() + " " + identity.getMiddleName());
        addIdAttribute(idAttributes, "placeOfBirthLabel",   identity.getPlaceOfBirth());
        addIdAttribute(idAttributes, "dateOfBirthLabel",    dateFormat.format(identity.getDateOfBirth().getTime()));
        addIdAttribute(idAttributes, "sexLabel",            IdFormatHelper.getGenderString(bundle,identity.getGender()));
        addIdAttribute(idAttributes, "nationalityLabel",    identity.getNationality());
        addIdAttribute(idAttributes, "nationalNumberLabel", identity.getNationalNumber());

        idAttributes.add(SEPARATOR);

        String nobleCondition = identity.getNobleCondition();
        addIdAttribute(idAttributes, "titleLabel",          (nobleCondition==null || nobleCondition.isEmpty())?IdFormatHelper.UNKNOWN_VALUE_TEXT:nobleCondition, (!nobleCondition.isEmpty()));
        String specialStatusStr = IdFormatHelper.getSpecialStatusString(bundle,identity.getSpecialStatus());
        addIdAttribute(idAttributes, "specialStatusLabel",  specialStatusStr.isEmpty()?IdFormatHelper.UNKNOWN_VALUE_TEXT:specialStatusStr, (!specialStatusStr.isEmpty()));
       
        idAttributes.add(SEPARATOR);

        addIdAttribute(idAttributes, "streetLabel",         address.getStreetAndNumber());
        addIdAttribute(idAttributes, "postalCodeLabel",     address.getZip());
        addIdAttribute(idAttributes, "municipalityLabel",   address.getMunicipality());

        idAttributes.add(SEPARATOR);

        addIdAttribute(idAttributes, "cardNumberLabel",     identity.getCardNumber());
        addIdAttribute(idAttributes, "placeOfIssueLabel",   identity.getCardDeliveryMunicipality());
        addIdAttribute(idAttributes, "validFromLabel",      dateFormat.format(identity.getCardValidityDateBegin().getTime()));
        addIdAttribute(idAttributes, "validUntilLabel",     dateFormat.format(identity.getCardValidityDateEnd().getTime()));

        idAttributes.add(SEPARATOR);

        addIdAttribute(idAttributes, "printedDateLabel",    dateFormat.format(new Date()));
        
        return idAttributes;
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

    public boolean imageUpdate(Image image, int i, int i1, int i2, int i3, int i4)
    {
        logger.log(Level.FINEST, "ImageUpdate{0}{1}{2}{3}{4}", new Object[]{i, i1, i2, i3, i4});
        return true;
    }

    private class IdentityAttribute
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
}
