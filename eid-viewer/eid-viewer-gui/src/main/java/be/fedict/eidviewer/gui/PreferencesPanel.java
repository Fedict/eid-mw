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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.util.ResourceBundle;
import javax.swing.SpinnerNumberModel;

/**
 *
 * @author Frank Marien
 */
public class PreferencesPanel extends javax.swing.JPanel implements ComponentListener
{
    private ResourceBundle          bundle;
    private TrustServiceController  trustServiceController;
    
    public PreferencesPanel()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/PreferencesPanel");
        initComponents();
        initProxyPrefsPanel();
        httpProxyPortSpinner.setModel(new SpinnerNumberModel(ViewerPrefs.getHTTPProxyPort(),1,65535,1));
        
    }

    public void start()
    {
        addComponentListener(this);
    }

    public void setTrustServiceController(TrustServiceController trustServiceController)
    {
        this.trustServiceController = trustServiceController;
    }

    private void fillProxyPrefs()
    {
        boolean proxyEnabled=ViewerPrefs.getUseHTTPProxy();
        
        httpProxyHost.setEnabled(proxyEnabled);
        httpProxyPortLabel.setEnabled(proxyEnabled);
        httpProxyPortSpinner.setEnabled(proxyEnabled);
        
        useProxyCheckbox.setSelected(proxyEnabled);
        httpProxyHost.setText(ViewerPrefs.getHTTPProxyHost());
        httpProxyPortSpinner.setValue(ViewerPrefs.getHTTPProxyPort());
    }

    private void applyProxyPrefs()
    {
        ViewerPrefs.setHTTPProxyHost(httpProxyHost.getText());
        ViewerPrefs.setHTTPProxyPort((Integer)httpProxyPortSpinner.getValue());
        ViewerPrefs.setUseHTTPProxy(useProxyCheckbox.isSelected());
        
        if(ViewerPrefs.getUseHTTPProxy())
        {
              trustServiceController.setProxy(ViewerPrefs.getHTTPProxyHost(), ViewerPrefs.getHTTPProxyPort());
        }
        else
        {
            if(ViewerPrefs.getStartupUseHttpProxy())
            {
               trustServiceController.setProxy(ViewerPrefs.getStartupHttpProxyHost(), ViewerPrefs.getStartupHttpProxyPort());
            }
            else
            {
                System.clearProperty("http.proxyHost");
                System.clearProperty("https.proxyHost");
                System.clearProperty("http.proxyPort");
                System.clearProperty("https.proxyPort");
            }
        }
    }

    private void initProxyPrefsPanel()
    {
        useProxyCheckbox.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent ae)
            {
                ViewerPrefs.setUseHTTPProxy(useProxyCheckbox.isSelected());
                applyProxyPrefs();
                fillProxyPrefs();
            }
        });
    }



    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        proxyPrefsPanel = new javax.swing.JPanel();
        httpProxyPortLabel = new javax.swing.JLabel();
        httpProxyPortSpinner = new javax.swing.JSpinner();
        useProxyCheckbox = new javax.swing.JCheckBox();
        httpProxyHost = new javax.swing.JTextField();

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(204, 255, 204), 24, true));
        setLayout(new java.awt.BorderLayout());

        proxyPrefsPanel.setBorder(javax.swing.BorderFactory.createCompoundBorder(javax.swing.BorderFactory.createEmptyBorder(16, 16, 16, 16), javax.swing.BorderFactory.createTitledBorder("Configure Proxies to Access the Internet:")));
        proxyPrefsPanel.setName("proxyPrefsPanel"); // NOI18N
        proxyPrefsPanel.setLayout(new java.awt.GridBagLayout());

        httpProxyPortLabel.setText("Port:");
        httpProxyPortLabel.setName("httpProxyPortLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(httpProxyPortLabel, gridBagConstraints);

        httpProxyPortSpinner.setName("httpProxyPortSpinner"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(httpProxyPortSpinner, gridBagConstraints);

        useProxyCheckbox.setText("HTTP Proxy:");
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
        httpProxyHost.setOpaque(false);
        httpProxyHost.setPreferredSize(new java.awt.Dimension(256, 18));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        proxyPrefsPanel.add(httpProxyHost, gridBagConstraints);

        add(proxyPrefsPanel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField httpProxyHost;
    private javax.swing.JLabel httpProxyPortLabel;
    private javax.swing.JSpinner httpProxyPortSpinner;
    private javax.swing.JPanel proxyPrefsPanel;
    private javax.swing.JCheckBox useProxyCheckbox;
    // End of variables declaration//GEN-END:variables


    public void componentShown(ComponentEvent ce)
    {
        fillProxyPrefs();
    }

    public void componentHidden(ComponentEvent ce)
    {
        applyProxyPrefs();
    }

    public void componentResized(ComponentEvent ce){}
    public void componentMoved(ComponentEvent ce){}
}
