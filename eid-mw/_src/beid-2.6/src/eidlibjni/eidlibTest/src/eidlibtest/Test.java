package eidlibtest;

/**
 * <p>Title: eidlibTest</p>
 * <p>Description: Test eidlib jni interface</p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: Zetes</p>
 * @author not attributable
 * @version 1.0
 */
import be.belgium.eid.*;
import java.io.*;

public class Test
{
  protected final static String[] hexChars = {"0", "1", "2", "3", "4", "5", "6", "7",
                                            "8", "9", "A", "B", "C", "D", "E", "F"};

  static
  {
    try
    {
        System.loadLibrary("beidlibjni");
    }
    catch (UnsatisfiedLinkError e)
    {
      System.err.println("Native code library failed to load.\n" + e);
      System.exit(1);
    }
  }

  public Test()
  {
  }

  public static String hexify(byte[] data)
  {
      if(data == null) return "null";

      StringBuffer out = new StringBuffer(256);
      int n = 0;

      for(int i=0; i<data.length; i++) {
        if(n>0) out.append(' ');

        out.append(hexChars[(data[i]>>4) & 0x0f]);
        out.append(hexChars[data[i] & 0x0f]);

        if(++n == 16) {
              out.append('\n');
              n = 0;
        }
      }

      return out.toString();
}

public static String hexifyShort(int val)
{
    return hexChars[((val & 0xffff) & 0xf000)>>>12] +
      hexChars[((val & 0xfff) & 0xf00)>>>8] +
      hexChars[((val & 0xff) & 0xf0)>>>4] + hexChars[val & 0x0f];
}

  public void PrintStatus( String title, BEID_Status tStatus )
  {
    System.out.println(" === " + title + " ===");

    if ( 0 == tStatus.getGeneral()) return;

    System.out.println(" *** Return codes: General=" + tStatus.getGeneral() +
                       " System=" + tStatus.getSystem() + " PC/SC=" + tStatus.getPcsc() +
                       " Card SW=" + hexify(tStatus.getCardSW()));
  }

  public void PrintIDData(BEID_ID_Data IDData)
  {
    System.out.println("Version : " + IDData.getVersion());
    System.out.println("Card Number : " + IDData.getCardNumber());
    System.out.println("Chip Number : " + IDData.getChipNumber());
    System.out.println("Validity : from " + IDData.getValidityDateBegin() + " until " + IDData.getValidityDateEnd());
    System.out.println("Delivery Municipality : " + IDData.getMunicipality());
    System.out.println("National Number :" + IDData.getNationalNumber());
    System.out.println("Name : " + IDData.getName());
    System.out.println("First name 1 : " + IDData.getFirstName1());
    System.out.println("First name 2 : " + IDData.getFirstName2());
    System.out.println("First name 3 : " + IDData.getFirstName3());
    System.out.println("Nationality : " + IDData.getNationality());
    System.out.println("Birthplace : " + IDData.getBirthLocation());
    System.out.println("Birthdate : " + IDData.getBirthDate());
    System.out.println("Gender : " + IDData.getSex());
    System.out.println("Noble Condition : " + IDData.getNobleCondition());
    System.out.println("Document Type : " + IDData.getDocumentType());
    System.out.println("Special Status: Whitecane : " + IDData.getWhiteCane() + " Yellowcane : " + IDData.getYellowCane() + " Extendedminority : " +
                       IDData.getExtendedMinority());
    System.out.println("");
  }

