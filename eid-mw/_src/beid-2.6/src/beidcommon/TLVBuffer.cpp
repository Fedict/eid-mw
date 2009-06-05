// TLVBuffer.cpp: implementation of the CTLVBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include "TLVBuffer.h"

namespace eidcommon
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char CTLVBuffer::hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

CTLVBuffer::CTLVBuffer()
{

}

CTLVBuffer::~CTLVBuffer()
{
    ITMap it;
    for (it = m_oMapTLV.begin(); it != m_oMapTLV.end(); ++it)
    {
        CTLV *pTemp = (*it).second;
        if(pTemp != NULL)
        {
            delete pTemp;
        }
    }
    m_oMapTLV.clear();
}

int CTLVBuffer::ParseTLV(unsigned char *pucData, unsigned long ulLen)
{
    int iRet = 0;
    
    if(pucData != NULL && ulLen > 0)
    {
        m_oMapTLV.clear();

        unsigned long ulIndex = 0;
        while (ulIndex < ulLen)
        {
            unsigned char ucTag = pucData[ulIndex++];
            unsigned long ulFieldLen = pucData[ulIndex];
            
            if(ucTag == 0x00 && ulIndex > 2)
            {
                break;
            }
            while(0xFF == pucData[ulIndex])
            {
                ulFieldLen += pucData[++ulIndex];
            }
            ++ulIndex;
            m_oMapTLV[ucTag] = new CTLV(ucTag, pucData + ulIndex, ulFieldLen);
            ulIndex += ulFieldLen;
        }
        iRet = 1;
    }
    return iRet;
}

CTLV *CTLVBuffer::GetTagData(unsigned char ucTag)
{
    ITMap it = m_oMapTLV.find(ucTag);
    if(it != m_oMapTLV.end())
    {
        return(*it).second;
    }
    return NULL;
}

void CTLVBuffer::FillDateData(unsigned char ucTag, char *pData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        // Format Data to YYYYMMDD
        unsigned char *pTemp = NULL;
        unsigned char *pDate = pTagData->GetData(); 
        int iLen = pTagData->GetLength();
        if(BEID_MAX_DATE_BEGIN_LEN == iLen && pDate[1] == ' ')
        {
            pTemp = new unsigned char[iLen + 2];
            memset(pTemp, 0, iLen + 2);
            pTemp[0] = '0';
            memcpy(pTemp + 1, pDate, iLen);
            pDate = pTemp;
            iLen++;
        }
        if (BEID_MAX_DATE_BEGIN_LEN == iLen)
        {
            // DD.MM.YYYY
            pData[0] = pDate[6];    
            pData[1] = pDate[7];    
            pData[2] = pDate[8];    
            pData[3] = pDate[9];    
            pData[4] = pDate[3];    
            pData[5] = pDate[4];    
            pData[6] = pDate[0];    
            pData[7] = pDate[1];    
        }
        else if (iLen > BEID_MAX_DATE_BEGIN_LEN)
        {
            // DD mmmm YYYY or DD.mmm.YYYY
            pData[0] = pDate[iLen-4];    
            pData[1] = pDate[iLen-3];    
            pData[2] = pDate[iLen-2];    
            pData[3] = pDate[iLen-1];       
            pData[6] = pDate[0];    
            pData[7] = pDate[1];
            FillMonth(pDate + 3, pData + 4);
        }
        else
        {
            memcpy(pData, pTagData->GetData(), pTagData->GetLength());
        }
        if(pTemp != NULL)
        {
            delete[] pTemp;
        }
    }
}

void CTLVBuffer::FillASCIIData(unsigned char ucTag, char *pData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        memcpy(pData, pTagData->GetData(), pTagData->GetLength());
    }
}

void CTLVBuffer::FillUTF8Data(unsigned char ucTag, char *pData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        memcpy(pData, pTagData->GetData(), pTagData->GetLength());
    }
}

