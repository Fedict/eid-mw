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
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
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
public class JPanelIDPrintout extends javax.swing.JPanel implements Printable
{

    private static final String UNKNOWN_VALUE_TEXT = "-";
    private ResourceBundle bundle;
    private DateFormat dateFormat;

    /** Creates new form IDPrintout */
    public JPanelIDPrintout()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/IDPrintout");
        dateFormat = DateFormat.getDateInstance(DateFormat.DEFAULT, Locale.getDefault());
        initComponents();
    }

    public void setIdentity(final Identity identity)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                name.setText(identity.getName());
                name.setEnabled(true);
                nameLabel.setEnabled(true);

                givenNames.setText(identity.getFirstName() + " " + identity.getMiddleName());
                givenNames.setEnabled(true);
                givenNamesLabel.setEnabled(true);

                placeOfBirth.setText(identity.getPlaceOfBirth());
                placeOfBirth.setEnabled(true);
                placeOfBirthLabel.setEnabled(true);

                dateOfBirth.setText(dateFormat.format(identity.getDateOfBirth().getTime()));
                dateOfBirth.setEnabled(true);
                dateOfBirthLabel.setEnabled(true);

                sex.setText(identity.getGender() == Gender.FEMALE ? bundle.getString("genderFemale") : bundle.getString("genderMale"));
                sex.setEnabled(true);
                sexLabel.setEnabled(true);

                nationality.setText(identity.getNationality());
                nationality.setEnabled(true);
                nationalityLabel.setEnabled(true);

                nationalNumber.setText(identity.getNationalNumber());
                nationalNumber.setEnabled(true);
                nationalNumberLabel.setEnabled(true);

                String nobleCondition = identity.getNobleCondition();
                if (!nobleCondition.isEmpty())
                {
                    title.setText(identity.getNobleCondition());
                    title.setVisible(true);
                    titleLabel.setEnabled(true);
                }
                else
                {
                    title.setText(UNKNOWN_VALUE_TEXT);
                    titleLabel.setEnabled(false);
                }

                String specialStatusStr = getSpecialStatusString(identity.getSpecialStatus());
                if (!specialStatusStr.isEmpty())
                {
                    specialStatus.setText(specialStatusStr);
                    specialStatus.setEnabled(true);
                    specialStatusLabel.setEnabled(true);
                }
                else
                {
                    specialStatus.setText(UNKNOWN_VALUE_TEXT);
                    specialStatus.setEnabled(false);
                    specialStatusLabel.setEnabled(false);
                }
            }
        });
    }

    public void setAddress(final Address address)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                street.setText(address.getStreetAndNumber());
                street.setEnabled(true);
                streetLabel.setEnabled(true);

                postalCode.setText(address.getZip());
                postalCode.setEnabled(true);
                postalCodeLabel.setEnabled(true);

                municipality.setText(address.getMunicipality());
                municipality.setEnabled(true);
                municipalityLabel.setEnabled(true);
            }
        });
    }

    public void setCardInfo(final Identity identity)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {

                cardNumber.setText(identity.getCardNumber());
                cardNumber.setEnabled(true);
                cardNumberLabel.setEnabled(true);

                placeOfIssue.setText(identity.getCardDeliveryMunicipality());
                placeOfIssue.setEnabled(true);
                placeOfIssueLabel.setEnabled(true);

                chipNumber.setText(identity.getChipNumber());
                chipNumber.setEnabled(true);
                chipNumberLabel.setEnabled(true);

                validFrom.setText(dateFormat.format(identity.getCardValidityDateBegin().getTime()));
                validFrom.setEnabled(true);
                validFromLabel.setEnabled(true);

                validUntil.setText(dateFormat.format(identity.getCardValidityDateEnd().getTime()));
                validUntil.setEnabled(true);
                validUntilLabel.setEnabled(true);
            }
        });
    }



    public void setPhoto(final Image image)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                photo.setIcon(new ImageIcon(image));
            }
        });
    }

    public int print(Graphics graphics, PageFormat pageFormat, int pageNumber) throws PrinterException
    {
        if(pageNumber > 0)
            return Printable.NO_SUCH_PAGE;
        Graphics2D graphics2D = (Graphics2D) graphics;
        graphics2D.translate(pageFormat.getImageableX(), pageFormat.getImageableY());
        //graphics2D.scale(.5, .5);

        /*JFrame frame = new JFrame("dummyFrameForPrinting");
        frame.add(this);
        
        frame.setPreferredSize(new Dimension((int)pageFormat.getImageableWidth(),(int)pageFormat.getImageableHeight()));
        frame.pack();
        frame.setVisible(true);

        revalidate();

        this.printChildren(graphics2D); */


        graphics2D.drawString("This is a TEST", 20, 20);

        graphics2D.drawRect(0, 0, (int)pageFormat.getImageableWidth(), (int)pageFormat.getImageableHeight());


        return Printable.PAGE_EXISTS;
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        picturesPanel = new javax.swing.JPanel();
        coatOfArmsLabel = new javax.swing.JLabel();
        photo = new javax.swing.JLabel();
        cardType = new javax.swing.JLabel();
        dataPanel = new javax.swing.JPanel();
        nameLabel = new javax.swing.JLabel();
        givenNamesLabel = new javax.swing.JLabel();
        placeOfBirthLabel = new javax.swing.JLabel();
        dateOfBirthLabel = new javax.swing.JLabel();
        nationalityLabel = new javax.swing.JLabel();
        nationalNumberLabel = new javax.swing.JLabel();
        sexLabel = new javax.swing.JLabel();
        titleLabel = new javax.swing.JLabel();
        specialStatusLabel = new javax.swing.JLabel();
        streetLabel = new javax.swing.JLabel();
        postalCodeLabel = new javax.swing.JLabel();
        municipalityLabel = new javax.swing.JLabel();
        addressCardDataSeparator = new javax.swing.JSeparator();
        name = new javax.swing.JLabel();
        givenNames = new javax.swing.JLabel();
        placeOfBirth = new javax.swing.JLabel();
        dateOfBirth = new javax.swing.JLabel();
        sex = new javax.swing.JLabel();
        nationalNumber = new javax.swing.JLabel();
        nationality = new javax.swing.JLabel();
        title = new javax.swing.JLabel();
        specialStatus = new javax.swing.JLabel();
        street = new javax.swing.JLabel();
        postalCode = new javax.swing.JLabel();
        municipality = new javax.swing.JLabel();
        spacer = new javax.swing.JLabel();
        validFrom = new javax.swing.JLabel();
        spacer1 = new javax.swing.JLabel();
        chipNumber = new javax.swing.JLabel();
        validFromLabel = new javax.swing.JLabel();
        validUntil = new javax.swing.JLabel();
        chipNumberLabel = new javax.swing.JLabel();
        placeOfIssue = new javax.swing.JLabel();
        placeOfIssueLabel = new javax.swing.JLabel();
        validUntilLabel = new javax.swing.JLabel();
        cardNumber = new javax.swing.JLabel();
        cardNumberLabel = new javax.swing.JLabel();
        idAddressSeparator = new javax.swing.JSeparator();

        setBackground(new java.awt.Color(255, 255, 255));
        setDebugGraphicsOptions(javax.swing.DebugGraphics.LOG_OPTION);
        setLayout(new java.awt.BorderLayout());

        picturesPanel.setBackground(new java.awt.Color(255, 255, 255));
        picturesPanel.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 48, 1));
        picturesPanel.setName("picturesPanel"); // NOI18N
        picturesPanel.setLayout(new java.awt.GridBagLayout());

        coatOfArmsLabel.setBackground(new java.awt.Color(255, 255, 255));
        coatOfArmsLabel.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/icons/schild_en.jpg"))); // NOI18N
        coatOfArmsLabel.setName("coatOfArmsLabel"); // NOI18N
        coatOfArmsLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        picturesPanel.add(coatOfArmsLabel, gridBagConstraints);

        photo.setBackground(new java.awt.Color(255, 255, 255));
        photo.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        photo.setMaximumSize(new java.awt.Dimension(140, 200));
        photo.setMinimumSize(new java.awt.Dimension(140, 200));
        photo.setName("photo"); // NOI18N
        photo.setOpaque(true);
        photo.setPreferredSize(new java.awt.Dimension(140, 200));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        picturesPanel.add(photo, gridBagConstraints);

        cardType.setFont(new java.awt.Font("Dialog", 1, 24));
        cardType.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        cardType.setName("cardType"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        picturesPanel.add(cardType, gridBagConstraints);

        add(picturesPanel, java.awt.BorderLayout.NORTH);

        dataPanel.setBackground(new java.awt.Color(255, 255, 255));
        dataPanel.setName("dataPanel"); // NOI18N
        dataPanel.setLayout(new java.awt.GridBagLayout());

        nameLabel.setFont(new java.awt.Font("Serif", 1, 12));
        nameLabel.setText(bundle.getString("nameLabel")); // NOI18N
        nameLabel.setEnabled(false);
        nameLabel.setName("nameLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(nameLabel, gridBagConstraints);

        givenNamesLabel.setText(bundle.getString("givenNamesLabel")); // NOI18N
        givenNamesLabel.setEnabled(false);
        givenNamesLabel.setName("givenNamesLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(givenNamesLabel, gridBagConstraints);

        placeOfBirthLabel.setText(bundle.getString("placeOfBirthLabel")); // NOI18N
        placeOfBirthLabel.setEnabled(false);
        placeOfBirthLabel.setName("placeOfBirthLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(placeOfBirthLabel, gridBagConstraints);

        dateOfBirthLabel.setText(bundle.getString("dateOfBirthLabel")); // NOI18N
        dateOfBirthLabel.setEnabled(false);
        dateOfBirthLabel.setName("dateOfBirthLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(dateOfBirthLabel, gridBagConstraints);

        nationalityLabel.setText(bundle.getString("nationalityLabel")); // NOI18N
        nationalityLabel.setEnabled(false);
        nationalityLabel.setName("nationalityLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(nationalityLabel, gridBagConstraints);

        nationalNumberLabel.setText(bundle.getString("nationalNumberLabel")); // NOI18N
        nationalNumberLabel.setEnabled(false);
        nationalNumberLabel.setName("nationalNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(nationalNumberLabel, gridBagConstraints);

        sexLabel.setText(bundle.getString("sexLabel")); // NOI18N
        sexLabel.setEnabled(false);
        sexLabel.setName("sexLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(sexLabel, gridBagConstraints);

        titleLabel.setText(bundle.getString("titleLabel")); // NOI18N
        titleLabel.setEnabled(false);
        titleLabel.setName("titleLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(titleLabel, gridBagConstraints);

        specialStatusLabel.setText(bundle.getString("specialStatusLabel")); // NOI18N
        specialStatusLabel.setEnabled(false);
        specialStatusLabel.setName("specialStatusLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(specialStatusLabel, gridBagConstraints);

        streetLabel.setText(bundle.getString("streetLabel")); // NOI18N
        streetLabel.setEnabled(false);
        streetLabel.setName("streetLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 10;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(streetLabel, gridBagConstraints);

        postalCodeLabel.setText(bundle.getString("postalCodeLabel")); // NOI18N
        postalCodeLabel.setEnabled(false);
        postalCodeLabel.setName("postalCodeLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 11;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(postalCodeLabel, gridBagConstraints);

        municipalityLabel.setText(bundle.getString("municipalityLabel")); // NOI18N
        municipalityLabel.setEnabled(false);
        municipalityLabel.setName("municipalityLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(municipalityLabel, gridBagConstraints);

        addressCardDataSeparator.setEnabled(false);
        addressCardDataSeparator.setName("addressCardDataSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 13;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new java.awt.Insets(7, 0, 7, 0);
        dataPanel.add(addressCardDataSeparator, gridBagConstraints);

        name.setEnabled(false);
        name.setName("name"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipady = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(name, gridBagConstraints);

        givenNames.setEnabled(false);
        givenNames.setName("givenNames"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(givenNames, gridBagConstraints);

        placeOfBirth.setEnabled(false);
        placeOfBirth.setName("placeOfBirth"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(placeOfBirth, gridBagConstraints);

        dateOfBirth.setEnabled(false);
        dateOfBirth.setName("dateOfBirth"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(dateOfBirth, gridBagConstraints);

        sex.setEnabled(false);
        sex.setName("sex"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(sex, gridBagConstraints);

        nationalNumber.setEnabled(false);
        nationalNumber.setName("nationalNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(nationalNumber, gridBagConstraints);

        nationality.setEnabled(false);
        nationality.setName("nationality"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(nationality, gridBagConstraints);

        title.setEnabled(false);
        title.setName("title"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(title, gridBagConstraints);

        specialStatus.setEnabled(false);
        specialStatus.setName("specialStatus"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(specialStatus, gridBagConstraints);

        street.setEnabled(false);
        street.setName("street"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 10;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(street, gridBagConstraints);

        postalCode.setEnabled(false);
        postalCode.setName("postalCode"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 11;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(postalCode, gridBagConstraints);

        municipality.setEnabled(false);
        municipality.setName("municipality"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 12;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(municipality, gridBagConstraints);

        spacer.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer.setName("spacer"); // NOI18N
        spacer.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        dataPanel.add(spacer, gridBagConstraints);

        validFrom.setEnabled(false);
        validFrom.setName("validFrom"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 16;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(validFrom, gridBagConstraints);

        spacer1.setEnabled(false);
        spacer1.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer1.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer1.setName("spacer1"); // NOI18N
        spacer1.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        dataPanel.add(spacer1, gridBagConstraints);

        chipNumber.setEnabled(false);
        chipNumber.setName("chipNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 15;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(chipNumber, gridBagConstraints);

        validFromLabel.setText(bundle.getString("validFromLabel")); // NOI18N
        validFromLabel.setEnabled(false);
        validFromLabel.setName("validFromLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 16;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(validFromLabel, gridBagConstraints);

        validUntil.setEnabled(false);
        validUntil.setName("validUntil"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 17;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(validUntil, gridBagConstraints);

        chipNumberLabel.setText(bundle.getString("chipNumberLabel")); // NOI18N
        chipNumberLabel.setEnabled(false);
        chipNumberLabel.setName("chipNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 15;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(chipNumberLabel, gridBagConstraints);

        placeOfIssue.setEnabled(false);
        placeOfIssue.setName("placeOfIssue"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 18;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(placeOfIssue, gridBagConstraints);

        placeOfIssueLabel.setText(bundle.getString("placeOfIssueLabel")); // NOI18N
        placeOfIssueLabel.setEnabled(false);
        placeOfIssueLabel.setName("placeOfIssueLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 18;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(placeOfIssueLabel, gridBagConstraints);

        validUntilLabel.setText(bundle.getString("validUntilLabel")); // NOI18N
        validUntilLabel.setEnabled(false);
        validUntilLabel.setName("validUntilLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 17;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(validUntilLabel, gridBagConstraints);

        cardNumber.setEnabled(false);
        cardNumber.setName("cardNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 14;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        dataPanel.add(cardNumber, gridBagConstraints);

        cardNumberLabel.setText(bundle.getString("cardNumberLabel")); // NOI18N
        cardNumberLabel.setEnabled(false);
        cardNumberLabel.setName("cardNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 14;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        dataPanel.add(cardNumberLabel, gridBagConstraints);

        idAddressSeparator.setEnabled(false);
        idAddressSeparator.setName("idAddressSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new java.awt.Insets(7, 0, 7, 0);
        dataPanel.add(idAddressSeparator, gridBagConstraints);

        add(dataPanel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JSeparator addressCardDataSeparator;
    private javax.swing.JLabel cardNumber;
    private javax.swing.JLabel cardNumberLabel;
    private javax.swing.JLabel cardType;
    private javax.swing.JLabel chipNumber;
    private javax.swing.JLabel chipNumberLabel;
    private javax.swing.JLabel coatOfArmsLabel;
    private javax.swing.JPanel dataPanel;
    private javax.swing.JLabel dateOfBirth;
    private javax.swing.JLabel dateOfBirthLabel;
    private javax.swing.JLabel givenNames;
    private javax.swing.JLabel givenNamesLabel;
    private javax.swing.JSeparator idAddressSeparator;
    private javax.swing.JLabel municipality;
    private javax.swing.JLabel municipalityLabel;
    private javax.swing.JLabel name;
    private javax.swing.JLabel nameLabel;
    private javax.swing.JLabel nationalNumber;
    private javax.swing.JLabel nationalNumberLabel;
    private javax.swing.JLabel nationality;
    private javax.swing.JLabel nationalityLabel;
    private javax.swing.JLabel photo;
    private javax.swing.JPanel picturesPanel;
    private javax.swing.JLabel placeOfBirth;
    private javax.swing.JLabel placeOfBirthLabel;
    private javax.swing.JLabel placeOfIssue;
    private javax.swing.JLabel placeOfIssueLabel;
    private javax.swing.JLabel postalCode;
    private javax.swing.JLabel postalCodeLabel;
    private javax.swing.JLabel sex;
    private javax.swing.JLabel sexLabel;
    private javax.swing.JLabel spacer;
    private javax.swing.JLabel spacer1;
    private javax.swing.JLabel specialStatus;
    private javax.swing.JLabel specialStatusLabel;
    private javax.swing.JLabel street;
    private javax.swing.JLabel streetLabel;
    private javax.swing.JLabel title;
    private javax.swing.JLabel titleLabel;
    private javax.swing.JLabel validFrom;
    private javax.swing.JLabel validFromLabel;
    private javax.swing.JLabel validUntil;
    private javax.swing.JLabel validUntilLabel;
    // End of variables declaration//GEN-END:variables

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
}
