package be.belgium.eid;

public class BEID_Raw
{
  private long CPtr;
  protected boolean CMemOwn;

  protected BEID_Raw(long cPtr, boolean cMemoryOwn)
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
      eidlibJNI.delete_BEID_Raw(CPtr);
    }
    CPtr = 0;
  }

  protected static long getCPtr(BEID_Raw obj)
  {
    return (obj == null) ? 0 : obj.CPtr;
  }

  public void setIdData(byte[] idData)
  {
    eidlibJNI.set_BEID_Raw_idData(CPtr,idData);
  }

  public byte[] getIdData()
  {
    return eidlibJNI.get_BEID_Raw_idData(CPtr);
  }

  public void setIdSigData(byte[] idSigData)
  {
    eidlibJNI.set_BEID_Raw_idSigData(CPtr, idSigData);
  }

  public byte[] getIdSigData()
  {
    return eidlibJNI.get_BEID_Raw_idSigData(CPtr);
  }

  public void setAddrData(byte[] addrData)
  {
    eidlibJNI.set_BEID_Raw_addrData(CPtr, addrData);
  }

  public byte[] getAddrData()
  {
    return eidlibJNI.get_BEID_Raw_addrData(CPtr);
  }

  public void setAddrSigData(byte[] addrSigData)
  {
    eidlibJNI.set_BEID_Raw_addrSigData(CPtr, addrSigData);
  }

  public byte[] getAddrSigData()
  {
    return eidlibJNI.get_BEID_Raw_addrSigData(CPtr);
  }

  public void setPictureData(byte[] pictureData)
  {
    eidlibJNI.set_BEID_Raw_pictureData(CPtr, pictureData);
  }

  public byte[] getPictureData()
  {
    return eidlibJNI.get_BEID_Raw_pictureData(CPtr);
  }

  public void setCardData(byte[] cardData)
  {
    eidlibJNI.set_BEID_Raw_cardData(CPtr, cardData);
  }

  public byte[] getCardData()
  {
    return eidlibJNI.get_BEID_Raw_cardData(CPtr);
  }

  public void setTokenInfo(byte[] tokenInfo)
  {
    eidlibJNI.set_BEID_Raw_tokenInfo(CPtr, tokenInfo);
  }

  public byte[] getTokenInfo()
  {
    return eidlibJNI.get_BEID_Raw_tokenInfo(CPtr);
  }

  public void setCertRN(byte[] certRN)
  {
    eidlibJNI.set_BEID_Raw_certRN(CPtr, certRN);
  }

  public byte[] getCertRN()
  {
    return eidlibJNI.get_BEID_Raw_certRN(CPtr);
  }

  public void setChallenge(byte[] challenge)
  {
    eidlibJNI.set_BEID_Raw_challenge(CPtr, challenge);
  }

  public byte[] getChallenge()
  {
    return eidlibJNI.get_BEID_Raw_challenge(CPtr);
  }

  public void setResponse(byte[] response)
  {
    eidlibJNI.set_BEID_Raw_response(CPtr, response);
  }

  public byte[] getResponse()
  {
    return eidlibJNI.get_BEID_Raw_response(CPtr);
  }

  public BEID_Raw()
  {
    this(eidlibJNI.new_BEID_Raw(), true);
  }

}