void CTLVBuffer::FillBinaryStringData(unsigned char ucTag, char *pData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        char *pszTemp = Hexify(pTagData->GetData(), pTagData->GetLength());
        memcpy(pData, pszTemp, strlen(pszTemp));
        delete pszTemp;
    }
}

void CTLVBuffer::FillShortData(unsigned char ucTag, short *psData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        *psData = BytesToShort(pTagData->GetData(), 0, true);
    }
}

void CTLVBuffer::FillIntData(unsigned char ucTag, int *piData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        char *pszTemp = new char[pTagData->GetLength() + 1];
        memset(pszTemp, 0, pTagData->GetLength() + 1);
        memcpy(pszTemp, pTagData->GetData(), pTagData->GetLength());
        *piData = atoi(pszTemp);
        delete []pszTemp;
    }
}

void CTLVBuffer::FillLongData(unsigned char ucTag, long *piData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        char *pszTemp = new char[pTagData->GetLength() + 1];
        memset(pszTemp, 0, pTagData->GetLength() + 1);
        memcpy(pszTemp, pTagData->GetData(), pTagData->GetLength());
        *piData = atol(pszTemp);
        delete []pszTemp;
    }
}

void CTLVBuffer::FillBinaryData(unsigned char ucTag, unsigned char *pData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        memcpy(pData, pTagData->GetData(), pTagData->GetLength());
    }
}

void CTLVBuffer::FillBinaryData(unsigned char ucTag, unsigned char *pData, unsigned long *pulLen)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        memcpy(pData, pTagData->GetData(), pTagData->GetLength());
        *pulLen = pTagData->GetLength();
    }
}

char *CTLVBuffer::Hexify(unsigned char * pData, unsigned long ulLen) 
{
    char *pszHex = new char[ulLen*2 + 1];
    memset(pszHex, 0, ulLen*2 + 1);
    if(pData != NULL)
    {
        int j = 0;
        for(unsigned long i = 0; i < ulLen; i++) 
        {
            pszHex[j++] = hexChars[pData[i]>>4 & 0x0F];
            pszHex[j++] = hexChars[pData[i] & 0x0F];
         }
    }
    return pszHex;
}

int CTLVBuffer::BytesToInt(unsigned char * pData, unsigned long ulLen) 
{ 
    int iOut = 0; 
    for (unsigned int i = 0; i < ulLen; i++) 
    { 
        iOut = iOut << 4; 
        iOut = iOut | ((pData[i] >> 4) & 0x0F); 
        iOut = iOut << 4; 
        iOut = iOut | (pData[i] & 0x0F); 
    }
    return iOut; 
}  

unsigned short CTLVBuffer::BytesToShort(unsigned char *pucData, int iOffset, bool msbFirst /* true */) 
{
    if(msbFirst)
	    return (256 * (((unsigned short)pucData[iOffset]) & 255) + (((int)pucData[iOffset+1]) & 255));
    else
	    return ((((unsigned short)pucData[iOffset]) & 255) + 256 * (((int)pucData[iOffset+1]) & 255));
}

unsigned long CTLVBuffer::GetLengthForSignature()
{
    unsigned long lRet = 0;
    ITMap it;
    for (it = m_oMapTLV.begin(); it != m_oMapTLV.end(); ++it)
    {
        CTLV *pTemp = (*it).second;
        if(pTemp != NULL)
        {
            lRet += pTemp->GetLength();
            lRet += 2;
        }
    }
    return lRet;
}

