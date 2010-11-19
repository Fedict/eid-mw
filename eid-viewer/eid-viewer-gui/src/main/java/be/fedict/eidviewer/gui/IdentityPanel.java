/*
 * IdentityPanel.java
 *
 * Created on Nov 18, 2010, 3:22:56 PM
 */
package be.fedict.eidviewer.gui;
import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import java.awt.Image;
import java.awt.Toolkit;
import java.net.URL;
import java.text.DateFormat;
import java.util.Locale;
import java.util.Observable;
import java.util.Observer;
import javax.swing.ImageIcon;

/**
 *
 * @author frank
 */
public class IdentityPanel extends javax.swing.JPanel implements Observer
{
    private static final String UNKNOWN_VALUE_TEXT="-";

    private DateFormat      mDateFormat;
    private ImageIcon       largeBusyIcon;

    public IdentityPanel()
    {
        initComponents();
        initIcons();
        mDateFormat = DateFormat.getDateInstance(DateFormat.DEFAULT, Locale.getDefault());

    }

    public void update(Observable o, Object o1)
    {
        EidController controller=(EidController)o;

        System.err.println("STATE [" + controller.getState().getState() +"]");

        if(controller.getState()==EidController.STATE.EID_PRESENT)
        {
            if(controller.hasIdentity())
                fillIdentity(controller.getIdentity(),false);
            else
               fillIdentity(null,true);

            if(controller.hasAddress())
                fillAddress(controller.getAddress(),false);
            else
                fillAddress(null,true);

            if(controller.hasPhoto())
                fillPhoto(controller.getPhoto(),false);
            else
                fillPhoto(null,true);

        }
        else
        {
            fillIdentity(null,false);
            fillAddress(null,false);
            fillPhoto(null,false);
        }
    }

