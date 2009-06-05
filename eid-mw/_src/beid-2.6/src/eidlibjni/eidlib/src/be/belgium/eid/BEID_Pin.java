package be.belgium.eid;

public class BEID_Pin
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Pin(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Pin(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Pin obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public void setPinType(int pinType)
  {
    eidlibJNI.set_BEID_Pin_pinType(CPtr, pinType);
  }

  public void setId(short id)
  {
    eidlibJNI.set_BEID_Pin_id(CPtr, id);
  }

  public void setUsageCode(int usageCode)
  {
    eidlibJNI.set_BEID_Pin_usageCode(CPtr, usageCode);
  }

  public void setShortUsage(String shortUsage)
  {
    eidlibJNI.set_BEID_Pin_shortUsage(CPtr, shortUsage);
  }

  public void setLongUsage(String longUsage)
  {
    eidlibJNI.set_BEID_Pin_longUsage(CPtr, longUsage);
  }

  public BEID_Pin()
  {
    this(eidlibJNI.new_BEID_Pin(), true);
  }

}
