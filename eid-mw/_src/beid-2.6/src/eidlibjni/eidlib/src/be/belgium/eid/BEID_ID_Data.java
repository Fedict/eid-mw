package be.belgium.eid;

public class BEID_ID_Data
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_ID_Data(long cPtr, boolean cMemoryOwn)
  {
    CMemOwn = cMemoryOwn;
    CPtr = cPtr;
  }

  protected void finalize()
  {
    delete();
  }

  public void delete()
  {
    if (CPtr != 0 && CMemOwn)
    {
      CMemOwn = false;
      eidlibJNI.delete_BEID_ID_Data(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_ID_Data obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public short getVersion()
  {
      return eidlibJNI.get_BEID_ID_Data_version(CPtr);
  }

  public String getCardNumber()
  {
    return eidlibJNI.get_BEID_ID_Data_cardNumber(CPtr);
  }

  public String getChipNumber()
  {
    return eidlibJNI.get_BEID_ID_Data_chipNumber(CPtr);
  }

  public String getValidityDateBegin()
  {
    return eidlibJNI.get_BEID_ID_Data_validityDateBegin(CPtr);
  }

  public String getValidityDateEnd()
  {
    return eidlibJNI.get_BEID_ID_Data_validityDateEnd(CPtr);
  }

  public String getMunicipality()
  {
    return eidlibJNI.get_BEID_ID_Data_municipality(CPtr);
  }

  public String getNationalNumber()
  {
    return eidlibJNI.get_BEID_ID_Data_nationalNumber(CPtr);
  }

  public String getName()
  {
    return eidlibJNI.get_BEID_ID_Data_name(CPtr);
   }

  public String getFirstName1()
  {
    return eidlibJNI.get_BEID_ID_Data_firstName1(CPtr);
  }

  public String getFirstName2()
  {
    return eidlibJNI.get_BEID_ID_Data_firstName2(CPtr);
  }

  public String getFirstName3()
  {
    return eidlibJNI.get_BEID_ID_Data_firstName3(CPtr);
  }

  public String getNationality()
  {
    return eidlibJNI.get_BEID_ID_Data_nationality(CPtr);
  }

  public String getBirthLocation()
  {
    return eidlibJNI.get_BEID_ID_Data_birthLocation(CPtr);
  }

  public String getBirthDate()
  {
    return eidlibJNI.get_BEID_ID_Data_birthDate(CPtr);
  }

  public String getSex()
  {
    return eidlibJNI.get_BEID_ID_Data_sex(CPtr);
  }

  public String getNobleCondition()
  {
    return eidlibJNI.get_BEID_ID_Data_nobleCondition(CPtr);
  }

  public int getDocumentType()
  {
    return eidlibJNI.get_BEID_ID_Data_documentType(CPtr);
  }

  public boolean getWhiteCane()
  {
    return eidlibJNI.get_BEID_ID_Data_whiteCane(CPtr);
  }

  public boolean getYellowCane()
  {
    return eidlibJNI.get_BEID_ID_Data_yellowCane(CPtr);
  }

  public boolean getExtendedMinority()
  {
    return eidlibJNI.get_BEID_ID_Data_extendedMinority(CPtr);
  }

  public byte[] getHashPhoto()
  {
    return eidlibJNI.get_BEID_ID_Data_hashPhoto(CPtr);
  }

  public BEID_ID_Data()
  {
    this(eidlibJNI.new_BEID_ID_Data(), true);
  }
}
