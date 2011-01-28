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
import be.fedict.eidviewer.gui.helper.ImageUtilities;
import be.fedict.eidviewer.gui.helper.X509Utilities;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.security.Principal;
import java.security.cert.CertificateExpiredException;
import java.security.cert.CertificateNotYetValidException;
import java.security.cert.X509Certificate;
import java.text.DateFormat;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Observable;
import java.util.Observer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JTree;
import javax.swing.UIManager;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

/**
 *
 * @author Frank Marien
 */
public class CertificatesPanel extends javax.swing.JPanel implements Observer, TreeSelectionListener
{
    private static final Logger                     logger=Logger.getLogger(CertificatesPanel.class.getName());
    private static final String                     ICONS = "resources/icons/";
    private ResourceBundle                          bundle;
    private DateFormat                              dateFormat;
    private Map<Principal, DefaultMutableTreeNode>  certificatesInTree;
    private DefaultMutableTreeNode                  rootNode;
    private DefaultTreeModel                        treeModel;
    private Color                                   defaultLabelForeground,defaultLabelBackground;
    private EidController                           eidController;

    public CertificatesPanel()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/CertificatesPanel");
        dateFormat = DateFormat.getDateInstance(DateFormat.LONG, Locale.getDefault());
        initComponents();
        trustErrors.setVisible(false);
        defaultLabelForeground = UIManager.getColor("Label.foreground");
        defaultLabelBackground = UIManager.getColor("Label.background");
        initCertsTree();  
    }

    public CertificatesPanel setEidController(EidController eidController)
    {
        this.eidController = eidController;
        return this;
    }

    public CertificatesPanel start()
    {
        initTrustPrefsPanel();
        certsBusyIcon.setVisible(false);
        certsTree.addTreeSelectionListener(this);
        clearCertsTree();
        return this;
    }

    public void update(Observable o, Object o1)
    {
        if (eidController == null)
            return;

        logger.finest("Updating..");
        updateVisibleState();

        if(eidController.getState() == EidController.STATE.EID_PRESENT || eidController.getState()==EidController.STATE.EID_YIELDED || eidController.getState()==EidController.STATE.FILE_LOADED)
        {
            logger.finest("Filling Out Certificate Data..");
            if(eidController.hasAuthCertChain())
                addCerts(eidController.getAuthCertChain());

            if(eidController.hasSignCertChain())
                addCerts(eidController.getSignCertChain());
        }
        else
        {
            logger.finest("Clearing Certificate Data..");
            clearCertsTree();
            rootNode = null;
        }   
    }

    private void addCerts(X509CertificateChainAndTrust chain)
    {
        if (chain != null)
        {
            List<X509CertificateAndTrust> certificates = chain.getCertificatesAndTrusts();
            for (ListIterator<X509CertificateAndTrust> i = certificates.listIterator(certificates.size()); i.hasPrevious();)
            {
                X509CertificateAndTrust certificate = i.previous();
                DefaultMutableTreeNode existingNode = certificatesInTree.get(certificate.getSubjectDN());

                if(existingNode == null)  // new information to add
                {
                    DefaultMutableTreeNode newkid = new DefaultMutableTreeNode(certificate);
                    DefaultMutableTreeNode parent = certificatesInTree.get(certificate.getIssuerDN());
                    addTreeNode(parent, newkid);
                    certificatesInTree.put(certificate.getSubjectDN(), newkid);
                    updateCertificateDetail();
                }
                else    // existing information to update
                {
                    updateTreeNode(existingNode);
                    updateCertificateDetail();
                    logger.log(Level.FINEST, "UPDATE [{0}]", (((X509CertificateAndTrust) (existingNode.getUserObject())).getSubjectDN().toString()));
                }
            }
        }
    }

    private void updateVisibleState()
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                validateNowButton.setEnabled(eidController.isReadyForCommand());

                if(eidController.getState()==EidController.STATE.EID_PRESENT)
                    certsBusyIcon.setVisible(eidController.getActivity() == EidController.ACTIVITY.READING_AUTH_CHAIN ||
                                             eidController.getActivity() == EidController.ACTIVITY.READING_SIGN_CHAIN ||
                                             eidController.isValidatingTrust());
                else
                    certsBusyIcon.setVisible(false);
            }
        });
    }

    private void updateTreeNode(final DefaultMutableTreeNode changedNode)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                treeModel.nodeChanged(changedNode);
            }
        });
    }

    private void addTreeNode(final DefaultMutableTreeNode parent, final DefaultMutableTreeNode child)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                if (parent == null) // Belgian Root CA. When this arrives, activate the tree
                {
                    rootNode = child;
                    treeModel = new DefaultTreeModel(rootNode);
                    certsTree.setModel(treeModel);
                    certsTree.setVisible(true);
                }
                else  // all other certs are attached to their parent node
                {
                    treeModel.insertNodeInto(child, parent, parent.getChildCount());
                    for (int row = 0; row < certsTree.getRowCount(); row++)  // and auto-expand to display it
                        certsTree.expandRow(row);
                    X509CertificateAndTrust certAndTrust=(X509CertificateAndTrust)child.getUserObject();
                    if(certAndTrust!=null && X509Utilities.keyHasDigitalSignatureConstraint(certAndTrust.getCertificate()))
                        certsTree.setSelectionPath(new TreePath(child.getPath()));
                }
            }
        });
    }

    // user (de)selected a certificate
    public void valueChanged(TreeSelectionEvent treeSelectionEvent)
    {
       updateCertificateDetail();
    }

    private void updateCertificateDetail()
    {
        DefaultMutableTreeNode treeNode = (DefaultMutableTreeNode) certsTree.getLastSelectedPathComponent();

        if (treeNode != null)
        {
            X509CertificateAndTrust certAndTrust = (X509CertificateAndTrust) treeNode.getUserObject();
            certificateSelected(certAndTrust);
        }
        else
        {
            certificateSelected(null);
        }
    }

    private void certificateSelected(final X509CertificateAndTrust certAndTrust)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                if (certAndTrust != null)
                {
                    X509Certificate certificate = certAndTrust.getCertificate();


                    if(certAndTrust.isValidated())
                    {
                        if(certAndTrust.isTrusted())
                        {
                            trustStatus.setText(bundle.getString("trustStatus_trusted"));
                            trustErrors.setVisible(false);
                            trustStatus.setForeground(greener(defaultLabelForeground));
                            trustStatusLabel.setForeground(greener(defaultLabelForeground));
                            trustStatus.setBackground(greener(defaultLabelBackground));
                            trustStatusLabel.setBackground(greener(defaultLabelBackground));
                        }
                        else
                        {
                            trustStatus.setText(bundle.getString("trustStatus_untrusted"));
                            trustErrors.setText(getMultilineLabelBundleText(bundle,"trustError_", certAndTrust.getInvalidReasons()));
                            trustErrors.setVisible(true);
                            trustStatus.setForeground(redder(defaultLabelForeground));
                            trustStatusLabel.setForeground(redder(defaultLabelForeground));
                            trustStatus.setBackground(redder(defaultLabelBackground));
                            trustStatusLabel.setBackground(redder(defaultLabelBackground));
                        }
                    }
                    else
                    {
                        trustStatus.setForeground(defaultLabelForeground);
                        trustStatusLabel.setForeground(defaultLabelForeground);
                        trustStatus.setBackground(defaultLabelBackground);
                        trustStatusLabel.setBackground(defaultLabelBackground);
                            
                        if(certAndTrust.getValidationException()!=null)
                        {
                            trustStatus.setText(getMultilinelabelText(bundle.getString("trustStatus_unobtainable")));
                            trustErrors.setText(certAndTrust.getValidationException().getLocalizedMessage());
                            trustErrors.setVisible(true);  
                        }
                        else
                        {
                            trustStatus.setText(bundle.getString(certAndTrust.isValidating()?"trustStatus_validating":"trustStatus_unknown"));
                            trustStatus.setForeground(defaultLabelForeground);
                            trustErrors.setVisible(false);
                        }
                    }

                    dn.setText(getMultilineDN(certificate.getSubjectDN().getName()));
                    validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()));
                    validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    keyUsage.setText(getMultilineLabelText(X509Utilities.getKeyUsageStrings(bundle, certificate.getKeyUsage())));

                    try
                    {
                        certificate.checkValidity();
                        validFromLabel.setForeground(defaultLabelForeground);
                        validFrom.setForeground(defaultLabelForeground);
                        validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()));
                        validUntilLabel.setForeground(defaultLabelForeground);
                        validUntil.setForeground(defaultLabelForeground);
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    }
                    catch (CertificateExpiredException ex)
                    {
                        validFromLabel.setForeground(defaultLabelForeground);
                        validFrom.setForeground(defaultLabelForeground);
                        validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()));
                        validUntilLabel.setForeground(redder(defaultLabelForeground));
                        validUntil.setForeground(redder(defaultLabelForeground));
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()) + " " + bundle.getString("notAfterWarning"));
                    }
                    catch (CertificateNotYetValidException ex)
                    {
                        validFromLabel.setForeground(redder(defaultLabelForeground));
                        validFrom.setForeground(redder(defaultLabelForeground));
                        validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()) + " " + bundle.getString("notBeforeWarning"));
                        validUntilLabel.setForeground(defaultLabelForeground);
                        validUntil.setForeground(defaultLabelForeground);
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    }

                    dn.setEnabled(true);
                    keyUsage.setEnabled(true);
                    validFromLabel.setEnabled(true);
                    validUntilLabel.setEnabled(true);
                    validFrom.setEnabled(true);
                    validUntil.setEnabled(true);
                    certificateIcon.setEnabled(true);
                    trustStatus.setEnabled(true);
                    trustStatusLabel.setEnabled(true);
                    keyUsageLabel.setEnabled(true);
                }
                else
                {
                    dn.setText("");
                    keyUsage.setText("");
                    validFrom.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    validUntil.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    trustStatus.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);

                    trustStatus.setForeground(defaultLabelForeground);
                    trustStatusLabel.setForeground(defaultLabelForeground);
                    trustStatus.setBackground(defaultLabelBackground);
                    trustStatusLabel.setBackground(defaultLabelBackground);

                    dn.setEnabled(false);
                    keyUsage.setEnabled(false);
                    validFromLabel.setEnabled(false);
                    validUntilLabel.setEnabled(false);
                    validFrom.setEnabled(false);
                    validUntil.setEnabled(false);
                    certificateIcon.setEnabled(false);
                    trustErrors.setVisible(false);
                    trustStatus.setEnabled(false);
                    trustStatusLabel.setEnabled(false);
                    keyUsageLabel.setEnabled(false);
                }
            }
        });
    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        certsDetailsSplitPane = new javax.swing.JSplitPane();
        authCertsPanel = new javax.swing.JPanel();
        certsTree = new javax.swing.JTree();
        certsBusyIcon = new javax.swing.JLabel();
        certDetailsPanel = new javax.swing.JPanel();
        certificateIcon = new javax.swing.JLabel();
        dn = new javax.swing.JLabel();
        validFromLabel = new javax.swing.JLabel();
        validUntilLabel = new javax.swing.JLabel();
        validFrom = new javax.swing.JLabel();
        validUntil = new javax.swing.JLabel();
        keyUsage = new javax.swing.JLabel();
        trustStatusLabel = new javax.swing.JLabel();
        trustStatus = new javax.swing.JLabel();
        trustErrors = new javax.swing.JLabel();
        keyUsageLabel = new javax.swing.JLabel();
        trustServiceTrustErrorsSeparator = new javax.swing.JSeparator();
        spacer = new javax.swing.JLabel();
        validdUntilKeyUsageSeparator = new javax.swing.JSeparator();
        keyUsageTrustSeparator = new javax.swing.JSeparator();
        trustPrefspanel = new javax.swing.JPanel();
        alwaysValidateCheckbox = new javax.swing.JCheckBox();
        validateNowButton = new javax.swing.JButton();

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(204, 255, 204), 24, true));
        setForeground(new java.awt.Color(0, 0, 0));
        setLayout(new java.awt.BorderLayout());

        certsDetailsSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        certsDetailsSplitPane.setName("certsDetailsSplitPane"); // NOI18N

        authCertsPanel.setBorder(javax.swing.BorderFactory.createEmptyBorder(4, 4, 4, 4));
        authCertsPanel.setMinimumSize(new java.awt.Dimension(24, 124));
        authCertsPanel.setName("authCertsPanel"); // NOI18N
        authCertsPanel.setOpaque(false);
        authCertsPanel.setPreferredSize(new java.awt.Dimension(600, 124));
        authCertsPanel.setLayout(new java.awt.BorderLayout());

        certsTree.setBorder(javax.swing.BorderFactory.createEmptyBorder(8, 8, 8, 8));
        certsTree.setName("certsTree"); // NOI18N
        authCertsPanel.add(certsTree, java.awt.BorderLayout.CENTER);

        certsBusyIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/busyicons/busy_anim_small.gif"))); // NOI18N
        certsBusyIcon.setName("certsBusyIcon"); // NOI18N
        authCertsPanel.add(certsBusyIcon, java.awt.BorderLayout.WEST);

        certsDetailsSplitPane.setLeftComponent(authCertsPanel);

        certDetailsPanel.setName("certDetailsPanel"); // NOI18N
        certDetailsPanel.setLayout(new java.awt.GridBagLayout());

        certificateIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/icons/certificate_large.png"))); // NOI18N
        certificateIcon.setName("certificateIcon"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridheight = 5;
        gridBagConstraints.ipadx = 8;
        gridBagConstraints.ipady = 8;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 10);
        certDetailsPanel.add(certificateIcon, gridBagConstraints);

        dn.setBackground(new java.awt.Color(204, 204, 204));
        dn.setText("(cn)");
        dn.setName("dn"); // NOI18N
        dn.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.ipady = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        certDetailsPanel.add(dn, gridBagConstraints);

        validFromLabel.setText(bundle.getString("validFromLabel")); // NOI18N
        validFromLabel.setName("validFromLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 4, 0, 0);
        certDetailsPanel.add(validFromLabel, gridBagConstraints);

        validUntilLabel.setText(bundle.getString("validUntilLabel")); // NOI18N
        validUntilLabel.setName("validUntilLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 4, 0, 0);
        certDetailsPanel.add(validUntilLabel, gridBagConstraints);

        validFrom.setText("-");
        validFrom.setName("validFrom"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 4);
        certDetailsPanel.add(validFrom, gridBagConstraints);

        validUntil.setText("-");
        validUntil.setName("validUntil"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 4);
        certDetailsPanel.add(validUntil, gridBagConstraints);

        keyUsage.setText(bundle.getString("keyUsageLabel")); // NOI18N
        keyUsage.setName("keyUsage"); // NOI18N
        keyUsage.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 4);
        certDetailsPanel.add(keyUsage, gridBagConstraints);

        trustStatusLabel.setText(bundle.getString("trustStatusLabel")); // NOI18N
        trustStatusLabel.setName("trustStatusLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 4, 0, 0);
        certDetailsPanel.add(trustStatusLabel, gridBagConstraints);

        trustStatus.setText("-"); // NOI18N
        trustStatus.setName("trustStatus"); // NOI18N
        trustStatus.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 2;
        gridBagConstraints.ipady = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 4);
        certDetailsPanel.add(trustStatus, gridBagConstraints);

        trustErrors.setBackground(new java.awt.Color(255, 153, 102));
        trustErrors.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        trustErrors.setText("(trusterrors)");
        trustErrors.setName("trustErrors"); // NOI18N
        trustErrors.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 10;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.ipady = 12;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        certDetailsPanel.add(trustErrors, gridBagConstraints);

        keyUsageLabel.setText(bundle.getString("keyUsageLabel")); // NOI18N
        keyUsageLabel.setName("keyUsageLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 4, 0, 0);
        certDetailsPanel.add(keyUsageLabel, gridBagConstraints);

        trustServiceTrustErrorsSeparator.setName("trustServiceTrustErrorsSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.insets = new java.awt.Insets(8, 0, 4, 0);
        certDetailsPanel.add(trustServiceTrustErrorsSeparator, gridBagConstraints);

        spacer.setEnabled(false);
        spacer.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer.setName("spacer"); // NOI18N
        spacer.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        certDetailsPanel.add(spacer, gridBagConstraints);

        validdUntilKeyUsageSeparator.setName("validdUntilKeyUsageSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.insets = new java.awt.Insets(8, 0, 4, 0);
        certDetailsPanel.add(validdUntilKeyUsageSeparator, gridBagConstraints);

        keyUsageTrustSeparator.setName("keyUsageTrustSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.insets = new java.awt.Insets(8, 0, 4, 0);
        certDetailsPanel.add(keyUsageTrustSeparator, gridBagConstraints);

        certsDetailsSplitPane.setRightComponent(certDetailsPanel);

        add(certsDetailsSplitPane, java.awt.BorderLayout.CENTER);

        trustPrefspanel.setName("trustPrefspanel"); // NOI18N
        trustPrefspanel.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.CENTER, 32, 5));

        alwaysValidateCheckbox.setText(bundle.getString("alwaysValidateCheckbox")); // NOI18N
        alwaysValidateCheckbox.setName("alwaysValidateCheckbox"); // NOI18N
        trustPrefspanel.add(alwaysValidateCheckbox);

        validateNowButton.setText(bundle.getString("validateNowButton")); // NOI18N
        validateNowButton.setName("validateNowButton"); // NOI18N
        trustPrefspanel.add(validateNowButton);

        add(trustPrefspanel, java.awt.BorderLayout.PAGE_END);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBox alwaysValidateCheckbox;
    private javax.swing.JPanel authCertsPanel;
    private javax.swing.JPanel certDetailsPanel;
    private javax.swing.JLabel certificateIcon;
    private javax.swing.JLabel certsBusyIcon;
    private javax.swing.JSplitPane certsDetailsSplitPane;
    private javax.swing.JTree certsTree;
    private javax.swing.JLabel dn;
    private javax.swing.JLabel keyUsage;
    private javax.swing.JLabel keyUsageLabel;
    private javax.swing.JSeparator keyUsageTrustSeparator;
    private javax.swing.JLabel spacer;
    private javax.swing.JLabel trustErrors;
    private javax.swing.JPanel trustPrefspanel;
    private javax.swing.JSeparator trustServiceTrustErrorsSeparator;
    private javax.swing.JLabel trustStatus;
    private javax.swing.JLabel trustStatusLabel;
    private javax.swing.JLabel validFrom;
    private javax.swing.JLabel validFromLabel;
    private javax.swing.JLabel validUntil;
    private javax.swing.JLabel validUntilLabel;
    private javax.swing.JButton validateNowButton;
    private javax.swing.JSeparator validdUntilKeyUsageSeparator;
    // End of variables declaration//GEN-END:variables

    private void initCertsTree()
    {
        certificatesInTree = new HashMap<Principal, DefaultMutableTreeNode>(5);
        certsTree.setCellRenderer(new CertAndTrustCellRenderer());
        certsTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        certsTree.setRootVisible(true);
    }

    private void initTrustPrefsPanel()
    {
        alwaysValidateCheckbox.setSelected(eidController.isAutoValidatingTrust());

        alwaysValidateCheckbox.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent ae)
            {
                eidController.setAutoValidateTrust(alwaysValidateCheckbox.isSelected());
                ViewerPrefs.setAutoValidating(alwaysValidateCheckbox.isSelected());
                if(alwaysValidateCheckbox.isSelected() && eidController.isReadyForCommand())
                    eidController.validateTrust();

            }
        });

        validateNowButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent ae)
            {
                if(eidController.isReadyForCommand())
                    eidController.validateTrust();
            }
        });
    }

    private class CertAndTrustCellRenderer extends DefaultTreeCellRenderer
    {
        private Icon    certIcon, certTrustedIcon, certInvalidIcon;
        private Color   redSelectedForeground, redForeground;
        private Color   greenSelectedForeground, greenForeground;
        private Color   defaultSelectedForeground, defaultForeground;
        private Font    defaultFont,boldFont;

        public CertAndTrustCellRenderer()
        {
            certIcon                    = ImageUtilities.getIcon(CertificatesPanel.class, ICONS+"certificate_small.png");
            certTrustedIcon             = ImageUtilities.getIcon(CertificatesPanel.class, ICONS+"certificate_trusted_small.png");
            certInvalidIcon             = ImageUtilities.getIcon(CertificatesPanel.class, ICONS+"certificate_invalid_small.png");
            defaultSelectedForeground   =UIManager.getColor("Tree.selectionForeground");
            defaultForeground           =UIManager.getColor("Tree.textForeground");
            redForeground               =redder(defaultForeground);
            redSelectedForeground       =redder(defaultSelectedForeground);
            greenForeground             =greener(defaultForeground);
            greenSelectedForeground     =greener(defaultSelectedForeground);
        }

        @Override
        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus)
        {
            super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);
            DefaultMutableTreeNode treeNode = (DefaultMutableTreeNode) value;

            if (!(treeNode.getUserObject() instanceof X509CertificateAndTrust))
                return this;

            if(defaultFont==null)
                defaultFont=getFont();

            if(boldFont==null)
                boldFont=getFont().deriveFont(Font.BOLD);

            X509CertificateAndTrust certAndTrust = (X509CertificateAndTrust) treeNode.getUserObject();

            if(certAndTrust.isValidated())
            {
                if(certAndTrust.isTrusted())
                {
                    setTextNonSelectionColor(greenForeground);
                    setTextSelectionColor(greenSelectedForeground);
                    setIcon(certTrustedIcon);
                    setFont(boldFont);
                }
                else
                {
                    setTextNonSelectionColor(redForeground);
                    setTextSelectionColor(redSelectedForeground);
                    setIcon(certInvalidIcon);
                    setFont(boldFont);
                }
            }
            else
            {
                setTextNonSelectionColor(defaultForeground);
                setTextSelectionColor(defaultSelectedForeground);
                setIcon(certIcon);
                setFont(defaultFont);
            }
                
            return this;
        }
    }

    private Color greener(Color originalColor)
    {
        Color less = originalColor.darker().darker();
        Color more = originalColor.brighter().brighter();
        return new Color(less.getRed(), more.getGreen(), less.getBlue());
    }

    private Color redder(Color originalColor)
    {
        Color less = originalColor.darker().darker();
        Color more = originalColor.brighter().brighter();
        return new Color(more.getRed(), less.getGreen(), less.getBlue());
    }

    private void clearCertsTree()
    {
        certsTree.setVisible(false);
        certificateSelected(null);
        certificatesInTree.clear();
        treeModel = new DefaultTreeModel(new DefaultMutableTreeNode("-"));
        certsTree.setModel(treeModel);
    }

    private String getMultilineDN(String dnStr)
    {
        StringBuilder html = new StringBuilder("<html>");
        String[] dnParts = dnStr.split("\\s*,\\s*");
        for (String dnPart : dnParts)
        {
            html.append(dnPart);
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }

     private String getMultilinelabelText(String string)
    {
        StringBuilder html = new StringBuilder("<html>");
        String[] parts = string.split(";");
        for (String part : parts)
        {
            html.append(part);
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }

    private String getMultilineLabelText(List<String> strings)
    {
        StringBuilder html = new StringBuilder("<html>");
        for (String string : strings)
        {
            html.append(string);
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }

    private String getMultilineLabelBundleText(ResourceBundle bundle, String prefix, List<String> strings)
    {
        StringBuilder html = new StringBuilder("<html>");
        for (String key : strings)
        {
            html.append(bundle.getString(prefix + key));
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }
}
