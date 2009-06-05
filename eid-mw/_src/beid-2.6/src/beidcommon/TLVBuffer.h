// TLVBuffer.h: interface for the CTLVBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLVBUFFER_H__E687DC32_54AD_4FE8_A520_2060F03D8729__INCLUDED_)
#define AFX_TLVBUFFER_H__E687DC32_54AD_4FE8_A520_2060F03D8729__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable:4786)
#endif

#include <map>
#include "TLV.h"
#include "eidlib.h"

namespace eidcommon
{

class CTLVBuffer  
{
public:
	CTLVBuffer();
	virtual ~CTLVBuffer();
    
    bool MakeFileTLV(BEID_Raw *pRawData, BEID_Bytes *pOneRaw);
    int ParseTLV(unsigned char *pucData, unsigned long ulLen);
    bool ParseFileTLV(unsigned char *pucData, unsigned long ulLen);
    CTLV *GetTagData(unsigned char ucTag);
    void FillASCIIData(unsigned char ucTag, char *pData);
    void FillUTF8Data(unsigned char ucTag, char *pData);
    void FillBinaryStringData(unsigned char ucTag, char *pData);
    void FillShortData(unsigned char ucTag, short *psData);
    void FillIntData(unsigned char ucTag, int *piData);
    void FillLongData(unsigned char ucTag, long *piData);
    void FillBinaryData(unsigned char ucTag, unsigned char *pData);
    void FillBinaryData(unsigned char ucTag, unsigned char *pData, unsigned long *pulLen);
    void FillDateData(unsigned char ucTag, char *pData);
    unsigned long GetLengthForSignature();
    static char *Hexify(unsigned char * pData, unsigned long ulLen);
    bool TlvEncodeLen(unsigned long ulLenVal, unsigned char *pucBufDest, int *piBufLen);
    bool TlvDecodeLen(unsigned char *pucBufSrc, int *piBufLen,  unsigned long *pulLenVal);

protected:
    int BytesToInt(unsigned char * pData, unsigned long ulLen);
    unsigned short BytesToShort(unsigned char *pucData, int iOffset, bool msbFirst = true);
    void FillMonth(unsigned char *pDate, char *pMonth);

private:
    static char hexChars[];
    typedef std::map<unsigned char, CTLV *> MapTLV; 
    typedef MapTLV::iterator ITMap;
    MapTLV m_oMapTLV;
};

}  // namespace eidcommon

#endif // !defined(AFX_TLVBUFFER_H__E687DC32_54AD_4FE8_A520_2060F03D8729__INCLUDED_)