  public void PrintVersionInfo(BEID_VersionInfo oVersion)
  {
    System.out.println("Serial Number : " + hexify(oVersion.getSerialNumber()));
    System.out.println("ComponentCode : " + hexifyShort(oVersion.getComponentCode()));
    System.out.println("OSNumber : " + hexifyShort(oVersion.getOSNumber()));
    System.out.println("OSVersion : " + hexifyShort(oVersion.getOSVersion()));
    System.out.println("SoftmaskNumber : "+ hexifyShort(oVersion.getSoftmaskNumber()));
    System.out.println("SoftmaskVersion : " + hexifyShort(oVersion.getSoftmaskVersion()));
    System.out.println("AppletVersion : " + hexifyShort(oVersion.getAppletVersion()));
    System.out.println("GlobalOSVersion : " + hexifyShort(oVersion.getGlobalOSVersion()));
    System.out.println("AppletInterfaceVersion : " + hexifyShort(oVersion.getAppletInterfaceVersion()));
    System.out.println("PKCS1Support : " + hexifyShort(oVersion.getPKCS1Support()));
    System.out.println("KeyExchangeVersion : " + hexifyShort(oVersion.getKeyExchangeVersion()));
    System.out.println("ApplicationLifeCycle : " + hexifyShort(oVersion.getApplicationLifeCycle()));
    System.out.println("GraphPerso : " + hexifyShort(oVersion.getGraphPerso()));
    System.out.println("ElecPerso : " + hexifyShort(oVersion.getElecPerso()));
    System.out.println("ElecPersoInterface : " + hexifyShort(oVersion.getElecPersoInterface()));
  }

String VerifyCertErrorString(long n)
{
      switch ((int)n)
      {
      case 0:
              return("Valid");
      case 1:
              return("Not validated");
      case 2:
              return("Unable to get issuer certificate");
      case 3:
              return("Unable to get certificate CRL");
      case 4:
              return("Unable to decrypt certificate's signature");
      case 5:
              return("Unable to decrypt CRL's signature");
      case 6:
              return("Unable to decode issuer public key");
      case 7:
              return("Certificate signature failure");
      case 8:
              return("CRL signature failure");
      case 9:
              return("Certificate is not yet valid");
      case 10:
              return("CRL is not yet valid");
      case 11:
              return("Certificate has expired");
      case 12:
              return("CRL has expired");
      case 13:
              return("Format error in certificate's notBefore field");
      case 14:
              return("Format error in certificate's notAfter field");
      case 15:
              return("Format error in CRL's lastUpdate field");
      case 16:
              return("Format error in CRL's nextUpdate field");
      case 17:
              return("Out of memory");
      case 18:
              return("Self signed certificate");
      case 19:
              return("Self signed certificate in certificate chain");
      case 20:
              return("Unable to get local issuer certificate");
      case 21:
              return("Unable to verify the first certificate");
      case 22:
              return("Certificate chain too long");
      case 23:
              return("Certificate revoked");
      case 24:
              return ("Invalid CA certificate");
      case 25:
              return ("Path length constraint exceeded");
      case 26:
              return ("Unsupported certificate purpose");
      case 27:
              return ("Certificate not trusted");
      case 28:
              return ("Certificate rejected");
      case 29:
              return("Subject issuer mismatch");
      case 30:
              return("Authority and subject key identifier mismatch");
      case 31:
              return("Authority and issuer serial number mismatch");
      case 32:
              return("Key usage does not include certificate signing");
      case 33:
              return("Unable to get CRL issuer certificate");
      case 34:
              return("Unhandled critical extension");

      default:
      return "Unknown status";
    }
}

  void PrintCertifCheck( BEID_Certif certif )
  {
      System.out.println("*** Certificate " + certif.getCertifLabel() +
                         " Status : " + certif.getCertifStatus() + "(" + VerifyCertErrorString(certif.getCertifStatus()) + ")");
  }

  public void PrintSignCheck(BEID_Certif_Check CertifCheck)
  {
    String resultStr[] = { "System error",
                                     "Valid",
                                     "Invalid",
                                     "Valid & wrong RRN certificate",
                                     "Invalid & wrong RRN certificate"
                         };
    String resultStrPol[] = { "None", "OCSP", "CRL", "Both"};


    System.out.println(" *** Signature result : " + CertifCheck.getSignatureCheck() + " " +
                        resultStr[CertifCheck.getSignatureCheck() + 1] );

    System.out.println(" *** Certificate checking used policy : " + CertifCheck.getUsedPolicy() + " " +
                       resultStrPol[CertifCheck.getUsedPolicy()]);
    for ( int i = 0; i < CertifCheck.getCertificatesLength(); i++ ) PrintCertifCheck( CertifCheck.getCertificate(i) );
    System.out.println("");
  }

  public void PrintAddressData(BEID_Address adData)
  {
    System.out.println("Version : " + adData.getVersion());
    System.out.println("Street : " + adData.getStreet());
    System.out.println("Number : " + adData.getStreetNumber());
    System.out.println("Box : " + adData.getBoxNumber());
    System.out.println("Zip : " + adData.getZip());
    System.out.println("Municipality : " + adData.getMunicipality());
    System.out.println("Country :" + adData.getCountry());
    System.out.println("");
  }

