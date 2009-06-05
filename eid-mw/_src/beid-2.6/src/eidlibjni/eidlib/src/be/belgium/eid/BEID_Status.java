package be.belgium.eid;

public class BEID_Status
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Status(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Status(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Status obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public int getGeneral()
  {
    return eidlibJNI.get_BEID_Status_general(CPtr);
  }

  public int getSystem()
  {
    return eidlibJNI.get_BEID_Status_system(CPtr);
  }

  public int getPcsc()
  {
    return eidlibJNI.get_BEID_Status_pcsc(CPtr);
  }

  public byte[] getCardSW()
  {
    return eidlibJNI.get_BEID_Status_cardSW(CPtr);
  }

  public BEID_Status()
  {
    this(eidlibJNI.new_BEID_Status(), true);
  }

}
