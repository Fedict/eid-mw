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

import be.fedict.eidviewer.gui.helper.PositiveIntegerDocument;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.util.Observable;
import java.util.Observer;
import java.util.ResourceBundle;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

/**
 *
 * @author Frank Marien
 */
public class PreferencesPanel extends javax.swing.JPanel implements Observer, ComponentListener
{
    private ResourceBundle          bundle;
    private TrustServiceController  trustServiceController;
    private EidController           eidController;
    private DiagnosticsContainer    diagnosticsContainer;

    public PreferencesPanel()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/PreferencesPanel");
        initComponents();
        initProxyPrefsPanel();
        initDiagnosticsPrefsPanel();
    }

    public void start()
    {
        addComponentListener(this);
    }

    public PreferencesPanel setEidController(EidController eidController)
    {
        this.eidController = eidController;
        return this;
    }

    public PreferencesPanel setTrustServiceController(TrustServiceController trustServiceController)
    {
        this.trustServiceController = trustServiceController;
        return this;
    }

    public PreferencesPanel setDiagnosticsContainer(DiagnosticsContainer diagnosticsContainer)
    {
        this.diagnosticsContainer = diagnosticsContainer;
        return this;
    }

    private void fillProxyPrefs()
    {
        boolean proxyEnabled = ViewerPrefs.getUseHTTPProxy();

        useProxyCheckbox.setSelected(proxyEnabled);
        httpProxyHost.setText(ViewerPrefs.getHTTPProxyHost());
        httpProxyPort.setText(String.valueOf(ViewerPrefs.getHTTPProxyPort()));

        updateProxyComponentsEnabled();
    }

    private void applyProxyPrefs()
    {
        ViewerPrefs.setHTTPProxyHost(httpProxyHost.getText());
        ViewerPrefs.setHTTPProxyPort(Integer.parseInt(httpProxyPort.getText()));
        ViewerPrefs.setUseHTTPProxy(useProxyCheckbox.isSelected());

        // if we have our own proxy settings, apply them
        if (ViewerPrefs.getUseHTTPProxy())
            trustServiceController.setProxy(ViewerPrefs.getHTTPProxyHost(), ViewerPrefs.getHTTPProxyPort());
        else
            trustServiceController.setProxy(null, 0);
    }

    private void initProxyPrefsPanel()
    {  
        applyProxyButton.addActionListener(new ActionListener()
        {

            public void actionPerformed(ActionEvent ae)
            {
                applyProxyPrefs();
                updateApplyButtonEnabled();
            }
        });

        useProxyCheckbox.addActionListener(new ActionListener()
        {

            public void actionPerformed(ActionEvent ae)
            {
                updateProxyComponentsEnabled();
                updateApplyButtonEnabled();
            }
        });

        httpProxyPort.setDocument(new PositiveIntegerDocument(5));
        httpProxyPort.getDocument().addDocumentListener(new DocumentListener()
        {
            public void insertUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }

            public void removeUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }

            public void changedUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }
        });

        httpProxyHost.getDocument().addDocumentListener(new DocumentListener()
        {
            public void insertUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }

            public void removeUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }

            public void changedUpdate(DocumentEvent de)
            {
                updateApplyButtonEnabled();
            }
        });    
    }

    private void updateProxyComponentsEnabled()
    {
        boolean enabled = useProxyCheckbox.isSelected();
        httpProxyHost.setEnabled(enabled);
        httpProxyPortLabel.setEnabled(enabled);
        httpProxyPort.setEnabled(enabled);
    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jPanel1 = new javax.swing.JPanel();
        proxyPrefsPanel = new javax.swing.JPanel();
        httpProxyPortLabel = new javax.swing.JLabel();
        useProxyCheckbox = new javax.swing.JCheckBox();
        httpProxyHost = new javax.swing.JTextField();
        applyProxyButton = new javax.swing.JButton();
        spacer = new javax.swing.JLabel();
        httpProxyPort = new javax.swing.JTextField();
        diagnosticsPrefsPanel = new javax.swing.JPanel();
        showLogCheckbox = new javax.swing.JCheckBox();
        showDiagnosticsCheckbox = new javax.swing.JCheckBox();
        spacer1 = new javax.swing.JLabel();

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(204, 255, 204), 24, true));
        setLayout(new java.awt.BorderLayout());

        jPanel1.setName("jPanel1"); // NOI18N
        jPanel1.setLayout(new java.awt.GridLayout(2, 1));

        proxyPrefsPanel.setBorder(javax.swing.BorderFactory.createCompoundBorder(javax.swing.BorderFactory.createEtchedBorder(), javax.swing.BorderFactory.createEmptyBorder(16, 16, 16, 16)));
        proxyPrefsPanel.setName("proxyPrefsPanel"); // NOI18N
        proxyPrefsPanel.setLayout(new java.awt.GridBagLayout());

        httpProxyPortLabel.setText(bundle.getString("portLabel")); // NOI18N
        httpProxyPortLabel.setName("httpProxyPortLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(httpProxyPortLabel, gridBagConstraints);

        useProxyCheckbox.setText(bundle.getString("proxyCheckbox")); // NOI18N
        useProxyCheckbox.setName("useProxyCheckbox"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(useProxyCheckbox, gridBagConstraints);

        httpProxyHost.setMinimumSize(new java.awt.Dimension(128, 18));
        httpProxyHost.setName("httpProxyHost"); // NOI18N
        httpProxyHost.setPreferredSize(new java.awt.Dimension(256, 18));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(httpProxyHost, gridBagConstraints);

        applyProxyButton.setText(bundle.getString("applyButton")); // NOI18N
        applyProxyButton.setEnabled(false);
        applyProxyButton.setName("applyProxyButton"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        proxyPrefsPanel.add(applyProxyButton, gridBagConstraints);

        spacer.setName("spacer"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.insets = new java.awt.Insets(8, 8, 8, 8);
        proxyPrefsPanel.add(spacer, gridBagConstraints);

        httpProxyPort.setText("8080");
        httpProxyPort.setMinimumSize(new java.awt.Dimension(48, 18));
        httpProxyPort.setName("httpProxyPort"); // NOI18N
        httpProxyPort.setPreferredSize(new java.awt.Dimension(48, 18));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        proxyPrefsPanel.add(httpProxyPort, gridBagConstraints);

        jPanel1.add(proxyPrefsPanel);

        diagnosticsPrefsPanel.setBorder(javax.swing.BorderFactory.createCompoundBorder(javax.swing.BorderFactory.createEtchedBorder(), javax.swing.BorderFactory.createEmptyBorder(16, 16, 16, 16)));
        diagnosticsPrefsPanel.setName("diagnosticsPrefsPanel"); // NOI18N
        diagnosticsPrefsPanel.setLayout(new java.awt.GridBagLayout());

        showLogCheckbox.setText(bundle.getString("showLogTab")); // NOI18N
        showLogCheckbox.setName("showLogCheckbox"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        diagnosticsPrefsPanel.add(showLogCheckbox, gridBagConstraints);

        showDiagnosticsCheckbox.setText("Show Diagnostics Tab");
        showDiagnosticsCheckbox.setEnabled(false);
        showDiagnosticsCheckbox.setName("showDiagnosticsCheckbox"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        diagnosticsPrefsPanel.add(showDiagnosticsCheckbox, gridBagConstraints);

        spacer1.setName("spacer1"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.insets = new java.awt.Insets(8, 8, 8, 8);
        diagnosticsPrefsPanel.add(spacer1, gridBagConstraints);

        jPanel1.add(diagnosticsPrefsPanel);

        add(jPanel1, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton applyProxyButton;
    private javax.swing.JPanel diagnosticsPrefsPanel;
    private javax.swing.JTextField httpProxyHost;
    private javax.swing.JTextField httpProxyPort;
    private javax.swing.JLabel httpProxyPortLabel;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel proxyPrefsPanel;
    private javax.swing.JCheckBox showDiagnosticsCheckbox;
    private javax.swing.JCheckBox showLogCheckbox;
    private javax.swing.JLabel spacer;
    private javax.swing.JLabel spacer1;
    private javax.swing.JCheckBox useProxyCheckbox;
    // End of variables declaration//GEN-END:variables

    public void update(Observable o, Object o1)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                updateApplyButtonEnabled();
            }
        });
    }

    private void updateApplyButtonEnabled()
    {
        applyProxyButton.setEnabled(canApply());
    }

    private boolean canApply()
    {
        if (!eidController.isReadyForCommand())
        {
            return false;
        }

        if (httpProxyHost.getText().isEmpty())
        {
            return false;
        }

        String hostName=httpProxyHost.getText();
        int    portNumber;

        try
        {
            portNumber=Integer.parseInt(httpProxyPort.getText());
            if(portNumber<1 || portNumber>65535)
                return false;
        }
        catch(NumberFormatException nfe)
        {
            return false;
        }

        if(useProxyCheckbox.isSelected())
            return (!hostName.equals(ViewerPrefs.getHTTPProxyHost()) || (portNumber!=ViewerPrefs.getHTTPProxyPort()) || (!ViewerPrefs.getUseHTTPProxy()));
        else
            return ViewerPrefs.getUseHTTPProxy();
    }

    public void componentShown(ComponentEvent componentEvent)
    {
        fillProxyPrefs();
        fillDiagnosticsPrefs();
    }

    public void componentResized(ComponentEvent ce)
    {
    }

    public void componentMoved(ComponentEvent ce)
    {
    }

    public void componentHidden(ComponentEvent ce)
    {
    }

    private void initDiagnosticsPrefsPanel()
    {
        showLogCheckbox.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent actionEvent)
            {
                ViewerPrefs.setShowLogTab(showLogCheckbox.isSelected());
                if(diagnosticsContainer!=null)
                    diagnosticsContainer.showLog(showLogCheckbox.isSelected());
            }
        });
    }

    private void fillDiagnosticsPrefs()
    {
        showLogCheckbox.setSelected(ViewerPrefs.getShowLogTab());
    }
}
