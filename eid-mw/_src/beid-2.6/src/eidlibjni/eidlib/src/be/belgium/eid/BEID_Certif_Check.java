package be.belgium.eid;

public class BEID_Certif_Check
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Certif_Check(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Certif_Check(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Certif_Check obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public int getUsedPolicy()
  {
    return eidlibJNI.get_BEID_Certif_Check_usedPolicy(CPtr);
  }

  public BEID_Certif getCertificate(int Index)
  {
      long cPtr = eidlibJNI.get_BEID_Certif_Check_certificate(CPtr, Index);
      return (cPtr == 0) ? null : new BEID_Certif(cPtr, false);
  }

  public int getCertificatesLength()
  {
      return eidlibJNI.get_BEID_Certif_Check_certificatesLength(CPtr);
  }

  public int getSignatureCheck()
  {
    return eidlibJNI.get_BEID_Certif_Check_signatureCheck(CPtr);
  }

  public BEID_Certif_Check()
  {
    this(eidlibJNI.new_BEID_Certif_Check(), true);
  }
}
