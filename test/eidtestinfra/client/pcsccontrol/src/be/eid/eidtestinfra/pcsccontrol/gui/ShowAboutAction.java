package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;
import java.util.jar.Attributes;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;
import javax.swing.UIManager;

/**
 * Upon {@link #actionPerformed(ActionEvent)} a dialog is shown containing a logo,
 * some fields from the jar manifest and the currently running java virtual machine.
 * 
 * @author Rogier Taal
 * 
 */
public class ShowAboutAction extends AbstractAction {
	
	private static final long serialVersionUID = 1L;
	
	public static final String NAME = "ACTION_SHOW_ABOUT";
	
	private Main main;	
	
	ShowAboutAction(Main main) {
		super(NAME);
		this.main = main;
	}

	public void actionPerformed(ActionEvent arg0) {
		String msg = null;
		if(main.getManifest() != null) {
			Attributes atts = main.getManifest().getMainAttributes();
			msg = "<html><body><center>"
				+atts.getValue(Attributes.Name.IMPLEMENTATION_TITLE)
				+"<br>"+atts.getValue(Attributes.Name.IMPLEMENTATION_VENDOR)
				+"<br>Version: "+atts.getValue(Attributes.Name.IMPLEMENTATION_VERSION)				
				+"<br><hr width=20% size=1 NOSHADE>Java Runtime: "+System.getProperty("java.version")+"</center></body></html>";
		}
		
		JOptionPane.showMessageDialog(main.getFrame(), msg, "About",
				JOptionPane.PLAIN_MESSAGE, UIManager.getIcon("FEDICT_ABOUT_LOGO"));
	}
}
