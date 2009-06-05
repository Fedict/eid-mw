package be.belgium.eid;

public class BEID_Bytes
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Bytes(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Bytes(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Bytes obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public byte[] getData()
  {
    return eidlibJNI.get_BEID_Bytes_data(CPtr);
  }

  public BEID_Bytes()
  {
    this(eidlibJNI.new_BEID_Bytes(), true);
  }

}
