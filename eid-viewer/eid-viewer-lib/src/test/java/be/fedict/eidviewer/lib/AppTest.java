package be.fedict.eidviewer.lib;

import be.fedict.eid.applet.Messages;
import java.util.List;
import java.util.ListIterator;
import java.util.Locale;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

import be.fedict.eid.applet.service.*;
import java.awt.Image;

/**
 * Unit test for simple App.
 */
public class AppTest extends TestCase
{
    public AppTest( String testName )
    {
        super( testName );    
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( AppTest.class );
    }

    /**
     * Rigourous Test :-)
     */
    public void testApp()
    {
        TestView    testView=new TestView();
        Messages    testMessages=new Messages(Locale.ENGLISH);

        Eid eid=EidFactory.getEidImpl(testView,testMessages);
        assertNotNull(eid);

        try
        {
            List<String> readers=eid.getReaderList();
            assertNotNull(readers);
            for(ListIterator<String> i=readers. listIterator();i.hasNext();)
            {
                System.err.println(i.next());
            }
        }
        catch(Exception ex)
        {
            System.err.println("getReaderList Failed");
        }


        try
        {
            if(eid.isEidPresent())
            {
                Identity identity=eid.getIdentity();
                System.err.println(identity.getFirstName());
                System.err.println(identity.getMiddleName());
                System.err.println(identity.getName());
                System.err.println(identity.getDocumentType());

                Address address=eid.getAddress();
                System.err.println(address.getStreetAndNumber());
                System.err.println(address.getZip());
                System.err.println(address.getMunicipality());

                Image photo=eid.getPhotoImage();
                System.err.println(photo);
            }

        }
        catch(Exception ex)
        {
            System.err.println("getReaderList Failed");
        }



    }
}