  public static void main(String[] args)
  {
    Test test = new Test();

    BEID_Status oStatus;
    BEID_ID_Data IDData = new BEID_ID_Data();
    BEID_Certif_Check CertCheck = new BEID_Certif_Check();
    BEID_Long CardHandle = new BEID_Long();

    oStatus = eidlib.BEID_Init(null, 0, 0, CardHandle);

    if(0 != oStatus.getGeneral())
      return;

  ////////////// High Level Test //////////////////////
  // Read ID Data
    oStatus = eidlib.BEID_GetID(IDData, CertCheck);
    test.PrintStatus( "Get Identity Data", oStatus );
    if(0 == oStatus.getGeneral())
    {
        test.PrintIDData(IDData);
        test.PrintSignCheck(CertCheck);
    }

   // Read Address
   BEID_Address adData = new BEID_Address();
   oStatus = eidlib.BEID_GetAddress(adData, CertCheck);
   test.PrintStatus( "Get Address", oStatus );
   if(0 == oStatus.getGeneral())
   {
       test.PrintAddressData(adData);
   }

   // Read Picture Data
   BEID_Bytes Picture = new BEID_Bytes();
   oStatus = eidlib.BEID_GetPicture(Picture, CertCheck);
   test.PrintStatus( "Get Picture", oStatus );
   if(0 == oStatus.getGeneral())
   {
     try
     {
       String photo = "photo.jpg";
       FileOutputStream oFile = new FileOutputStream(photo);
       oFile.write(Picture.getData());
       oFile.close();
       System.out.println("Created " + photo);
       // windows-only ...
       //Process p = Runtime.getRuntime().exec("rundll32 SHELL32.DLL,ShellExec_RunDLL photo.jpg");
       //p.waitFor();
       System.out.println("Done.");
     }
     catch (FileNotFoundException excep)
     {
       System.out.println(excep.getMessage());
     }
     catch (IOException excepIO)
     {
       System.out.println(excepIO.getMessage());
     }
     catch(Exception e)
     {
       e.printStackTrace();
     }
   }

////////////// Mid Level Test //////////////////////

    // Verify Pin
    // null means:  Pinpad -> input on pinpad or No pinpad -> dialogbox popup
    //no null means : Pinpad->input on pinpad or No pinpad -> no input asked
   BEID_Pin PinData = new BEID_Pin();
   PinData.setPinType(0);
   PinData.setId((short)1);
   PinData.setUsageCode(1);
   BEID_Long Triesleft = new BEID_Long();
   oStatus = eidlib.BEID_VerifyPIN(PinData, null, Triesleft);

   // Change Pin
   //oStatus = eidlib.BEID_ChangePIN(PinData, null, null, Triesleft);

    // Read RN Certificate
    byte[] AID = {(byte)0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};
    byte[] FileID = {0x50, 0x3C};
    oStatus = eidlib.BEID_SelectApplication(AID);
    test.PrintStatus( "Select Application", oStatus);
    BEID_Bytes BytesRead = new BEID_Bytes();
    oStatus = eidlib.BEID_ReadFile(FileID, BytesRead, PinData);
    test.PrintStatus( "Read File", oStatus );

    // Get version
    BEID_Bytes SignBytes = new BEID_Bytes();
    BEID_VersionInfo oVersion = new BEID_VersionInfo();
    oStatus = eidlib.BEID_GetVersionInfo(oVersion, 0, SignBytes);
    test.PrintStatus( "Get Version", oStatus );
    if(0 == oStatus.getGeneral())
    {
        test.PrintVersionInfo(oVersion);
    }

    // PIN Status
    oStatus = eidlib.BEID_GetPINStatus(PinData, Triesleft, 0, SignBytes);
    test.PrintStatus( "Get PIN status", oStatus );

////////////// Low Level Test //////////////////////

    // Low level test
    oStatus = eidlib.BEID_BeginTransaction();
    if(0 == oStatus.getGeneral())
    {
      // Send select SGNID
      byte[] SendBytes = {0x00, (byte)0xA4, 0x08, 0x0C, 0x06, 0x3F, 0x00, (byte)0xDF, 0x01, 0x40, 0x32};
      BEID_Bytes RespBytes = new BEID_Bytes();
      oStatus = eidlib.BEID_SendAPDU(SendBytes, PinData, RespBytes);
      byte [] Resp = RespBytes.getData();
      oStatus = eidlib.BEID_EndTransaction();
    }
    // FlushCache
    oStatus = eidlib.BEID_FlushCache();

    oStatus = eidlib.BEID_Exit();
  }

}
