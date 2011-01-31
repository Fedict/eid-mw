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

package be.fedict.eidviewer.gui.helper;
import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eidviewer.gui.EidData;
import be.fedict.eidviewer.gui.X509CertificateChainAndTrust;
import be.fedict.eidviewer.gui.file.EidFiles;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import java.awt.*;
import java.beans.*;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.util.ResourceBundle;
import javax.imageio.ImageIO;
import javax.swing.border.LineBorder;

/**
 *
 * @author Frank Marien
 */
public class EidFilePreviewAccessory extends JPanel implements PropertyChangeListener,EidData
{
    private static final Logger logger = Logger.getLogger(EidFilePreviewAccessory.class.getName());
    private JLabel              photoLabel;
    private JLabel              nameLabel;
    private ResourceBundle      bundle;

    public EidFilePreviewAccessory(ResourceBundle bundle)
    { 
        this.bundle=bundle;
        setPreferredSize(new Dimension(320,320));
        setMinimumSize(new Dimension(320,320));
        setBorder(BorderFactory.createCompoundBorder(BorderFactory.createEmptyBorder(0,8,0,0), new LineBorder(new Color(204,255,204),24,true)));

        photoLabel=new JLabel();
        photoLabel.setHorizontalAlignment(SwingConstants.CENTER);
        photoLabel.setVerticalAlignment(SwingConstants.CENTER);
        photoLabel.setBorder(BorderFactory.createEmptyBorder(0,16,0,16));
        nameLabel=new JLabel();
        nameLabel.setHorizontalAlignment(SwingConstants.CENTER);
        nameLabel.setVerticalAlignment(SwingConstants.CENTER);
        nameLabel.setBorder(BorderFactory.createEmptyBorder(0,0,16,0));
        setLayout(new BorderLayout());

        add(photoLabel,BorderLayout.CENTER);
        add(nameLabel,BorderLayout.SOUTH);
        
        clear();
    }

    public void propertyChange(PropertyChangeEvent event)
    {
        if(!event.getPropertyName().equals(JFileChooser.SELECTED_FILE_CHANGED_PROPERTY))
            return;
      
        File selection=(File)event.getNewValue();
        if (selection==null || (!selection.canRead()) || (!selection.isFile()))
        {
            clear();
        }
        else
        {
            try
            {
                EidFiles.loadFromFile(selection, this);
            }
            catch (Exception ex)
            {
                logger.log(Level.SEVERE, "Failed To Load eID File", ex);
            }
       }   
    }
    
    private void clear()
    {
        photoLabel.setIcon(null);
        photoLabel.setText(bundle.getString("previewLabel"));
        photoLabel.setEnabled(false);
        nameLabel.setText("");
        nameLabel.setEnabled(false);
    }

    public EidData setIdentity(Identity identity)
    {
        nameLabel.setText(identity.getFirstName() + " " + identity.getName());
        nameLabel.setEnabled(true);
        return this;
    }
    
    public EidData setPhoto(byte[] photo)
    {
        try
        {
            photoLabel.setIcon(new ImageIcon(ImageIO.read(new ByteArrayInputStream(photo))));
            photoLabel.setEnabled(true);
            photoLabel.setText("");
        }
        catch (IOException ex)
        {
            Logger.getLogger(EidFilePreviewAccessory.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return this;
    }

    // other stuff from the EidFile Interface we don't need here
    public EidData setAddress(Address address)                                  { return this; }
    public EidData setAuthCertChain(X509CertificateChainAndTrust authCertChain) { return this; }
    public EidData setSignCertChain(X509CertificateChainAndTrust signCertChain) { return this; }
    public EidData setRRNCertChain(X509CertificateChainAndTrust rrnCertChain)   { return this; }
    public Identity getIdentity()                                               { return null; }
    public Address getAddress()                                                 { return null; }
    public byte[] getPhoto()                                                    { return null; }
    public X509CertificateChainAndTrust getAuthCertChain()                      { return null; }
    public X509CertificateChainAndTrust getSignCertChain()                      { return null; }
    public X509CertificateChainAndTrust getRRNCertChain()                       { return null; }
    public boolean hasAddress()                                                 { return false; }
    public boolean hasIdentity()                                                { return false; }
    public boolean hasPhoto()                                                   { return false; }
    public boolean hasAuthCertChain()                                           { return false; }
    public boolean hasSignCertChain()                                           { return false; }
    public boolean hasRRNCertChain()                                            { return false; }
  
}
