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
import java.security.Principal;
import java.security.cert.CertificateExpiredException;
import java.security.cert.CertificateNotYetValidException;
import java.security.cert.X509Certificate;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Observable;
import java.util.Observer;
import javax.swing.ImageIcon;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeSelectionModel;

/**
 *
 * @author frank
 */
public class CertificatesPanel extends javax.swing.JPanel  implements Observer,TreeSelectionListener
{
    private static final String                     ICONS = "resources/icons/";
    
    private ResourceBundle                          bundle;
    private DateFormat                              dateFormat;
    private Map<Principal,X509Certificate>          certificatesByPrincipal;
    private Map<Principal,List<X509Certificate>>    certificatesByIssuer;
    private DefaultMutableTreeNode                  rootNode;
    private DefaultTreeModel                        treeModel;
    private DefaultTreeCellRenderer                 treeCellRenderer;
    private Color                                   defaultLabelForeground;

    public CertificatesPanel()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/CertificatesPanel");
        dateFormat = DateFormat.getDateInstance(DateFormat.LONG, Locale.getDefault());
        
        initComponents();
        defaultLabelForeground=validFromLabel.getForeground();

        initCellRenderer();
        initCertificates();
        
        certsTree.setVisible(false);
        certsBusyIcon.setVisible(false);
        certsTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        certsTree.addTreeSelectionListener(this);
        certificateSelected(null);
    }

    private void initCertificates()
    {
        certificatesByPrincipal = new HashMap<Principal, X509Certificate>(5);
        certificatesByIssuer    = new HashMap<Principal, List<X509Certificate>>(5);
    }

    public void update(Observable o, Object o1)
    {
        EidController controller = (EidController) o;

        if(controller.getState()==EidController.STATE.EID_PRESENT)
        {
            if(controller.hasAuthCertChain())
                addCerts(controller.getAuthCertChain(),false);
            else
                addCerts(null,true);

            if(controller.hasSignCertChain())
                addCerts(controller.getSignCertChain(),false);
            else
                addCerts(null,true);
        }
        else
        {
            certificateSelected(null);
            initCertificates();
            certsTree.setVisible(false);
        }
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
        jSeparator1 = new javax.swing.JSeparator();
        spacer1 = new javax.swing.JLabel();
        keyUsage = new javax.swing.JLabel();

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

        jSeparator1.setName("jSeparator1"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 3;
        certDetailsPanel.add(jSeparator1, gridBagConstraints);

        spacer1.setEnabled(false);
        spacer1.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer1.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer1.setName("spacer1"); // NOI18N
        spacer1.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        certDetailsPanel.add(spacer1, gridBagConstraints);

        keyUsage.setBackground(new java.awt.Color(204, 204, 204));
        keyUsage.setText("(keyusage)");
        keyUsage.setName("keyUsage"); // NOI18N
        keyUsage.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 12;
        gridBagConstraints.ipady = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        certDetailsPanel.add(keyUsage, gridBagConstraints);

        certsDetailsSplitPane.setRightComponent(certDetailsPanel);

        add(certsDetailsSplitPane, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel authCertsPanel;
    private javax.swing.JPanel certDetailsPanel;
    private javax.swing.JLabel certificateIcon;
    private javax.swing.JLabel certsBusyIcon;
    private javax.swing.JSplitPane certsDetailsSplitPane;
    private javax.swing.JTree certsTree;
    private javax.swing.JLabel dn;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JLabel keyUsage;
    private javax.swing.JLabel spacer1;
    private javax.swing.JLabel validFrom;
    private javax.swing.JLabel validFromLabel;
    private javax.swing.JLabel validUntil;
    private javax.swing.JLabel validUntilLabel;
    // End of variables declaration//GEN-END:variables

    private void addCerts(List<X509Certificate> certs, boolean loading)
    {
        certsBusyIcon.setVisible(loading);
        if(certs!=null)
        {
            for(X509Certificate cert: certs)
            {
                if(!certificatesByPrincipal.containsKey(cert.getSubjectDN()))      // we'll get the root CA twice, don't replace it.
                {
                    certificatesByPrincipal.put(cert.getSubjectDN(),cert);

                    if(!X509Utilities.isSelfSigned(cert))
                    {
                        List certsByThisIssuer=certificatesByIssuer.get(cert.getIssuerDN());
                        if(certsByThisIssuer==null)
                        {
                            certsByThisIssuer=new ArrayList();
                            certificatesByIssuer.put(cert.getIssuerDN(), certsByThisIssuer);
                        }
                        certsByThisIssuer.add(cert);
                    }
                }
            }
            certificatesToTree();
        }
    }

    private void addCertHierarchy(X509Certificate certificate, DefaultMutableTreeNode node)
    {
        List<X509Certificate> children=certificatesByIssuer.get(certificate.getSubjectDN());

        if(children==null)
            return;

        for(X509Certificate child : children)
        {
            DefaultMutableTreeNode childNode=new DefaultMutableTreeNode(new CertificateDisplayNode(child));
            node.add(childNode);
            addCertHierarchy(child,childNode);
        }
    }

    private void certificatesToTree()
    {
       X509Certificate rootCert=null;
       rootNode=null;
       
       for(X509Certificate certificate : certificatesByPrincipal.values())
       {
            if(X509Utilities.isCertificateAuthority(certificate))       // Belgian root CA
            {
                rootCert=certificate;
                rootNode=new DefaultMutableTreeNode(new CertificateDisplayNode(certificate));
                break;
            }
       }

       if(rootCert!=null)
       {
            addCertHierarchy(rootCert,rootNode);
            treeModel=new DefaultTreeModel(rootNode);
            certsTree.setModel(treeModel);
            certsTree.setCellRenderer(treeCellRenderer);
            for(int i=0;i<certsTree.getRowCount();i++)
                certsTree.expandRow(i);
            certsDetailsSplitPane.setDividerLocation(authCertsPanel.getHeight());
            
            certsTree.setVisible(true);
       }
       else
       {
          certsTree.setVisible(false);
       }
    }

    private void initCellRenderer()
    {
        treeCellRenderer=new DefaultTreeCellRenderer();
        ImageIcon certIcon = new ImageIcon(ImageUtilities.getImage(CertificatesPanel.class, ICONS, "certificate_small.png"));
        if (certIcon != null)
        {
            treeCellRenderer.setLeafIcon(certIcon);
            treeCellRenderer.setOpenIcon(certIcon);
            treeCellRenderer.setClosedIcon(certIcon);
        }
    }

    public void valueChanged(TreeSelectionEvent treeSelectionEvent)
    {
       DefaultMutableTreeNode   treeNode=(DefaultMutableTreeNode)certsTree.getLastSelectedPathComponent();

       if(treeNode!=null)
       {
            CertificateDisplayNode   displayNode=(CertificateDisplayNode)treeNode.getUserObject();
            certificateSelected(displayNode.getCertificate());
       }
       else
       {
           certificateSelected(null);
       }
    }

    private void certificateSelected(final X509Certificate certificate)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                if(certificate!=null)
                {
                    dn.setText(getMultilineDN(certificate.getSubjectDN().getName()));
                    validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()));
                    validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    keyUsage.setText(getMultilineKeyUsage(X509Utilities.getKeyUsageStrings(bundle, certificate.getKeyUsage())));
                    
                    try
                    {
                        certificate.checkValidity();
                        validFromLabel  .setForeground(defaultLabelForeground);
                        validFrom       .setForeground(defaultLabelForeground);
                        validFrom       .setText(dateFormat.format(certificate.getNotBefore().getTime()));
                        validUntilLabel   .setForeground(defaultLabelForeground);
                        validUntil        .setForeground(defaultLabelForeground);
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    }
                    catch (CertificateExpiredException ex)
                    {
                        validFromLabel  .setForeground(defaultLabelForeground);
                        validFrom       .setForeground(defaultLabelForeground);
                        validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime()));
                        validUntilLabel   .setForeground(Color.red);
                        validUntil        .setForeground(Color.red);
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()) + " " + bundle.getString("notAfterWarning"));
                    }
                    catch (CertificateNotYetValidException ex)
                    {
                        validFromLabel  .setForeground(Color.red);
                        validFrom       .setForeground(Color.red);
                        validFrom.setText(dateFormat.format(certificate.getNotBefore().getTime())  + " " + bundle.getString("notBeforeWarning"));
                        validUntilLabel   .setForeground(defaultLabelForeground);
                        validUntil        .setForeground(defaultLabelForeground);
                        validUntil.setText(dateFormat.format(certificate.getNotAfter().getTime()));
                    }

                    dn.setEnabled(true);
                    keyUsage.setEnabled(true);
                    validFromLabel.setEnabled(true);
                    validUntilLabel.setEnabled(true);
                    validFrom.setEnabled(true);
                    validUntil.setEnabled(true);
                    certificateIcon.setEnabled(true);
                }
                else
                {
                    dn.setText("");
                    keyUsage.setText("");
                    validFrom.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);
                    validUntil.setText(IdFormatHelper.UNKNOWN_VALUE_TEXT);

                    dn.setEnabled(false);
                    keyUsage.setEnabled(false);
                    validFromLabel.setEnabled(false);
                    validUntilLabel.setEnabled(false);
                    validFrom.setEnabled(false);
                    validUntil.setEnabled(false);
                    certificateIcon.setEnabled(false);
                }
            }
        });
    }

    private String getMultilineDN(String dnStr)
    {
        StringBuilder html=new StringBuilder("<html>");
        String[] dnParts=dnStr.split("\\s*,\\s*");
        for(String dnPart : dnParts)
        {
            html.append(dnPart);
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }

    private String getMultilineKeyUsage(List<String> usages)
    {
        StringBuilder html=new StringBuilder("<html>");
        for(String usage : usages)
        {
            html.append(usage);
            html.append("<br/>");
        }
        html.append("</html>");
        return html.toString();
    }

    private class CertificateDisplayNode
    {
        private X509Certificate certificate;

        public CertificateDisplayNode(X509Certificate certificate)
        {
            this.certificate = certificate;
        }

        public X509Certificate getCertificate()
        {
            return certificate;
        }

        @Override
        public String toString()
        {
            return X509Utilities.getCN(certificate);
        }
    }
}
