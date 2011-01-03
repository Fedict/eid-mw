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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.util.ResourceBundle;

import be.fedict.eid.applet.DiagnosticTests;
import be.fedict.eid.applet.Messages;
import be.fedict.eid.applet.Messages.MESSAGE_ID;
import be.fedict.eid.applet.Status;
import be.fedict.eid.applet.View;
import be.fedict.eidviewer.lib.Eid;
import be.fedict.eidviewer.lib.EidFactory;
import java.awt.Component;
import java.awt.Toolkit;
import java.awt.print.PrinterException;
import java.awt.print.PrinterJob;
import java.util.EnumMap;
import java.util.Locale;
import java.util.Observable;
import java.util.Observer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.ActionMap;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;
import org.jdesktop.application.Action;
import org.jdesktop.application.Application;

/**
 *
 * @author Frank Marien
 */
public class BelgianEidViewer extends javax.swing.JFrame implements View, Observer
{

    private ResourceBundle bundle;
    private static final String EXTENSION_PNG = ".png";
    private static final String ICONS = "resources/icons/";
    private Messages coreMessages;
    private Eid eid;
    private EidController eidController;
    private TrustServiceController trustServiceController;
    private EnumMap<EidController.STATE, ImageIcon> cardStatusIcons;
    private EnumMap<EidController.STATE, String> cardStatusTexts;
    private EnumMap<EidController.ACTIVITY, String> activityTexts;
    private IdentityPanel identityPanel;
    private CertificatesPanel certificatesPanel;
    private CardPanel cardPanel;
    private PreferencesPanel preferencesPanel;
    private javax.swing.Action printAction;

