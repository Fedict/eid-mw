package be.belgium.eid;

public class BEID_Address
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Address(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Address(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Address obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public short getVersion()
  {
    return eidlibJNI.get_BEID_Address_version(CPtr);
  }

  public String getStreet()
  {
    return eidlibJNI.get_BEID_Address_street(CPtr);
  }


  public String getStreetNumber()
  {
    return eidlibJNI.get_BEID_Address_streetNumber(CPtr);
  }

  public String getBoxNumber()
  {
    return eidlibJNI.get_BEID_Address_boxNumber(CPtr);
  }

  public String getZip()
  {
    return eidlibJNI.get_BEID_Address_zip(CPtr);
  }

  public String getMunicipality()
  {
    return eidlibJNI.get_BEID_Address_municipality(CPtr);
  }

  public String getCountry()
  {
    return eidlibJNI.get_BEID_Address_country(CPtr);
  }

  public BEID_Address()
  {
    this(eidlibJNI.new_BEID_Address(), true);
  }
}
