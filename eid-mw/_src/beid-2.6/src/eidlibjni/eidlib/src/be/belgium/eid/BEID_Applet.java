package be.belgium.eid;

import java.io.*;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * <p>Title: EIDLib</p>
 * <p>Description: EID Middelware Library</p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.1
 */

public class BEID_Applet extends JApplet
{
  protected BEID_ID_Data m_idData = null;
  protected BEID_Address m_addrData = null;
  protected BEID_Raw m_oRaw = null;
  protected String m_strReader = null;
  protected int m_OCSP = 0;
  protected int m_CRL = 0;
  protected boolean m_bWarning = true;
  JButton jButtonRead = new JButton();
  JLabel lblImage = new JLabel();
  Container cont = this.getContentPane();

  public BEID_Applet()
  {
    try
    {
      jbInit();
    }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }

  public void init()
  {
    try
    {
      System.loadLibrary("beidlibjni");
      m_idData = null;
      m_addrData = null;
      m_oRaw = null;
      m_strReader = getParameter("Reader");
      String strOCSP = getParameter("OCSP");
      if (strOCSP != null)
      {
        m_OCSP = Integer.parseInt(strOCSP);
      }
      String strCRL = getParameter("CRL");
      if (strCRL != null)
      {
        m_CRL = Integer.parseInt(strCRL);
      }
      String strWarn = getParameter("DisableWarning");
      if (strWarn != null)
      {
        if(0 == strWarn.compareToIgnoreCase("true"))
        {
          m_bWarning = false;
        }
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

  public void destroy()
  {
    m_idData = null;
    m_addrData = null;
    m_oRaw = null;
  }

  public int InitLib(String strReader)
  {
    m_idData = null;
    m_addrData = null;
    m_oRaw = null;
    lblImage.setIcon(null);
    if (strReader != null)
    {
      m_strReader = strReader;
    }
    BEID_Status oStatus;
    BEID_Long CardHandle = new BEID_Long();
    oStatus = eidlib.BEID_Init(m_strReader, m_OCSP, m_CRL, CardHandle);
    return oStatus.getGeneral();
  }

  public int ExitLib()
  {
    m_idData = null;
    m_addrData = null;
    m_oRaw = null;

    BEID_Status oStatus;
    oStatus = eidlib.BEID_Exit();
    return oStatus.getGeneral();
  }

  protected boolean CheckIDData()
  {
    boolean bRet = true;
    try
    {
      if (m_idData == null)
      {
        BEID_Status oStatus;
        m_idData = new BEID_ID_Data();
        Locale l = java.util.Locale.getDefault();
        String strTemp = getResourceString(l, "ReadID");
        if(!ShowDialog(strTemp))
          return false;
        BEID_Certif_Check CertCheck = new BEID_Certif_Check();
        oStatus = eidlib.BEID_GetID(m_idData, CertCheck);
        if (0 == oStatus.getGeneral())
        {
          return true;
        }
        m_idData = null;
        bRet = false;
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
      bRet = false;
    }
    return bRet;
  }

  protected boolean CheckAddrData()
  {
    boolean bRet = true;
    try
    {
      if (m_addrData == null)
      {
        BEID_Status oStatus;
        m_addrData = new BEID_Address();
        Locale l = java.util.Locale.getDefault();
        String strTemp = getResourceString(l, "ReadAddr");
        if(!ShowDialog(strTemp))
          return false;
        BEID_Certif_Check CertCheck = new BEID_Certif_Check();
        oStatus = eidlib.BEID_GetAddress(m_addrData, CertCheck);
        if (0 == oStatus.getGeneral())
        {
          return true;
        }
        m_addrData = null;
        bRet = false;
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
      bRet = false;
    }

    return bRet;
  }

  protected boolean CheckRawData()
  {
    boolean bRet = true;
    try
    {
      if (m_oRaw == null)
      {
        BEID_Status oStatus;
        m_oRaw = new BEID_Raw();
        Locale l = java.util.Locale.getDefault();
        String strTemp = getResourceString(l, "ReadRaw");
        if(!ShowDialog(strTemp))
          return false;
        oStatus = eidlib.BEID_GetRawData(m_oRaw);
        if (0 == oStatus.getGeneral())
        {
          return true;
        }
        m_oRaw = null;
        bRet = false;
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
      bRet = false;
    }

    return bRet;
  }

protected String getResourceString(Locale l, String Key)
{
 ResourceBundle bundle = null;
 try
 {
   bundle = ResourceBundle.getBundle("be.belgium.eid.LabelsBundle", l);
 }
 catch(MissingResourceException e)
 {
   e.printStackTrace();
 }
 if(bundle == null)
 {
   try
   {
     bundle = ResourceBundle.getBundle("be.belgium.eid.LabelsBundle", Locale.ENGLISH);
   }
   catch(MissingResourceException e)
   {
    e.printStackTrace();
   }
   if(bundle == null)
     return null;
   return (String) bundle.getObject(Key);
 }
 else
 {
   return (String) bundle.getObject(Key);
 }
}
  protected boolean ShowDialog(String strFunction)
  {
    if(!m_bWarning)
      return true;

    try
    {
      JPanel messagePanel = new JPanel();
      messagePanel.setLayout(new BoxLayout(messagePanel, BoxLayout.Y_AXIS));
      String strTemp;
      Locale l = java.util.Locale.getDefault();
      strTemp = getResourceString(l, "AppLabel");
      messagePanel.add(new JLabel(strTemp));
      messagePanel.add(new JLabel("   "));
      strTemp = getResourceString(l, "Function");
      messagePanel.add(new JLabel(strTemp));
      messagePanel.add(new JLabel(strFunction));
      messagePanel.add(new JLabel("   "));
      strTemp = getResourceString(l, "URL");
      messagePanel.add(new JLabel(strTemp));
      messagePanel.add(new JLabel(getDocumentBase().getFile()));
      messagePanel.add(new JLabel("  "));
      strTemp = getResourceString(l, "Accept");
      messagePanel.add(new JLabel(strTemp));
      messagePanel.add(new JLabel("  "));
      JOptionPane pane = new JOptionPane(messagePanel,
                                         JOptionPane.WARNING_MESSAGE,
                                         JOptionPane.YES_NO_OPTION);
      strTemp = getResourceString(l, "Title");
      JDialog dialog = pane.createDialog(this, strTemp);
      dialog.setModal(true);
      pane.setLocale(l);
      centerScreen(dialog);
      dialog.show();
      Object selectedValue = pane.getValue();
      if(selectedValue == null)
        return false;
      if (((Integer)selectedValue).intValue() == JOptionPane.YES_OPTION)
      {
        return true;
      }
    }
    catch(Exception exp)
    {
      exp.printStackTrace();
    }
    return false;
  }

  public void centerScreen(JDialog dialog)
  {
    Dimension dim = getToolkit().getScreenSize();
    //Rectangle abounds = getBounds();
    dialog.setLocation((dim.width - dialog.getWidth()) / 2, (dim.height - dialog.getHeight()) / 2);
    dialog.requestFocus();
  }

  public String getCardNumber()
  {
    if (CheckIDData())
    {
      return m_idData.getCardNumber();
    }
    return "";
  }

  public String getChipNumber()
  {
    if (CheckIDData())
    {
      return m_idData.getChipNumber();
    }
    return "";
  }

  public String getValidityDateBegin()
  {
    if (CheckIDData())
    {
      return m_idData.getValidityDateBegin();
    }
    return "";
  }

  public String getValidityDateEnd()
  {
    if (CheckIDData())
    {
      return m_idData.getValidityDateEnd();
    }
    return "";
  }

  public String getIssMunicipality()
  {
    if (CheckIDData())
    {
      return m_idData.getMunicipality();
    }
    return "";
  }

  public String getNationalNumber()
  {
    if (CheckIDData())
    {
      return m_idData.getNationalNumber();
    }
    return "";
  }

  public String getName()
  {
    if (CheckIDData())
    {
      return m_idData.getName();
    }
    return "";
  }

  public String getFirstName1()
  {
    if (CheckIDData())
    {
      return m_idData.getFirstName1();
    }
    return "";
  }

  public String getFirstName2()
  {
    if (CheckIDData())
    {
      return m_idData.getFirstName2();
    }
    return "";
  }

  public String getFirstName3()
  {
    if (CheckIDData())
    {
      return m_idData.getFirstName3();
    }
    return "";
  }

  public String getNationality()
  {
    if (CheckIDData())
    {
      return m_idData.getNationality();
    }
    return "";
  }

  public String getBirthLocation()
  {
    if (CheckIDData())
    {
      return m_idData.getBirthLocation();
    }
    return "";
  }

  public String getBirthDate()
  {
    if (CheckIDData())
    {
      return m_idData.getBirthDate();
    }
    return "";
  }

  public String getSex()
  {
    if (CheckIDData())
    {
      return m_idData.getSex();
    }
    return "";
  }

  public String getNobleCondition()
  {
    if (CheckIDData())
    {
      return m_idData.getNobleCondition();
    }
    return "";
  }

  public int getDocumentType()
  {
    if (CheckIDData())
    {
      return m_idData.getDocumentType();
    }
    return 0;
  }

  public boolean getWhiteCane()
  {
    if (CheckIDData())
    {
      return m_idData.getWhiteCane();
    }
    return false;
  }

  public boolean getYellowCane()
  {
    if (CheckIDData())
    {
      return m_idData.getYellowCane();
    }
    return false;
  }

  public boolean getExtendedMinority()
  {
    if (CheckIDData())
    {
      return m_idData.getExtendedMinority();
    }
    return false;
  }

  public String getStreet()
  {
    if (CheckAddrData())
    {
      return m_addrData.getStreet();
    }
    return "";
  }

  public String getStreetNumber()
  {
    if (CheckAddrData())
    {
      return m_addrData.getStreetNumber();
    }
    return "";
  }

  public String getBoxNumber()
  {
    if (CheckAddrData())
    {
      return m_addrData.getBoxNumber();
    }
    return "";
  }

  public String getZip()
  {
    if (CheckAddrData())
    {
      return m_addrData.getZip();
    }
    return "";
  }

  public String getMunicipality()
  {
    if (CheckAddrData())
    {
      return m_addrData.getMunicipality();
    }
    return "";
  }

  public String getCountry()
  {
    if (CheckAddrData())
    {
      return m_addrData.getCountry();
    }
    return "";
  }

  public byte[] GetPicture()
  {
    Locale l = java.util.Locale.getDefault();
    String strTemp = getResourceString(l, "ReadPic");
    if(!ShowDialog(strTemp))
      return null;

    BEID_Status oStatus;
    BEID_Bytes PictureData = new BEID_Bytes();
    BEID_Certif_Check CertCheck = new BEID_Certif_Check();
    oStatus = eidlib.BEID_GetPicture(PictureData, CertCheck);
    if (0 == oStatus.getGeneral())
    {
      try
      {
        Rectangle abounds = getBounds();
        java.awt.Toolkit toolkit = Toolkit.getDefaultToolkit();
        Image tempImage = toolkit.createImage(PictureData.getData());
        lblImage.setIcon( new ImageIcon(tempImage.getScaledInstance(abounds.width, abounds.height, Image.SCALE_SMOOTH)));
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }

      return PictureData.getData();
    }
    return null;
  }

  public int SetRawData(byte[] IDData, byte[] SigIDData, byte[] AddrData,
                        byte[] SigAddrData,
                        byte[] PictureData, byte[] RNData, byte[] cardData,
                        byte[] tokenInfoData,
                        byte[] challengeData, byte[] responseData)
  {
    BEID_Status oStatus;
    m_oRaw = null;
    m_oRaw = new BEID_Raw();
    m_oRaw.setAddrData(AddrData);
    m_oRaw.setAddrSigData(SigAddrData);
    m_oRaw.setCardData(cardData);
    m_oRaw.setCertRN(RNData);
    m_oRaw.setChallenge(challengeData);
    m_oRaw.setIdData(IDData);
    m_oRaw.setIdSigData(SigIDData);
    m_oRaw.setPictureData(PictureData);
    m_oRaw.setResponse(responseData);
    m_oRaw.setTokenInfo(tokenInfoData);
    oStatus = eidlib.BEID_SetRawData(m_oRaw);
    return oStatus.getGeneral();
  }

  public byte[] GetRawIDData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getIdData();
    }
    return null;
  }

  public byte[] GetRawSigIDData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getIdSigData();
    }
    return null;
  }

  public byte[] GetRawAddrData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getAddrData();
    }
    return null;
  }