    private void fillIdentity(final Identity identity, final boolean loading)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {

            public void run()
            {
                identityBusyIcon.setVisible(loading);
                if(identity!=null)
                {  
                    name.setText(identity.getName());                                               nameLabel.setEnabled(true);
                    givenNames.setText(identity.getFirstName() + " " + identity.getMiddleName());   givenNamesLabel.setEnabled(true);
                    placeOfBirth.setText(identity.getPlaceOfBirth());                               placeOfBirthLabel.setEnabled(true);
                    dateOfBirth.setText(mDateFormat.format(identity.getDateOfBirth().getTime()));   dateOfBirthlabel.setEnabled(true);
                    sex.setText(identity.getGender().name());                                       sexLabel.setEnabled(true);
                    nationality.setText(identity.getNationality());                                 nationalitylabel.setEnabled(true);
                    nationalNumber.setText(identity.getNationalNumber());                           nationalNumberLabel.setEnabled(true);
                    title.setText(identity.getNobleCondition());                                    titleLabel.setEnabled(true);
                    specialStatus.setText(identity.getSpecialStatus().name());                      specialStatusLabel.setEnabled(true);
                }
                else
                {
                    name.setText          (UNKNOWN_VALUE_TEXT); nameLabel.setEnabled(false);
                    givenNames.setText    (UNKNOWN_VALUE_TEXT); givenNamesLabel.setEnabled(false);
                    placeOfBirth.setText  (UNKNOWN_VALUE_TEXT); placeOfBirthLabel.setEnabled(false);
                    dateOfBirth.setText   (UNKNOWN_VALUE_TEXT); dateOfBirthlabel.setEnabled(false);
                    sex.setText           (UNKNOWN_VALUE_TEXT); sexLabel.setEnabled(false);
                    nationality.setText   (UNKNOWN_VALUE_TEXT); nationalitylabel.setEnabled(false);
                    nationalNumber.setText(UNKNOWN_VALUE_TEXT); nationalNumberLabel.setEnabled(false);
                    title.setText         (UNKNOWN_VALUE_TEXT); titleLabel.setEnabled(false);
                    specialStatus.setText (UNKNOWN_VALUE_TEXT); specialStatusLabel.setEnabled(false);

                }
            }
        });
    }

    private void fillAddress(final Address address, final boolean loading)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                addressBusyIcon.setVisible(loading);
                if(address!=null)
                {
                    street.setText(address.getStreetAndNumber());       streetLabel.setEnabled(true);
                    postalCode.setText(address.getZip());               postalCodeLabel.setEnabled(true);
                    municipality.setText(address.getMunicipality());    municipalityLabel.setEnabled(true);
                }
                else
                {
                    street.setText        (UNKNOWN_VALUE_TEXT);         streetLabel.setEnabled(false);
                    postalCode.setText       (UNKNOWN_VALUE_TEXT);      postalCodeLabel.setEnabled(false);
                    municipality.setText  (UNKNOWN_VALUE_TEXT);         municipalityLabel.setEnabled(false);

                }
            }
        });
    }

    private void fillPhoto(final Image image, final boolean loading)
    {
        java.awt.EventQueue.invokeLater(new Runnable()
        {
            public void run()
            {
                if(image!=null)
                    photo.setIcon(new ImageIcon(image));
                else
                    photo.setIcon(loading?largeBusyIcon:null);
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

        photo = new javax.swing.JLabel();
        nameLabel = new javax.swing.JLabel();
        givenNamesLabel = new javax.swing.JLabel();
        placeOfBirthLabel = new javax.swing.JLabel();
        dateOfBirthlabel = new javax.swing.JLabel();
        nationalitylabel = new javax.swing.JLabel();
        nationalNumberLabel = new javax.swing.JLabel();
        sexLabel = new javax.swing.JLabel();
        titleLabel = new javax.swing.JLabel();
        specialStatusLabel = new javax.swing.JLabel();
        streetLabel = new javax.swing.JLabel();
        postalCodeLabel = new javax.swing.JLabel();
        municipalityLabel = new javax.swing.JLabel();
        idAddressSeparator = new javax.swing.JSeparator();
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
        addressBusyIcon = new javax.swing.JLabel();
        identityBusyIcon = new javax.swing.JLabel();
        spacer = new javax.swing.JLabel();

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(204, 255, 204), 24, true));
        setLayout(new java.awt.GridBagLayout());

        photo.setBackground(new java.awt.Color(255, 255, 255));
        photo.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        java.util.ResourceBundle bundle = java.util.ResourceBundle.getBundle("be/fedict/eidviewer/gui/resources/IdentityPanel"); // NOI18N
        photo.setText(bundle.getString("IdentityPanel.photo.text")); // NOI18N
        photo.setMaximumSize(new java.awt.Dimension(140, 200));
        photo.setMinimumSize(new java.awt.Dimension(140, 200));
        photo.setName("photo"); // NOI18N
        photo.setOpaque(true);
        photo.setPreferredSize(new java.awt.Dimension(140, 200));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 13;
        gridBagConstraints.insets = new java.awt.Insets(23, 23, 23, 31);
        add(photo, gridBagConstraints);

        nameLabel.setText(bundle.getString("IdentityPanel.nameLabel.text")); // NOI18N
        nameLabel.setName("nameLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(nameLabel, gridBagConstraints);

        givenNamesLabel.setText(bundle.getString("IdentityPanel.givenNamesLabel.text")); // NOI18N
        givenNamesLabel.setName("givenNamesLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(givenNamesLabel, gridBagConstraints);

        placeOfBirthLabel.setText(bundle.getString("IdentityPanel.placeOfBirthLabel.text")); // NOI18N
        placeOfBirthLabel.setName("placeOfBirthLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(placeOfBirthLabel, gridBagConstraints);

        dateOfBirthlabel.setText(bundle.getString("IdentityPanel.dateOfBirthlabel.text")); // NOI18N
        dateOfBirthlabel.setName("dateOfBirthlabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(dateOfBirthlabel, gridBagConstraints);

        nationalitylabel.setText(bundle.getString("IdentityPanel.nationalitylabel.text")); // NOI18N
        nationalitylabel.setName("nationalitylabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(nationalitylabel, gridBagConstraints);

        nationalNumberLabel.setText(bundle.getString("IdentityPanel.nationalNumberLabel.text")); // NOI18N
        nationalNumberLabel.setName("nationalNumberLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(nationalNumberLabel, gridBagConstraints);

        sexLabel.setText(bundle.getString("IdentityPanel.sexLabel.text")); // NOI18N
        sexLabel.setName("sexLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(sexLabel, gridBagConstraints);

        titleLabel.setText(bundle.getString("IdentityPanel.titleLabel.text")); // NOI18N
        titleLabel.setName("titleLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(titleLabel, gridBagConstraints);

        specialStatusLabel.setText(bundle.getString("IdentityPanel.specialStatusLabel.text")); // NOI18N
        specialStatusLabel.setName("specialStatusLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(specialStatusLabel, gridBagConstraints);

        streetLabel.setText(bundle.getString("IdentityPanel.streetLabel.text")); // NOI18N
        streetLabel.setName("streetLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 10;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(streetLabel, gridBagConstraints);

        postalCodeLabel.setText(bundle.getString("IdentityPanel.postalCodeLabel.text")); // NOI18N
        postalCodeLabel.setName("postalCodeLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 11;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(postalCodeLabel, gridBagConstraints);

        municipalityLabel.setText(bundle.getString("IdentityPanel.municipalityLabel.text")); // NOI18N
        municipalityLabel.setName("municipalityLabel"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 12;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(municipalityLabel, gridBagConstraints);

        idAddressSeparator.setName("idAddressSeparator"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new java.awt.Insets(7, 0, 7, 0);
        add(idAddressSeparator, gridBagConstraints);

        name.setText(bundle.getString("IdentityPanel.name.text")); // NOI18N
        name.setName("name"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(name, gridBagConstraints);

        givenNames.setText(bundle.getString("IdentityPanel.givenNames.text")); // NOI18N
        givenNames.setName("givenNames"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(givenNames, gridBagConstraints);

        placeOfBirth.setText(bundle.getString("IdentityPanel.placeOfBirth.text")); // NOI18N
        placeOfBirth.setName("placeOfBirth"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(placeOfBirth, gridBagConstraints);

        dateOfBirth.setText(bundle.getString("IdentityPanel.dateOfBirth.text")); // NOI18N
        dateOfBirth.setName("dateOfBirth"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(dateOfBirth, gridBagConstraints);

        sex.setText(bundle.getString("IdentityPanel.sex.text")); // NOI18N
        sex.setName("sex"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(sex, gridBagConstraints);

        nationalNumber.setText(bundle.getString("IdentityPanel.nationalNumber.text")); // NOI18N
        nationalNumber.setName("nationalNumber"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(nationalNumber, gridBagConstraints);

        nationality.setText(bundle.getString("IdentityPanel.nationality.text")); // NOI18N
        nationality.setName("nationality"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(nationality, gridBagConstraints);

        title.setText(bundle.getString("IdentityPanel.title.text")); // NOI18N
        title.setName("title"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(title, gridBagConstraints);

        specialStatus.setText(bundle.getString("IdentityPanel.specialStatus.text")); // NOI18N
        specialStatus.setName("specialStatus"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(specialStatus, gridBagConstraints);

        street.setText(bundle.getString("IdentityPanel.street.text")); // NOI18N
        street.setName("street"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 10;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(street, gridBagConstraints);

        postalCode.setText(bundle.getString("IdentityPanel.postalCode.text")); // NOI18N
        postalCode.setName("postalCode"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 11;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(postalCode, gridBagConstraints);

        municipality.setText(bundle.getString("IdentityPanel.municipality.text")); // NOI18N
        municipality.setName("municipality"); // NOI18N
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 12;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        add(municipality, gridBagConstraints);

        addressBusyIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/busyicons/busy_anim_small.gif"))); // NOI18N
        addressBusyIcon.setText(bundle.getString("IdentityPanel.identityBusyIcon.text")); // NOI18N
        addressBusyIcon.setMaximumSize(new java.awt.Dimension(16, 16));
        addressBusyIcon.setMinimumSize(new java.awt.Dimension(16, 16));
        addressBusyIcon.setName("identityBusyIcon"); // NOI18N
        addressBusyIcon.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 11;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(addressBusyIcon, gridBagConstraints);

        identityBusyIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/be/fedict/eidviewer/gui/resources/busyicons/busy_anim_small.gif"))); // NOI18N
        identityBusyIcon.setText(bundle.getString("IdentityPanel.identityBusyIcon.text")); // NOI18N
        identityBusyIcon.setMaximumSize(new java.awt.Dimension(16, 16));
        identityBusyIcon.setMinimumSize(new java.awt.Dimension(16, 16));
        identityBusyIcon.setName("identityBusyIcon"); // NOI18N
        identityBusyIcon.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(identityBusyIcon, gridBagConstraints);

        spacer.setText(bundle.getString("IdentityPanel.spacer.text")); // NOI18N
        spacer.setMaximumSize(new java.awt.Dimension(16, 16));
        spacer.setMinimumSize(new java.awt.Dimension(16, 16));
        spacer.setName("spacer"); // NOI18N
        spacer.setPreferredSize(new java.awt.Dimension(16, 16));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        add(spacer, gridBagConstraints);
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel addressBusyIcon;
    private javax.swing.JLabel dateOfBirth;
    private javax.swing.JLabel dateOfBirthlabel;
    private javax.swing.JLabel givenNames;
    private javax.swing.JLabel givenNamesLabel;
    private javax.swing.JSeparator idAddressSeparator;
    private javax.swing.JLabel identityBusyIcon;
    private javax.swing.JLabel municipality;
    private javax.swing.JLabel municipalityLabel;
    private javax.swing.JLabel name;
    private javax.swing.JLabel nameLabel;
    private javax.swing.JLabel nationalNumber;
    private javax.swing.JLabel nationalNumberLabel;
    private javax.swing.JLabel nationality;
    private javax.swing.JLabel nationalitylabel;
    private javax.swing.JLabel photo;
    private javax.swing.JLabel placeOfBirth;
    private javax.swing.JLabel placeOfBirthLabel;
    private javax.swing.JLabel postalCode;
    private javax.swing.JLabel postalCodeLabel;
    private javax.swing.JLabel sex;
    private javax.swing.JLabel sexLabel;
    private javax.swing.JLabel spacer;
    private javax.swing.JLabel specialStatus;
    private javax.swing.JLabel specialStatusLabel;
    private javax.swing.JLabel street;
    private javax.swing.JLabel streetLabel;
    private javax.swing.JLabel title;
    private javax.swing.JLabel titleLabel;
    // End of variables declaration//GEN-END:variables

    private void initIcons()
    {
         URL url=BelgianEidViewer.class.getResource("resources/busyicons/busy_anim_large.gif");
         if(url!=null)
         largeBusyIcon=new ImageIcon(Toolkit.getDefaultToolkit().getImage(url));
    }
}