    public BelgianEidViewer()
    {
        bundle = ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/BelgianEidViewer");
        coreMessages = new Messages(Locale.getDefault());
        initComponents();
        initPanels();
        initIcons();
        initTexts();
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    private void start()
    {
        ActionMap actionMap = Application.getInstance().getContext().getActionMap(BelgianEidViewer.class, this);
        printAction = actionMap.get("print"); // NOI18N
        eid = EidFactory.getEidImpl(this, coreMessages);
        eidController = new EidController(eid);
        trustServiceController = new TrustServiceController(ViewerPrefs.getTrustServiceURL());

        if(ViewerPrefs.getUseHTTPProxy())
            trustServiceController.setProxy(ViewerPrefs.getHTTPProxyHost(),ViewerPrefs.getHTTPProxyPort());

        eidController.setTrustServiceController(trustServiceController);
        eidController.setAutoValidateTrust(ViewerPrefs.getIsAutoValidating());
        cardPanel.setEidController(eidController);
        certificatesPanel.setEidController(eidController);
        certificatesPanel.start();
        preferencesPanel.setTrustServiceController(trustServiceController);
        preferencesPanel.start();
        eidController.addObserver(identityPanel);
        eidController.addObserver(cardPanel);
        eidController.addObserver(certificatesPanel);
        eidController.addObserver(this);
        eidController.start();

        setVisible(true);
    }

    private void stop()
    {
        eidController.stop();
        this.dispose();
    }

    public void update(Observable o, Object o1)
    {
        updateVisibleState();
    }

    private void updateVisibleState()
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                printAction.setEnabled(eidController.hasIdentity() && eidController.hasAddress() && eidController.hasPhoto());
                statusIcon.setIcon(cardStatusIcons.get(eidController.getState()));

                if (eidController.getState() == EidController.STATE.EID_PRESENT)
                {
                    statusText.setText(activityTexts.get(eidController.getActivity()));
                }
                else
                {
                    statusText.setText(cardStatusTexts.get(eidController.getState()));
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

        tabPanel = new JTabbedPane();
        statusPanel = new JPanel();
        statusIcon = new JLabel();
        statusText = new JLabel();
        printPanel = new JPanel();
        printButton = new JButton();
        menuBar = new JMenuBar();
        fileMenu = new JMenu();
        jMenuItem1 = new JMenuItem();
        fileMenuQuitItem = new JMenuItem();

        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        setBackground(new Color(255, 255, 255));
        setMinimumSize(new Dimension(640, 480));

        tabPanel.setName("tabPanel"); // NOI18N
        tabPanel.setPreferredSize(new Dimension(600, 512));
        getContentPane().add(tabPanel, BorderLayout.CENTER);

        statusPanel.setName("statusPanel"); // NOI18N
        statusPanel.setLayout(new BorderLayout());

        statusIcon.setHorizontalAlignment(SwingConstants.CENTER);
        statusIcon.setIcon(new ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/icons/state_noeidpresent.png"))); // NOI18N
        statusIcon.setMaximumSize(new Dimension(72, 72));
        statusIcon.setMinimumSize(new Dimension(72, 72));
        statusIcon.setName("statusIcon"); // NOI18N
        statusIcon.setPreferredSize(new Dimension(90, 72));
        statusPanel.add(statusIcon, BorderLayout.EAST);

        statusText.setHorizontalAlignment(SwingConstants.RIGHT);
        statusText.setName("statusText"); // NOI18N
        statusPanel.add(statusText, BorderLayout.CENTER);

        printPanel.setMinimumSize(new Dimension(72, 72));
        printPanel.setName("printPanel"); // NOI18N
        printPanel.setPreferredSize(new Dimension(72, 72));
        printPanel.setLayout(new GridBagLayout());

        ActionMap actionMap = Application.getInstance().getContext().getActionMap(BelgianEidViewer.class, this);
        printButton.setAction(actionMap.get("print")); // NOI18N
        printButton.setIcon(new ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/icons/print.png"))); // NOI18N
        printButton.setMnemonic('p');
        printButton.setEnabled(false);
        printButton.setHideActionText(true);
        printButton.setMaximumSize(new Dimension(200, 50));
        printButton.setMinimumSize(new Dimension(50, 50));
        printButton.setName("printButton"); // NOI18N
        printButton.setPreferredSize(new Dimension(200, 50));
        printPanel.add(printButton, new GridBagConstraints());

        statusPanel.add(printPanel, BorderLayout.WEST);

        getContentPane().add(statusPanel, BorderLayout.SOUTH);

        menuBar.setName("menuBar"); // NOI18N

        fileMenu.setText(bundle.getString("fileMenuTitle")); // NOI18N
        fileMenu.setName("fileMenu"); // NOI18N

        jMenuItem1.setAction(actionMap.get("print")); // NOI18N
        jMenuItem1.setText(bundle.getString("fileMenuPrintItem")); // NOI18N
        jMenuItem1.setName("jMenuItem1"); // NOI18N
        fileMenu.add(jMenuItem1);

        fileMenuQuitItem.setAction(actionMap.get("quit")); // NOI18N
        fileMenuQuitItem.setText(bundle.getString("fileMenuQuitItem")); // NOI18N
        fileMenuQuitItem.setName("fileMenuQuitItem"); // NOI18N
        fileMenu.add(fileMenuQuitItem);

        menuBar.add(fileMenu);

        setJMenuBar(menuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    public void addDetailMessage(String detailMessage)
    {
        System.err.println(detailMessage);
    }

    public void setStatusMessage(Status status, MESSAGE_ID messageId)
    {
        String message = coreMessages.getMessage(messageId);
        System.err.println(message);

        setStatusMessage(message);
    }

    private void setStatusMessage(final String message)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                //idStatus.setText(message);
            }
        });
    }

    public boolean privacyQuestion(boolean includeAddress, boolean includePhoto, String identityDataUsage)
    {
        return true;
    }

    public Component getParentComponent()
    {
        return this;
    }

    public void addTestResult(DiagnosticTests diagnosticTest, boolean success, String description)
    {
        System.err.println(description);
    }

    public void resetProgress(int max)
    {
        setProgress(0);
        System.err.println("|<<");
    }

    public void increaseProgress()
    {
        //setProgress(idProgressBar.getValue() + 1);
        System.err.println(">>");
    }

    private void setProgress(final int progress)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                //idProgressBar.setValue(progress);
            }
        });
    }

    public void setProgressIndeterminate()
    {
        System.err.println("???");
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                //idProgressBar.setIndeterminate(true);
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    JMenu fileMenu;
    JMenuItem fileMenuQuitItem;
    JMenuItem jMenuItem1;
    JMenuBar menuBar;
    JButton printButton;
    JPanel printPanel;
    JLabel statusIcon;
    JPanel statusPanel;
    JLabel statusText;
    JTabbedPane tabPanel;
    // End of variables declaration//GEN-END:variables

    private void initPanels()
    {
        ResourceBundle res = java.util.ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/BelgianEidViewer");
        identityPanel = new IdentityPanel();
        cardPanel = new CardPanel();
        certificatesPanel = new CertificatesPanel();
        preferencesPanel = new PreferencesPanel();
        tabPanel.add(identityPanel, res.getString("IDENTITY"));
        tabPanel.add(cardPanel, res.getString("CARD"));
        tabPanel.add(certificatesPanel, res.getString("CERTIFICATES"));
        tabPanel.add(preferencesPanel, res.getString("PREFERENCES"));
    }

    private void initIcons()
    {
        cardStatusIcons = new EnumMap<EidController.STATE, ImageIcon>(EidController.STATE.class);
        cardStatusIcons.put(EidController.STATE.NO_READERS, getIcon(EidController.STATE.NO_READERS + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.ERROR, getIcon(EidController.STATE.ERROR + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.NO_EID_PRESENT, getIcon(EidController.STATE.NO_EID_PRESENT + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.EID_PRESENT, getIcon(EidController.STATE.EID_PRESENT + EXTENSION_PNG));
    }

    private void initTexts()
    {
        cardStatusTexts = new EnumMap<EidController.STATE, String>(EidController.STATE.class);
        cardStatusTexts.put(EidController.STATE.NO_READERS, bundle.getString(EidController.STATE.NO_READERS.toString()));
        cardStatusTexts.put(EidController.STATE.ERROR, bundle.getString(EidController.STATE.ERROR.toString()));
        cardStatusTexts.put(EidController.STATE.NO_EID_PRESENT, bundle.getString(EidController.STATE.NO_EID_PRESENT.toString()));
        activityTexts = new EnumMap<EidController.ACTIVITY, String>(EidController.ACTIVITY.class);
        activityTexts.put(EidController.ACTIVITY.IDLE, bundle.getString(EidController.ACTIVITY.IDLE.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_IDENTITY, bundle.getString(EidController.ACTIVITY.READING_IDENTITY.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_ADDRESS, bundle.getString(EidController.ACTIVITY.READING_ADDRESS.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_PHOTO, bundle.getString(EidController.ACTIVITY.READING_PHOTO.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_AUTH_CHAIN, bundle.getString(EidController.ACTIVITY.READING_AUTH_CHAIN.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_SIGN_CHAIN, bundle.getString(EidController.ACTIVITY.READING_SIGN_CHAIN.toString()));
    }

    private ImageIcon getIcon(String name)
    {
        return new ImageIcon(Toolkit.getDefaultToolkit().getImage(BelgianEidViewer.class.getResource(ICONS + name)));
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String args[])
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                new BelgianEidViewer().start();
            }
        });
    }

    @Action
    public void print()
    {
        PrinterJob job = PrinterJob.getPrinterJob();
        IDPrintout printout = new IDPrintout();
        printout.setIdentity(eidController.getIdentity());
        printout.setAddress(eidController.getAddress());
        printout.setPhoto(eidController.getPhoto());

        job.setPrintable(printout);
        boolean ok = job.printDialog();
        //job.setJobName(eidController.getIdentity().getNationalNumber());

        if (ok)
        {
            try
            {
                job.print();
            }
            catch (PrinterException pex)
            {
                Logger.getLogger(BelgianEidViewer.class.getName()).log(Level.SEVERE, null, pex);
            }
        }
    }

    @Action
    public void quit()
    {
        this.stop();
    }
}