  public byte[] GetRawSigAddrData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getAddrSigData();
    }
    return null;
  }

  public byte[] GetRawPictureData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getPictureData();
    }
    return null;
  }

  public byte[] GetRawCardData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getCardData();
    }
    return null;
  }

  public byte[] GetRawTokenInfoData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getTokenInfo();
    }
    return null;
  }

  public byte[] GetRawRNData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getCertRN();
    }
    return null;
  }

  public byte[] GetRawChallengeData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getChallenge();
    }
    return null;
  }

  public byte[] GetRawResponseData()
  {
    if (CheckRawData())
    {
      return m_oRaw.getResponse();
    }
    return null;
  }

  public String[][] getParameterInfo()
  {
    String pinfo[][] =
    {
        {"Reader", "String", "Card Reader name"},
        {"OCSP", "0-2", "OCSP checking"},
        {"CRL", "0-2", "CRL checking"}
    };
    return pinfo;
  }

  public String getAppletInfo()
  {
    return "Belgian EID Middelware Applet";
  }

  private void jbInit() throws Exception
  {
    jButtonRead.setActionCommand("jButtonRead");
    jButtonRead.setText("Read");
    jButtonRead.addActionListener(new BEID_Applet_jButtonRead_actionAdapter(this));
    // Begin Testing
    // cont.setLayout(new BorderLayout());
    // cont.add(jButtonRead, BorderLayout.NORTH);
    // cont.add(lblImage, BorderLayout.CENTER);
    // End Testing
    cont.add(lblImage);
  }

  public void jButtonRead_actionPerformed(ActionEvent e)
  {
    if (0 == InitLib(null))
    {
      CheckIDData();
      CheckAddrData();
      GetPicture();
      ExitLib();
    }
  }
}

class BEID_Applet_jButtonRead_actionAdapter
    implements ActionListener
{
  private BEID_Applet adaptee;
  BEID_Applet_jButtonRead_actionAdapter(BEID_Applet adaptee)
  {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e)
  {
    adaptee.jButtonRead_actionPerformed(e);
  }
}
