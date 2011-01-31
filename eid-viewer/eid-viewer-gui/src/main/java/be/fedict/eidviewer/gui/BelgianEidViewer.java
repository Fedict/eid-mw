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
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ResourceBundle;

import be.fedict.eid.applet.DiagnosticTests;
import be.fedict.eid.applet.Messages;
import be.fedict.eid.applet.Messages.MESSAGE_ID;
import be.fedict.eid.applet.Status;
import be.fedict.eid.applet.View;
import be.fedict.eidviewer.gui.helper.EidFileFilter;
import be.fedict.eidviewer.gui.helper.EidFilePreviewAccessory;
import be.fedict.eidviewer.gui.helper.EidFileView;
import be.fedict.eidviewer.gui.helper.ImageUtilities;
import be.fedict.eidviewer.lib.Eid;
import be.fedict.eidviewer.lib.EidFactory;
import java.awt.Component;
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
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu.Separator;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;
import javax.swing.UIManager;
import javax.swing.WindowConstants;
import org.jdesktop.application.Action;
import org.jdesktop.application.Application;

/**
 *
 * @author Frank Marien
 */
public class BelgianEidViewer extends javax.swing.JFrame implements View, Observer, DiagnosticsContainer
{
    private static final Logger logger = Logger.getLogger(BelgianEidViewer.class.getName());
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
    private LogPanel logPanel;
    private javax.swing.Action printAction, openAction, saveAction, closeAction;

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
        logger.fine("starting..");
        ActionMap actionMap = Application.getInstance().getContext().getActionMap(BelgianEidViewer.class, this);
        printAction = actionMap.get("print");       // NOI18N
        openAction = actionMap.get("openFile");    // NOI18N
        closeAction = actionMap.get("closeFile");   // NOI18N
        saveAction = actionMap.get("saveFile");    // NOI18N
        eid = EidFactory.getEidImpl(this, coreMessages);
        eidController = new EidController(eid);

        trustServiceController = new TrustServiceController(ViewerPrefs.getTrustServiceURL());
        trustServiceController.start();

        if (ViewerPrefs.getUseHTTPProxy())
        {
            trustServiceController.setProxy(ViewerPrefs.getHTTPProxyHost(), ViewerPrefs.getHTTPProxyPort());
        }
        else
        {
            trustServiceController.setProxy(null, 0);
        }

        eidController.setTrustServiceController(trustServiceController);
        eidController.setAutoValidateTrust(ViewerPrefs.getIsAutoValidating());

        identityPanel.setEidController(eidController);

        cardPanel.setEidController(eidController);
        
        certificatesPanel.setEidController(eidController);
        certificatesPanel.start();

        preferencesPanel.setTrustServiceController(trustServiceController);
        preferencesPanel.setEidController(eidController);
        preferencesPanel.setDiagnosticsContainer(this);
        preferencesPanel.start();

        eidController.addObserver(identityPanel);
        eidController.addObserver(cardPanel);
        eidController.addObserver(certificatesPanel);
        eidController.addObserver(preferencesPanel);
        eidController.addObserver(this);
        eidController.start();

