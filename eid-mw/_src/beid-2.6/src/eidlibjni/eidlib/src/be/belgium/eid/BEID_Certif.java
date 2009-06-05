package be.belgium.eid;

public class BEID_Certif
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Certif(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Certif(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Certif obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public byte[] getCertif()
  {
    return eidlibJNI.get_BEID_Certif_certif(CPtr);
  }


  public String getCertifLabel()
  {
    return eidlibJNI.get_BEID_Certif_certifLabel(CPtr);
  }


  public int getCertifStatus()
  {
    return eidlibJNI.get_BEID_Certif_certifStatus(CPtr);
  }

  public BEID_Certif()
  {
    this(eidlibJNI.new_BEID_Certif(), true);
  }
}
