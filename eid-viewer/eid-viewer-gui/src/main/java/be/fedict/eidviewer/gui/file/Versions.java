/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package be.fedict.eidviewer.gui.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Frank Marien
 */
public class Versions
{

    private static final Logger logger = Logger.getLogger(Versions.class.getName());

    public static int getXMLFileVersion(File file)
    {
        int             version =0;
        FileInputStream fis     =null;

        try
        {
            if (!file.canRead())
                return 0;

            if ((!file.getName().endsWith(".xml")) && (!file.getName().endsWith(".XML")))
                return 0;

            byte[] buffer = new byte[512];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);
            if (headStr.contains("<eid>"))
            {
                version = 4;
                logger.finest("Found Version 4.x XML file");
            }
            else if (headStr.contains("<BelPicDirectory>"))
            {
                version = 3;
                logger.finest("Found Version 3.x.x XML file");
            }
            return version;
        }
        catch (IOException ex)
        {
            Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
        }        
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        return version;
    }

    public static int getCSVFileVersion(File file)
    {
        int             version = 0;
        FileInputStream fis     = null;
       
        try
        { 
            if (!file.canRead())
                return 0;

            if ((!file.getName().endsWith(".csv")) && (!file.getName().endsWith(".CSV")))
                return 0;

            byte[] buffer = new byte[16];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);
            String[] fields = headStr.split(";");
            if (fields.length >= 2 && fields[1].equalsIgnoreCase("eid"))
            {
                try
                {
                    version = Integer.parseInt(fields[0]);
                }
                catch (NumberFormatException nfe)
                {
                    logger.log(Level.FINE, "CSV File Failed To Parse Version", nfe);
                }
            }
            return version;
        }
        catch (IOException ex)
        {
            Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
        }
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        return version;
    }

    public static boolean isTLVEidFile(File file)
    {
        FileInputStream fis = null;
        boolean isTLVEid = false;

        if (!file.canRead())
            return false;

        if ((!file.getName().endsWith(".eid")) && (!file.getName().endsWith(".EID")))
            return false;

        try
        {
            byte[] buffer = new byte[128];
            fis = new FileInputStream(file);
            fis.read(buffer);
            fis.close();
            String headStr = new String(buffer, "utf-8");
            System.err.println(headStr);

            isTLVEid = (buffer[0] == 0 && buffer[1] == 1 && headStr != null && headStr.length() > 0 && headStr.contains("Belgium Root CA"));
        }
        catch (IOException ex)
        {
            Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
        }
        finally
        {
            try
            {
                if(fis!=null)
                    fis.close();
            }
            catch (IOException ex)
            {
                Logger.getLogger(Versions.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        return isTLVEid;
    }
}
