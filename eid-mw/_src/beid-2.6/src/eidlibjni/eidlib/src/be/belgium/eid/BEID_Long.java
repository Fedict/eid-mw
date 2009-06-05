package be.belgium.eid;

public class BEID_Long
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Long(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Long(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Long obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public long getLong()
  {
    return eidlibJNI.get_BEID_Long_data(CPtr);
  }

  public BEID_Long()
  {
    this(eidlibJNI.new_BEID_Long(), true);
  }
}