void CTLVBuffer::FillMonth(unsigned char *pDate, char *pMonth)
{
    if(0 == memcmp(pDate, "JAN", 3))
    {
        pMonth[0] = '0';
        pMonth[1] = '1';
        return;
    }
    if(0 == memcmp(pDate, "FEB", 3) || 0 == memcmp(pDate, "FEV", 3))
    {
        pMonth[0] = '0';
        pMonth[1] = '2';
        return;
    }
    if(0 == memcmp(pDate, "MARS", 4) || 0 == memcmp(pDate, "MAAR", 4) || 0 == memcmp(pDate, "\x4D\xC3\x84\x52", 4))
    {
        pMonth[0] = '0';
        pMonth[1] = '3';
        return;
    }
    if(0 == memcmp(pDate, "APR", 3) || 0 == memcmp(pDate, "AVR", 3))
    {
        pMonth[0] = '0';
        pMonth[1] = '4';
        return;
    }
    if(0 == memcmp(pDate, "MAI", 3) || 0 == memcmp(pDate, "MEI", 3))
    {
        pMonth[0] = '0';
        pMonth[1] = '5';
        return;
    }
    if(0 == memcmp(pDate, "JUN", 3) || 0 == memcmp(pDate, "JUIN", 4))
    {
        pMonth[0] = '0';
        pMonth[1] = '6';
        return;
    }
    if(0 == memcmp(pDate, "JUL", 3) || 0 == memcmp(pDate, "JUIL", 4))
    {
        pMonth[0] = '0';
        pMonth[1] = '7';
        return;
    }
    if(0 == memcmp(pDate, "AUG", 3) || 0 == memcmp(pDate, "AOUT", 4))
    {
        pMonth[0] = '0';
        pMonth[1] = '8';
        return;
    }
    if(0 == memcmp(pDate, "SEP", 3) || 0 == memcmp(pDate, "SEPT", 4))
    {
        pMonth[0] = '0';
        pMonth[1] = '9';
        return;
    }
    if(0 == memcmp(pDate, "OKT", 3) || 0 == memcmp(pDate, "OCT", 3))
    {
        pMonth[0] = '1';
        pMonth[1] = '0';
        return;
    }
    if(0 == memcmp(pDate, "NOV", 3))
    {
        pMonth[0] = '1';
        pMonth[1] = '1';
        return;
    }
    if(0 == memcmp(pDate, "DEC", 3) || 0 == memcmp(pDate, "DEZ", 3))
    {
        pMonth[0] = '1';
        pMonth[1] = '2';
        return;
    }
}

/**********************************************************************************************************
Encode the length in the TLV format    

    Default one length byte.
    The highest bit = 1 means one additional length-byte.
    The lowest 7 bits of each byte are 7 bits of the length-number.

    1 length byte  will encode max 7-bit  numbers, between     00 -       7F  
    2 length bytes will encode max 14-bit numbers, between     80 -     3FFF
    3 length bytes will encode max 21-bit numbers, between    400 -  1F FFFF
    4 length bytes will encode max 28-bit numbers, between 20 000 - FFF FFFF
    
examples:
  len=7B        -> encoding =          7B    
  len=CD        -> encoding =       81 4D
  len=2039      -> encoding =       C0 39 
  len=43ABDA    -> encoding = 82 8E D7 5A
    
 **********************************************************************************************************/
bool CTLVBuffer::TlvEncodeLen(unsigned long ulLenVal,  //In: length-value to encode in the TLV-format
                    unsigned char *pucBufDest,  //In/Out: Destination buffer with the encoded length-stream
                    int *piBufLen  //In/out: Max buffer length, Out:length of encoded stream
                    )
{
    int iStreamLen = 1;         //length of the stream
    unsigned long  ulLenValLoc = 0;        //length
    unsigned char ucExtraByte = 0x00;

    //--- parameter check   
    if (pucBufDest == NULL || piBufLen == NULL)
    {
        return false;
    }

    //--- determinate number of length-bytes
    ulLenValLoc = ulLenVal>>7;

    while(ulLenValLoc)
    {
        iStreamLen++;
        ulLenValLoc >>= 7;
    }

    if (*piBufLen < iStreamLen)
    {
        return false;
    }
    *piBufLen = iStreamLen;

    //--- write encoded length
    ulLenValLoc = ulLenVal;
    pucBufDest[0] = 0;
    ucExtraByte = 0;
    while(iStreamLen--)
    {
        pucBufDest[iStreamLen] = ((unsigned char)ulLenValLoc & 0x7F) + ucExtraByte;
        ucExtraByte = 0x80;
        ulLenValLoc >>= 7;
    }

    return true;
}