        setVisible(true);
    }

    private void stop()
    {
        logger.fine("stopping..");
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
                printAction.setEnabled(eidController.hasIdentity() && eidController.hasAddress() && eidController.hasPhoto() && (PrinterJob.lookupPrintServices().length>0));
                saveAction.setEnabled(eidController.hasIdentity() && eidController.hasAddress() && eidController.hasPhoto() && eidController.hasAuthCertChain());
                openAction.setEnabled(eidController.getState() != EidController.STATE.EID_PRESENT && eidController.getState()!=EidController.STATE.EID_YIELDED);
                closeAction.setEnabled(eidController.isLoadedFromFile() && (eidController.hasAddress() || eidController.hasPhoto() || eidController.hasAuthCertChain() || eidController.hasSignCertChain()));

                statusIcon.setIcon(cardStatusIcons.get(eidController.getState()));

                switch (eidController.getState())
                {
                    case EID_PRESENT:
                        statusText.setText(activityTexts.get(eidController.getActivity()));
                        break;

                    case FILE_LOADED:
                    default:
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
        jSeparator2 = new Separator();
        jMenuItem2 = new JMenuItem();
        jMenuItem3 = new JMenuItem();
        jMenuItem4 = new JMenuItem();
        jSeparator3 = new Separator();
        jMenuItem1 = new JMenuItem();
        jSeparator1 = new Separator();
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

        jSeparator2.setName("jSeparator2"); // NOI18N
        fileMenu.add(jSeparator2);

        jMenuItem2.setAction(actionMap.get("openFile")); // NOI18N
        jMenuItem2.setName("jMenuItem2"); // NOI18N
        fileMenu.add(jMenuItem2);

        jMenuItem3.setAction(actionMap.get("saveFile")); // NOI18N
        jMenuItem3.setName("jMenuItem3"); // NOI18N
        fileMenu.add(jMenuItem3);

        jMenuItem4.setAction(actionMap.get("closeFile")); // NOI18N
        jMenuItem4.setName("jMenuItem4"); // NOI18N
        fileMenu.add(jMenuItem4);

        jSeparator3.setName("jSeparator3"); // NOI18N
        fileMenu.add(jSeparator3);

        jMenuItem1.setAction(actionMap.get("print")); // NOI18N
        jMenuItem1.setText(bundle.getString("fileMenuPrintItem")); // NOI18N
        jMenuItem1.setName("jMenuItem1"); // NOI18N
        fileMenu.add(jMenuItem1);

        jSeparator1.setName("jSeparator1"); // NOI18N
        fileMenu.add(jSeparator1);

        fileMenuQuitItem.setAction(actionMap.get("quit")); // NOI18N
        fileMenuQuitItem.setText(bundle.getString("fileMenuQuitItem")); // NOI18N
        fileMenuQuitItem.setName("fileMenuQuitItem"); // NOI18N
        fileMenu.add(fileMenuQuitItem);

        menuBar.add(fileMenu);

        setJMenuBar(menuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    JMenu fileMenu;
    JMenuItem fileMenuQuitItem;
    JMenuItem jMenuItem1;
    JMenuItem jMenuItem2;
    JMenuItem jMenuItem3;
    JMenuItem jMenuItem4;
    Separator jSeparator1;
    Separator jSeparator2;
    Separator jSeparator3;
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
        identityPanel = new IdentityPanel();
        cardPanel = new CardPanel();
        certificatesPanel = new CertificatesPanel();
        preferencesPanel = new PreferencesPanel();
        tabPanel.add(identityPanel, bundle.getString("IDENTITY"));
        tabPanel.add(cardPanel, bundle.getString("CARD"));
        tabPanel.add(certificatesPanel, bundle.getString("CERTIFICATES"));
        tabPanel.add(preferencesPanel, bundle.getString("PREFERENCES"));

        if(ViewerPrefs.getShowLogTab())
            showLog(true);
    }

    private void initIcons()
    {
        cardStatusIcons = new EnumMap<EidController.STATE, ImageIcon>(EidController.STATE.class);
        cardStatusIcons.put(EidController.STATE.NO_READERS,     ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.NO_READERS + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.ERROR,          ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.ERROR + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.NO_EID_PRESENT, ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.NO_EID_PRESENT + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.EID_PRESENT,    ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.EID_PRESENT + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.FILE_LOADING,   ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.FILE_LOADING + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.FILE_LOADED,    ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.FILE_LOADED + EXTENSION_PNG));
        cardStatusIcons.put(EidController.STATE.EID_YIELDED,    ImageUtilities.getIcon(this.getClass(),ICONS+EidController.STATE.EID_YIELDED + EXTENSION_PNG));
    }

    private void initTexts()
    {
        cardStatusTexts = new EnumMap<EidController.STATE, String>(EidController.STATE.class);
        cardStatusTexts.put(EidController.STATE.NO_READERS, bundle.getString(EidController.STATE.NO_READERS.toString()));
        cardStatusTexts.put(EidController.STATE.ERROR, bundle.getString(EidController.STATE.ERROR.toString()));
        cardStatusTexts.put(EidController.STATE.NO_EID_PRESENT, bundle.getString(EidController.STATE.NO_EID_PRESENT.toString()));
        cardStatusTexts.put(EidController.STATE.FILE_LOADING, bundle.getString(EidController.STATE.FILE_LOADING.toString()));
        cardStatusTexts.put(EidController.STATE.FILE_LOADED, bundle.getString(EidController.STATE.FILE_LOADED.toString()));
        cardStatusTexts.put(EidController.STATE.EID_YIELDED, bundle.getString(EidController.STATE.EID_YIELDED.toString()));
        activityTexts = new EnumMap<EidController.ACTIVITY, String>(EidController.ACTIVITY.class);
        activityTexts.put(EidController.ACTIVITY.IDLE, bundle.getString(EidController.ACTIVITY.IDLE.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_IDENTITY, bundle.getString(EidController.ACTIVITY.READING_IDENTITY.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_ADDRESS, bundle.getString(EidController.ACTIVITY.READING_ADDRESS.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_PHOTO, bundle.getString(EidController.ACTIVITY.READING_PHOTO.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_RRN_CHAIN, bundle.getString(EidController.ACTIVITY.READING_RRN_CHAIN.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_AUTH_CHAIN, bundle.getString(EidController.ACTIVITY.READING_AUTH_CHAIN.toString()));
        activityTexts.put(EidController.ACTIVITY.READING_SIGN_CHAIN, bundle.getString(EidController.ACTIVITY.READING_SIGN_CHAIN.toString()));
        activityTexts.put(EidController.ACTIVITY.VALIDATING_IDENTITY, bundle.getString(EidController.ACTIVITY.VALIDATING_IDENTITY.toString()));
        activityTexts.put(EidController.ACTIVITY.VALIDATING_ADDRESS, bundle.getString(EidController.ACTIVITY.VALIDATING_ADDRESS.toString()));
    }

   

    @Action
    public void print()
    {
        logger.fine("print action chosen..");
        PrinterJob job = PrinterJob.getPrinterJob();
        job.setJobName(eidController.getIdentity().getNationalNumber());
        
        IDPrintout printout = new IDPrintout();
        printout.setIdentity(eidController.getIdentity());
        printout.setAddress(eidController.getAddress());
        
        try
        {
            printout.setPhoto(eidController.getPhotoImage());
        }
        catch (IOException ex)
        {
            logger.log(Level.SEVERE, "Photo conversion from JPEG Failed", ex);
        }

        job.setPrintable(printout);
        
        boolean ok = job.printDialog();


        if (ok)
        {
            try
            {
                logger.finest("starting print job..");
                job.print();
                logger.finest("print job completed.");
            }
            catch (PrinterException pex)
            {
                logger.log(Level.SEVERE, "Print Job Failed", pex);
            }
        }
    }

    @Action
    public void quit()
    {
        logger.fine("quit action chosen..");
        this.stop();
    }

    @Action
    public void openFile() throws FileNotFoundException, IOException
    {
        logger.fine("Open action chosen..");
        final JFileChooser fileChooser = new JFileChooser();

        fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        fileChooser.setAcceptAllFileFilterUsed(false);

        fileChooser.addChoosableFileFilter(new EidFileFilter(true, true, true,  bundle.getString("allEIDFiles")));
        fileChooser.addChoosableFileFilter(new EidFileFilter(true, false,false, bundle.getString("xmlEIDFiles")));
        fileChooser.addChoosableFileFilter(new EidFileFilter(false,true, false, bundle.getString("csvEIDFiles")));
        fileChooser.addChoosableFileFilter(new EidFileFilter(false,false,true,  bundle.getString("tlvEIDFiles")));

        fileChooser.setFileView(new EidFileView(bundle));

        EidFilePreviewAccessory preview=new EidFilePreviewAccessory(bundle);
        fileChooser.setAccessory(preview);
        fileChooser.addPropertyChangeListener(preview);

        if(fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION)
        {
            File file=fileChooser.getSelectedFile();
            if(file.isFile())
                eidController.loadFromFile(file);
        }
    }

    @Action
    public void saveFile()
    {
        logger.fine("Save action chosen..");
        final JFileChooser fileChooser = new JFileChooser();
        if (fileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION)
        {
            eidController.saveToXMLFile(fileChooser.getSelectedFile());
        }
    }

    @Action
    public void closeFile()
    {
        logger.fine("Close action chosen..");
        eidController.closeFile();
    }

 
    /* ------------------------ Interaction meant for Applets ---------------------------------------------------------------- */

    public void addDetailMessage(String detailMessage)
    {
        logger.finest(detailMessage);
    }

    public void setStatusMessage(Status status, MESSAGE_ID messageId)
    {
        String message = coreMessages.getMessage(messageId);
        logger.info(message);
    }

    public boolean privacyQuestion(boolean includeAddress, boolean includePhoto, String identityDataUsage)
    {
        // this app's only purpose being to read eID cards.. asking "are you sure" is merely annoying to the user
        // and gives no extra security whatsoever
        // (the privacyQuestion was designed for Applets, where it makes a *lot* of sense)
        return true;
    }

    public Component getParentComponent()
    {
        return this;
    }

    /* ------------------------Unused from Applet Core ---------------------------------------------------------------- */

    public void addTestResult(DiagnosticTests diagnosticTest, boolean success, String description)
    {
    }

    public void resetProgress(int max)
    {
    }

    public void increaseProgress()
    {
    }

    private void setProgress(final int progress)
    {
    }

    public void setProgressIndeterminate()
    {
    }

    /* ---------------------------------------------------------------------------------------- */

    public static void main(String args[])
    {
        try
        {
            logger.finest("Setting System Look And Feel");
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        }
        catch(Exception e)
        {
            logger.log(Level.WARNING,"Can't Set SystemLookAndFeel", e);
        }

        new BelgianEidViewer().start();
    }

    public void showLog(boolean show)
    {
        if(show)
        {
            if(logPanel==null)
            {
                logPanel=new LogPanel();
                logPanel.start();
                tabPanel.add(logPanel, bundle.getString("LOG"));
                tabPanel.insertTab(bundle.getString("LOG"),null,logPanel,bundle.getString("LOG"),4);
            }
        }
        else
        {
            if(logPanel!=null)
            {
                logPanel.stop();
                tabPanel.remove(logPanel);
                logPanel=null;
            }
        }
    }

    public void showDiagnostics(boolean show)
    {
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
