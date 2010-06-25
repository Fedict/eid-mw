package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Point;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JOptionPane;
import javax.swing.JRootPane;
import javax.swing.JTree;
import javax.swing.LookAndFeel;
import javax.swing.SwingUtilities;
import javax.swing.UIDefaults;
import javax.swing.UIManager;
import javax.swing.border.CompoundBorder;

import com.sun.java.swing.plaf.windows.WindowsClassicLookAndFeel;
import com.sun.java.swing.plaf.windows.WindowsLookAndFeel;

import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * This class is a place for all kind of handy utility methods used within this gui project.
 * 
 * @author Rogier Taal
 * 
 */
public class Toolkit {
	
	public static final boolean isWindows = System.getProperty("os.name").startsWith("Windows");
	
	public static Map<String, ImageIcon> icons =  new HashMap<String, ImageIcon>();
	static
	{	
		icons.put("ICO_CARD_EID_PLAIN_16x16", new ImageIcon(Main.class.getResource("/icons/ICO_CARD_EID_PLAIN_16x16.png")));
		icons.put("SELECTED_JTREE_LEAF_16x13", new ImageIcon(Main.class.getResource("/icons/SELECTED_JTREE_LEAF_16x13.gif")));
		icons.put("DEFAULT_JTREE_LEAF_16x13", new ImageIcon(Main.class.getResource("/icons/DEFAULT_JTREE_LEAF_16x13.gif")));
		icons.put("ManageAPI_16x16", new ImageIcon(Main.class.getResource("/icons/ManageAPI_16x16.png")));
		icons.put("FEDICT_ABOUT_LOGO", new ImageIcon(Main.class.getResource("/icons/logo_fedict.gif")));	
	}
	
	public static void copyFile(File from, File to) throws IOException, FileNotFoundException {
		//There is no reliable platform independent way to move a file in java
		//therefore copy temporary file to controlFile using input/output stream
	    DataInputStream in = new DataInputStream(new BufferedInputStream(new FileInputStream(from)));
	    OutputStream fos = new FileOutputStream(to);
	    byte[] buf = new byte[400];
	    int readCnt =0; 
		while((readCnt = in.read(buf)) != -1)
			fos.write(buf, 0, readCnt);
		fos.close();
	}
	
	public static boolean arrayEquals(String[] a1, String[] a2, boolean ignoreCase) {
		if(!ignoreCase)
			return Arrays.equals(a1, a2);
		
		if(a1 == null && a2 != null)
			return false;
		if(a1 != null && a2 == null)
			return false;
		if(a1 != null && a2 != null) {
			if(a1.length != a2.length)
				return false;
			for(int i=0; i<a1.length; i++) {
				if(!a1[i].equalsIgnoreCase(a2[i]))
					return false;
			}
		}
		return true;
	}

	/** Sets cursor for specified component to Wait cursor */
	public static void startWaitCursor(Component c) {
	    JRootPane root = SwingUtilities.getRootPane(c);
	    root.getGlassPane().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	    root.getGlassPane().setVisible(true);
	}

	  /** Sets cursor for specified component to normal cursor */
	public static void stopWaitCursor(Component c) {
		JRootPane root = SwingUtilities.getRootPane(c);
	    root.getGlassPane().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
	    root.getGlassPane().setVisible(false);
	}
	
	public static void showErrorDialog(Component parent, Exception e) {
			if (Log.logger != null)
				Log.logger.error("", e);
		String msg = (e.getCause() != null ? e.getCause().getMessage() : e.getMessage());
		if(msg == null || msg.length() == 0) {
			if(e instanceof RuntimeException) {
				msg = "An internal error occurred: " + e;
			} else {
				msg = "An unexpected error occurred: " + e;
			}
		}
		showErrorDialog(parent, msg);
	}
	