/**********************************************************************************************************
Decode the length from the TLV format    
   
 **********************************************************************************************************/
bool CTLVBuffer::TlvDecodeLen(unsigned char *pucBufSrc, //In: Source buffer with the encoded length-stream
                    int *piBufLen,  //In/Out: Maximum length of encoded stream, Out:number of length-bytes
                    unsigned long *pulLenVal  //Out: length-value from the TLV stream
                    )
{
    int  iStreamLenMax = 0; //length of the stream

    //--- parameter check   
    if (pucBufSrc == NULL || piBufLen == NULL || (*piBufLen == 0) || pulLenVal == NULL)
    {
        return false;
    }

    //--- decode first byte   
    iStreamLenMax = *piBufLen;
    *piBufLen = 1;
    *pulLenVal = 0x7F & *pucBufSrc;

    //--- handle additional length bytes   
    while( *(pucBufSrc++) & 0x80)
    {
        if (++(*piBufLen) > iStreamLenMax)
        {
            return false;
        }

        *pulLenVal = (*pulLenVal<<7) + (0x7F & *pucBufSrc);
    }
    return true;
}

bool CTLVBuffer::MakeFileTLV(BEID_Raw *pRawData, BEID_Bytes *pOneRaw)
{
    bool bRet = false;
    
    if(pRawData != NULL && pOneRaw != NULL)
    {
        unsigned char ucTag = 0x00;
        unsigned char ucLenBuffer[4] = {0};
        int iBufLen = sizeof(ucLenBuffer);
        CByteArray oTempArray;

        // Version Tag
        oTempArray.Append(ucTag);
        oTempArray.Append(0x01);
        oTempArray.Append(0x01);
    
        // Data
        if(TlvEncodeLen(pRawData->idLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->idData, pRawData->idLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->idSigLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->idSigData, pRawData->idSigLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->addrLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->addrData, pRawData->addrLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->addrSigLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->addrSigData, pRawData->addrSigLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->pictureLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->pictureData, pRawData->pictureLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->cardDataLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->cardData, pRawData->cardDataLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->tokenInfoLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->tokenInfo, pRawData->tokenInfoLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->certRNLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->certRN, pRawData->certRNLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->challengeLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->challenge, pRawData->challengeLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);

        if(TlvEncodeLen(pRawData->responseLength, ucLenBuffer, &iBufLen))
        {
            oTempArray.Append(++ucTag);
            oTempArray.Append(ucLenBuffer, iBufLen);         
            oTempArray.Append(pRawData->response, pRawData->responseLength);         
        }
        iBufLen = sizeof(ucLenBuffer);
        memset(ucLenBuffer, 0, iBufLen);
        
        unsigned long ulSize = oTempArray.GetSize();
        if(pOneRaw->length >= ulSize)
        {
            pOneRaw->length = ulSize;
            memcpy(pOneRaw->data, oTempArray.GetData(), ulSize);
            bRet = true;
        }
    }
    return bRet;
}

bool CTLVBuffer::ParseFileTLV(unsigned char *pucData, unsigned long ulLen)
{
    bool bRet = false;
    
    if(pucData != NULL && ulLen > 0)
    {
        m_oMapTLV.clear();

        bRet = true;
        unsigned long ulIndex = 0;
        while (ulIndex < ulLen)
        {
            unsigned char ucTag = pucData[ulIndex++];
            unsigned long ulFieldLen = 0;
            
            if((ucTag == 0x00 && ulIndex > 2) || (ucTag != 0x00 && ulIndex < 2))
            {
                bRet = false;
                break;
            }
            int iNrBytes = ulLen - ulIndex - 1;
            if (!TlvDecodeLen(pucData + ulIndex, &iNrBytes,  &ulFieldLen))            
            {
                bRet = false;
                break;
            }
            ulIndex += iNrBytes;
            m_oMapTLV[ucTag] = new CTLV(ucTag, pucData + ulIndex, ulFieldLen);
            ulIndex += ulFieldLen;
        }
    }
    return bRet;
}


}  // namespace eidcommon
