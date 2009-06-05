package be.belgium.eid;

public class eidlib
{
  public static BEID_Status BEID_Init(String ReaderName, int OCSP, int CRL,
                                      BEID_Long CardHandle)
  {
    return new BEID_Status(eidlibJNI.BEID_Init(ReaderName, OCSP, CRL, BEID_Long.getCPtr(CardHandle)), true);
  }

  public static BEID_Status BEID_Exit()
  {
    return new BEID_Status(eidlibJNI.BEID_Exit(), true);
  }

  public static BEID_Status BEID_GetID(BEID_ID_Data IDData,
                                       BEID_Certif_Check CertifCheck)
  {
    return new BEID_Status(eidlibJNI.BEID_GetID(BEID_ID_Data.getCPtr(IDData),
                                                BEID_Certif_Check.getCPtr(CertifCheck)), true);
  }

  public static BEID_Status BEID_GetAddress(BEID_Address Address,
                                            BEID_Certif_Check CertifCheck)
  {
    return new BEID_Status(eidlibJNI.BEID_GetAddress(BEID_Address.getCPtr(
        Address), BEID_Certif_Check.getCPtr(CertifCheck)), true);
  }

  public static BEID_Status BEID_GetPicture(BEID_Bytes Picture,
                                            BEID_Certif_Check CertifCheck)
  {
    return new BEID_Status(eidlibJNI.BEID_GetPicture(BEID_Bytes.getCPtr(Picture) ,
        BEID_Certif_Check.getCPtr(CertifCheck)), true);
  }

  public static BEID_Status BEID_GetRawData(BEID_Raw RawData)
  {
    return new BEID_Status(eidlibJNI.BEID_GetRawData(BEID_Raw.getCPtr(RawData)), true);
  }

  public static BEID_Status BEID_SetRawData(BEID_Raw RawData)
  {
    return new BEID_Status(eidlibJNI.BEID_SetRawData(BEID_Raw.getCPtr(RawData)), true);
  }

  public static BEID_Status BEID_GetVersionInfo(BEID_VersionInfo VersionInfo,
                                                int Signature,
                                                BEID_Bytes SignedStatus)
  {
    return new BEID_Status(eidlibJNI.BEID_GetVersionInfo(BEID_VersionInfo.
        getCPtr(VersionInfo), Signature, BEID_Bytes.getCPtr(SignedStatus)), true);
  }

  public static BEID_Status BEID_BeginTransaction()
  {
    return new BEID_Status(eidlibJNI.BEID_BeginTransaction(), true);
  }

  public static BEID_Status BEID_EndTransaction()
  {
    return new BEID_Status(eidlibJNI.BEID_EndTransaction(), true);
  }

  public static BEID_Status BEID_SelectApplication(byte[] Application)
  {
    return new BEID_Status(eidlibJNI.BEID_SelectApplication(Application), true);
  }

  public static BEID_Status BEID_VerifyPIN(BEID_Pin PinData, String Pin,
                                           BEID_Long TriesLeft)
  {
    return new BEID_Status(eidlibJNI.BEID_VerifyPIN(BEID_Pin.getCPtr(PinData), Pin, BEID_Long.getCPtr(TriesLeft)), true);
  }

  public static BEID_Status BEID_ChangePIN(BEID_Pin PinData,
                                           String pszOldPin, String pszNewPin, BEID_Long TriesLeft)
  {
    return new BEID_Status(eidlibJNI.BEID_ChangePIN(BEID_Pin.getCPtr(PinData), pszOldPin,
        pszNewPin, BEID_Long.getCPtr(TriesLeft)), true);
  }

  public static BEID_Status BEID_GetPINStatus(BEID_Pin PinData,
                                              BEID_Long TriesLeft,
                                              int Signature,
                                              BEID_Bytes SignedStatus)
  {
    return new BEID_Status(eidlibJNI.BEID_GetPINStatus(BEID_Pin.getCPtr(PinData),
        BEID_Long.getCPtr(TriesLeft), Signature, BEID_Bytes.getCPtr(SignedStatus)), true);
  }

  public static BEID_Status BEID_ReadFile(byte[] FileID, BEID_Bytes OutData,
                                          BEID_Pin PinData)
  {
    return new BEID_Status(eidlibJNI.BEID_ReadFile(FileID, BEID_Bytes.getCPtr(OutData), BEID_Pin.getCPtr(PinData)), true);
  }

  public static BEID_Status BEID_WriteFile(byte[] FileID, byte[] InData, BEID_Pin PinData)
  {
    return new BEID_Status(eidlibJNI.BEID_WriteFile(FileID, InData, BEID_Pin.getCPtr(PinData)), true);
  }

  public static BEID_Status BEID_FlushCache()
  {
    return new BEID_Status(eidlibJNI.BEID_FlushCache(), true);
  }

  public static BEID_Status BEID_SendAPDU(byte[] CmdAPDU,
                                          BEID_Pin PinData,
                                          BEID_Bytes RespAPDU)
  {
    return new BEID_Status(eidlibJNI.BEID_SendAPDU(CmdAPDU,BEID_Pin.getCPtr(PinData), BEID_Bytes.getCPtr(RespAPDU)), true);
  }

}