	public static void showErrorDialog(final Component parent, final String msg) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				JOptionPane.showMessageDialog(parent, msg, "Error", JOptionPane.ERROR_MESSAGE);						
			}					
		});
	}
	
	public static Dimension toDimension(String d) {
		int w = 0;
		int h = 0;
		if(d != null) {
			int offset = d.indexOf(',');
			if(offset != -1) {
				try {
					w = Integer.parseInt(d.substring(0, offset));
					h = Integer.parseInt(d.substring(offset+1));
				} catch(NumberFormatException nfe) {						
				}					
			}
		}
		return new Dimension(w,h);
	}
	
	public static Point toPoint(String p) {
		int x = 0;
		int y = 0;
		if(p != null) {
			int offset = p.indexOf(',');
			if(offset != -1) {
				try {
					x = Integer.parseInt(p.substring(0, offset));
					y = Integer.parseInt(p.substring(offset+1));
				} catch(NumberFormatException nfe) {						
				}					
			}
		}
		return new Point(x,y);
	}
	
	public static String toString(Point p) {
		return p.x+","+p.y;
	}
	
	public static String toString(Dimension d) {
		return d.width+","+d.height;
	}
	
	/**
	 * Centers the component <CODE>c</CODE> on component <CODE>p</CODE>.  
	 * If <CODE>p</CODE> is <CODE>null</CODE>, the component <CODE>c</CODE> 
	 * will be centered on the screen.  
	 * 
	 * @param  c  the component to center
	 * @param  p  the parent component to center on or null for screen
	 * @see  #centerComponent(Component)
	 */
	public static void centerComponent(Component c, Component p) {
		if(c == null) {
			return;
		}
		Dimension d = (p != null ? p.getSize() : 
			java.awt.Toolkit.getDefaultToolkit().getScreenSize()
		);
		c.setLocation(
			Math.max(0, (d.getSize().width/2)  - (c.getSize().width/2)), 
			Math.max(0, (d.getSize().height/2) - (c.getSize().height/2))
		);
	}

	public static void expandAll(JTree tree) {
		int row = 0;
		while (row < tree.getRowCount()) {
			tree.expandRow(row++);
		}
	}
	
	public static boolean fileExists(String file) {
		return (file != null && new File(file).exists());
	}
	
	@SuppressWarnings("unchecked")
	public static void fixWindowsClassicLAF() {
		LookAndFeel laf = UIManager.getLookAndFeel();
        boolean themeActive =
            Boolean.TRUE.equals(java.awt.Toolkit.getDefaultToolkit().getDesktopProperty("win.xpstyle.themeActive"));
        boolean noxp = System.getProperty("swing.noxp") != null;
        boolean isClassic = (laf instanceof WindowsClassicLookAndFeel
        		|| (laf instanceof WindowsLookAndFeel && (!themeActive || noxp)));
        
        if(isClassic) {
        	UIDefaults uiDefaults = UIManager.getDefaults();
	    	uiDefaults.put("TextField.border", new CompoundBorder(
		    		BorderFactory.createLineBorder(Color.GRAY, 1),
		    		BorderFactory.createEmptyBorder(2,2,2,2)));
	    	uiDefaults.put("ScrollPane.border", BorderFactory.createLineBorder(Color.GRAY, 1));
		    
	    	Map<String, Object> reps = new HashMap<String,Object>();
	    	Enumeration keynum = uiDefaults.keys();		    	
	    	while(keynum.hasMoreElements()) {
				Object objKey = keynum.nextElement();
				if(objKey instanceof String) {
					String key = ((String)objKey);
					String replaceWithKey = null;
					if(key.endsWith("darkShadow"))
						replaceWithKey = key.substring(0, key.length()-10)+"background";
					else if(key.endsWith("highlight"))
						replaceWithKey = key.substring(0, key.length()-9)+"shadow";												
					if(replaceWithKey != null) {
						Object replacementVal = uiDefaults.get(replaceWithKey);
						if(replacementVal != null)
							reps.put(key, replacementVal);	
					}
				}
	    	}
	    	UIManager.getDefaults().putAll(reps);
        }
	}


}
