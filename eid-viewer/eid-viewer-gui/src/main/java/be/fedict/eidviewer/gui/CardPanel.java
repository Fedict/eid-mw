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

import be.fedict.eid.applet.service.Identity;
import be.fedict.eidviewer.gui.helper.IdFormatHelper;
import java.text.DateFormat;
import java.util.Locale;
import java.util.Observable;
import java.util.Observer;
import java.util.ResourceBundle;
import java.util.logging.Logger;

/**
 *
 * @author Frank Marien
 */
public class CardPanel extends javax.swing.JPanel implements Observer
{
    private static final Logger logger = Logger.getLogger(CardPanel.class.getName());
    private ResourceBundle      bundle;
    private DateFormat          dateFormat;
    private EidController       eidController;

    public CardPanel()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/CardPanel");
        dateFormat = DateFormat.getDateInstance(DateFormat.DEFAULT, Locale.getDefault());
        initComponents();
        fillCardInfo(null,false);
    }

    public CardPanel setEidController(EidController eidController)
    {
        logger.finest("Setting eidController");
        this.eidController = eidController;
        return this;
    }

    public void update(Observable o, Object o1)
    {
        if (eidController == null)
            return;

        logger.finest("Updating..");
        updateVisibleState();

        if (eidController.getState() == EidController.STATE.EID_PRESENT  || eidController.getState()==EidController.STATE.EID_YIELDED || eidController.getState()==EidController.STATE.FILE_LOADED)
        {
            if(eidController.hasIdentity())
            {
                logger.finest("Filling Out Card Data..");
                fillCardInfo(eidController.getIdentity(), false);
            }
            else
            {
                logger.finest("Clear Card Data But Loading..");
                fillCardInfo(null, true);
            }
        }
        else
        {
            logger.finest("Clear Card Data And Not Loading");
            fillCardInfo(null, false);
        }    
    }

    private void updateVisibleState()
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                boolean enablePinActions=eidController.isReadyForCommand() && eidController.getState()==EidController.STATE.EID_PRESENT;
               verifyPinButton.setEnabled(enablePinActions);
               changePinButton.setEnabled(enablePinActions);
            }
        });
    }

    private void fillCardInfo(final Identity identity, final boolean loading)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                cardInfoBusyIcon.setVisible(loading);
                if (identity != null)
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
                else
                {
                    cardNumber.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    cardNumber.setEnabled(false);
                    cardNumberLabel.setEnabled(false);

                    placeOfIssue.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    placeOfIssue.setEnabled(false);
                    placeOfIssueLabel.setEnabled(false);

                    chipNumber.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    chipNumber.setEnabled(false);
                    chipNumberLabel.setEnabled(false);

                    validFrom.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    validFrom.setEnabled(false);
                    validFromLabel.setEnabled(false);

                    validUntil.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    validUntil.setEnabled(false);
                    validUntilLabel.setEnabled(false);
                }
            }
        });
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

        cardNumberLabel = new javax.swing.JLabel();
        placeOfIssueLabel = new javax.swing.JLabel();
        chipNumberLabel = new javax.swing.JLabel();
        validFromLabel = new javax.swing.JLabel();
        cardNumber = new javax.swing.JLabel();
        placeOfIssue = new javax.swing.JLabel();
        chipNumber = new javax.swing.JLabel();
        validUntil = new javax.swing.JLabel();
        cardInfoBusyIcon = new javax.swing.JLabel();
        spacer = new javax.swing.JLabel();
        validUntilLabel = new javax.swing.JLabel();
        validFrom = new javax.swing.JLabel();
        spacer1 = new javax.swing.JLabel();
        changePinButton = new javax.swing.JButton();
        verifyPinButton = new javax.swing.JButton();

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(204, 255, 204), 24, true));
        setLayout(new java.awt.GridBagLayout());

        cardNumberLabel.setText(bundle.getString("cardNumberLabel")); // NOI18N
        cardNumberLabel.setEnabled(false);
        cardNumberLabel.setName("cardNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(cardNumberLabel, gridBagConstraints);

        placeOfIssueLabel.setText(bundle.getString("placeOfIssueLabel")); // NOI18N
        placeOfIssueLabel.setEnabled(false);
        placeOfIssueLabel.setName("placeOfIssueLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(placeOfIssueLabel, gridBagConstraints);

        chipNumberLabel.setText(bundle.getString("chipNumberLabel")); // NOI18N
        chipNumberLabel.setEnabled(false);
        chipNumberLabel.setName("chipNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(chipNumberLabel, gridBagConstraints);

        validFromLabel.setText(bundle.getString("validFromLabel")); // NOI18N
        validFromLabel.setEnabled(false);
        validFromLabel.setName("validFromLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(validFromLabel, gridBagConstraints);

        cardNumber.setEnabled(false);
        cardNumber.setName("cardNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(cardNumber, gridBagConstraints);

        placeOfIssue.setEnabled(false);
        placeOfIssue.setName("placeOfIssue"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(placeOfIssue, gridBagConstraints);

        chipNumber.setEnabled(false);
        chipNumber.setName("chipNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(chipNumber, gridBagConstraints);

        validUntil.setEnabled(false);
        validUntil.setName("validUntil"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(validUntil, gridBagConstraints);

        cardInfoBusyIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/busyicons/busy_anim_small.gif"))); // NOI18N
        cardInfoBusyIcon.setName("cardInfoBusyIcon"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(cardInfoBusyIcon, gridBagConstraints);

        spacer.setEnabled(false);
        spacer.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer.setName("spacer"); // NOI18N
        spacer.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        add(spacer, gridBagConstraints);

        validUntilLabel.setText(bundle.getString("validUntilLabel")); // NOI18N
        validUntilLabel.setEnabled(false);
        validUntilLabel.setName("validUntilLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(validUntilLabel, gridBagConstraints);

        validFrom.setEnabled(false);
        validFrom.setName("validFrom"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(validFrom, gridBagConstraints);

        spacer1.setEnabled(false);
        spacer1.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer1.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer1.setName("spacer1"); // NOI18N
        spacer1.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        add(spacer1, gridBagConstraints);

        changePinButton.setText(bundle.getString("changePinButton")); // NOI18N
        changePinButton.setEnabled(false);
        changePinButton.setName("changePinButton"); // NOI18N
        changePinButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                changePinButtonActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(changePinButton, gridBagConstraints);

        verifyPinButton.setText(bundle.getString("verifyPinButton")); // NOI18N
        verifyPinButton.setEnabled(false);
        verifyPinButton.setName("verifyPinButton"); // NOI18N
        verifyPinButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                verifyPinButtonActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(verifyPinButton, gridBagConstraints);
    }// </editor-fold>//GEN-END:initComponents

    private void changePinButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_changePinButtonActionPerformed
    {//GEN-HEADEREND:event_changePinButtonActionPerformed
        eidController.changePin();
    }//GEN-LAST:event_changePinButtonActionPerformed

    private void verifyPinButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_verifyPinButtonActionPerformed
    {//GEN-HEADEREND:event_verifyPinButtonActionPerformed
        eidController.verifyPin();
    }//GEN-LAST:event_verifyPinButtonActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel cardInfoBusyIcon;
    private javax.swing.JLabel cardNumber;
    private javax.swing.JLabel cardNumberLabel;
    private javax.swing.JButton changePinButton;
    private javax.swing.JLabel chipNumber;
    private javax.swing.JLabel chipNumberLabel;
    private javax.swing.JLabel placeOfIssue;
    private javax.swing.JLabel placeOfIssueLabel;
    private javax.swing.JLabel spacer;
    private javax.swing.JLabel spacer1;
    private javax.swing.JLabel validFrom;
    private javax.swing.JLabel validFromLabel;
    private javax.swing.JLabel validUntil;
    private javax.swing.JLabel validUntilLabel;
    private javax.swing.JButton verifyPinButton;
    // End of variables declaration//GEN-END:variables
}
