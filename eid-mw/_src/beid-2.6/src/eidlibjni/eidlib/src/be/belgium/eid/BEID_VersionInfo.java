package be.belgium.eid;

public class BEID_VersionInfo
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_VersionInfo(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_VersionInfo(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_VersionInfo obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public byte[] getSerialNumber()
  {
    return eidlibJNI.get_BEID_VersionInfo_SerialNumber(CPtr);
  }

  public short getComponentCode()
  {
    return eidlibJNI.get_BEID_VersionInfo_ComponentCode(CPtr);
  }

  public short getOSNumber()
  {
    return eidlibJNI.get_BEID_VersionInfo_OSNumber(CPtr);
  }

  public short getOSVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_OSVersion(CPtr);
  }

  public short getSoftmaskNumber()
  {
    return eidlibJNI.get_BEID_VersionInfo_SoftmaskNumber(CPtr);
  }

  public short getSoftmaskVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_SoftmaskVersion(CPtr);
  }

  public short getAppletVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_AppletVersion(CPtr);
  }

  public int getGlobalOSVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_GlobalOSVersion(CPtr);
  }

  public short getAppletInterfaceVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_AppletInterfaceVersion(CPtr);
  }

  public short getPKCS1Support()
  {
    return eidlibJNI.get_BEID_VersionInfo_PKCS1Support(CPtr);
  }

  public short getKeyExchangeVersion()
  {
    return eidlibJNI.get_BEID_VersionInfo_KeyExchangeVersion(CPtr);
  }

  public short getApplicationLifeCycle()
  {
    return eidlibJNI.get_BEID_VersionInfo_ApplicationLifeCycle(CPtr);
  }

  public short getGraphPerso()
  {
    return eidlibJNI.get_BEID_VersionInfo_GraphPerso(CPtr);
  }

  public short getElecPerso()
  {
    return eidlibJNI.get_BEID_VersionInfo_ElecPerso(CPtr);
  }

  public short getElecPersoInterface()
  {
    return eidlibJNI.get_BEID_VersionInfo_ElecPersoInterface(CPtr);
  }

  public short getReserved()
  {
    return eidlibJNI.get_BEID_VersionInfo_Reserved(CPtr);
  }

  public BEID_VersionInfo()
  {
    this(eidlibJNI.new_BEID_VersionInfo(), true);
  }
}
