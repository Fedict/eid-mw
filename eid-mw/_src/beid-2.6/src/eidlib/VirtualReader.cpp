// VirtualReader.cpp: implementation of the CVirtualReader class.
//
//////////////////////////////////////////////////////////////////////

#include "VirtualReader.h"
#include "eidlib.h"
#include "eiderrors.h"
#include "../beidcommon/beidconst.h"

#define READERNAME  "VIRTUAL"
#define VIRTUAL_HANDLE  99999

using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualReader::CVirtualReader()
{
}

CVirtualReader::~CVirtualReader()
{
    CleanUp();
}

void CVirtualReader::CleanUp()
{
}

const char *CVirtualReader::GetConnectedName()
{
    return READERNAME;
}

long CVirtualReader::Connect(const char *pszReaderName, BEID_Status *ptStatus)
{
    long lRet = BEID_E_NOT_INITIALIZED;

    if(pszReaderName != NULL || strlen(pszReaderName) > 0)
    {
        if(0 == strcmp(READERNAME, pszReaderName))
        {
            m_hCard = VIRTUAL_HANDLE;
            lRet = SC_NO_ERROR;
        }
    }

    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::Disconnect(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_hCard > 0)
    {
        m_hCard = 0;
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::ReadFile(const unsigned char *ucFile, int iFileLen, unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_hCard > 0)
    {
        if(SC_NO_ERROR == (lRet = BeginTransaction(ptStatus)))
        {
            lRet = SelectFile(ucFile, iFileLen, ptStatus);
            if(SC_NO_ERROR == lRet && CheckSW(ptStatus))
            {
                lRet = ReadBinary(pucOutput, pulOutLen, ptStatus);
            }
            EndTransaction(ptStatus);
        }
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long CVirtualReader::SelectFile(const unsigned char *ucFile, int iFileLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_hCard > 0)
    {
        m_SelectedFile.RemoveAll();     
        m_SelectedFile.Append((unsigned char *)ucFile, iFileLen);
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long CVirtualReader::ReadBinary(unsigned char *pszOutput, unsigned long *pulOutLen, BEID_Status *ptStatus, int iOffset /* 0 */, int iCount /* MAX_FILE_SIZE */)
{
    long lRet = SC_ERROR_FILE_NOT_FOUND;
    *pulOutLen = 0;
    memset(ptStatus->cardSW, 0, 2);
    if(m_hCard > 0)
    {
        if(m_SelectedFile.GetSize() > 1)
        {
            CByteArray oCurrent;
            int iBound = m_SelectedFile.GetUpperBound();
            switch(m_SelectedFile.GetAt(iBound - 1) )
            {
            case g_ucDataTag:
                 switch(m_SelectedFile.GetAt(iBound) )
                {
                 case g_ucDataTagID:
                       oCurrent += m_ByteID;
                     break;
                 case g_ucDataTagIDSIG:
                       oCurrent += m_ByteSigID;
                     break;
                 case g_ucDataTagADDR:
                       oCurrent += m_ByteAddress;
                     break;
                 case g_ucDataTagADDRSSIG:
                       oCurrent += m_ByteSigAddress;
                     break;
                 case g_ucDataTagPHOTO:
                       oCurrent += m_BytePicture;
                     break;
                }
                break;
            case g_ucCertTag:
                 switch(m_SelectedFile.GetAt(iBound) )
                {
                 case g_ucDataTagTOKENINFO:
                       oCurrent += m_ByteTokenInfo;
                     break;
                 case g_ucDataTagRN:
                       oCurrent += m_ByteRNCert;
                     break;                
                 }                
                break;
            }
            long lBytesRead = 0;
            if((lBytesRead = oCurrent.GetSize()) > 0)
            {
                memcpy(pszOutput, oCurrent.GetData(), lBytesRead);
                *pulOutLen = lBytesRead;
                lRet = SC_NO_ERROR;
            }
        }
    }
    ConvertOpenSCError(lRet, ptStatus);

    return lRet;
}

long CVirtualReader::Transmit(const unsigned char *pucSend, int iSendLen, unsigned char *pucRecv, unsigned long *pulRecvLen, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_CARD_CMD_FAILED;
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::SelectFile(const unsigned char *ucFile, int iFileLen, unsigned char ucP1, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_hCard > 0)
    {
        m_SelectedFile.RemoveAll();     
        m_SelectedFile.Append((unsigned char *)ucFile, iFileLen);
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long CVirtualReader::GetCardData(unsigned char *pucOutput, unsigned long *pulOutLen, bool bSigned, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_FILE_NOT_FOUND;
    *pulOutLen = 0;
    memset(ptStatus->cardSW, 0, 2);
    if(m_hCard > 0)
    {
        if(m_ByteCardData.GetSize() >= BEID_MAX_CARD_DATA_LEN)
        {
            memcpy(pucOutput, m_ByteCardData.GetData(), BEID_MAX_CARD_DATA_LEN);
            *pulOutLen = BEID_MAX_CARD_DATA_LEN;            
            lRet = SC_NO_ERROR;
        }
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::GetPINStatus(unsigned char *pucOutput, unsigned long *pulOutLen, unsigned char ucPinReference, bool bSigned, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_CARD_CMD_FAILED;
    *pulOutLen = 0;
    memset(ptStatus->cardSW, 0, 2);
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::UpdateBinary(unsigned char *pucInput, unsigned long ulInLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_hCard > 0)
    {
        if(m_SelectedFile.GetSize() > 1 && ulInLen > 0)
        {
            CByteArray oNew(pucInput, ulInLen);
            int iBound = m_SelectedFile.GetUpperBound();
            switch(m_SelectedFile.GetAt(iBound - 1) )
            {
            case g_ucDataTag:
                 switch(m_SelectedFile.GetAt(iBound) )
                {
                 case g_ucDataTagID:
                       m_ByteID.InsertAt(0, oNew);
                     break;
                 case g_ucDataTagIDSIG:
                       m_ByteSigID.InsertAt(0, oNew);
                     break;
                 case g_ucDataTagADDR:
                       m_ByteAddress.InsertAt(0, oNew);
                     break;
                 case g_ucDataTagADDRSSIG:
                       m_ByteSigAddress.InsertAt(0, oNew);
                     break;
                 case g_ucDataTagPHOTO:
                       m_BytePicture.InsertAt(0, oNew);
                     break;
                }
                break;
            case g_ucCertTag:
                break;
            }
        }
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_CARD_CMD_FAILED;
    *piTriesLeft = -1;
    memset(ptStatus->cardSW, 0, 2);    
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_CARD_CMD_FAILED;
    *piTriesLeft = -1;
    memset(ptStatus->cardSW, 0, 2);    
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

void CVirtualReader::FlushCache()
{
    CleanUp();
}

void CVirtualReader::SetRawData(BEID_Raw *pRawData)
{
    m_ByteID.RemoveAll();
    m_ByteID.Append(pRawData->idData, pRawData->idLength);  
    m_ByteSigID.RemoveAll();
    m_ByteSigID.Append(pRawData->idSigData, pRawData->idSigLength);  
    m_ByteAddress.RemoveAll();
    m_ByteAddress.Append(pRawData->addrData, pRawData->addrLength);  
    m_ByteSigAddress.RemoveAll();
    m_ByteSigAddress.Append(pRawData->addrSigData, pRawData->addrSigLength);  
    m_BytePicture.RemoveAll();
    m_BytePicture.Append(pRawData->pictureData, pRawData->pictureLength);  
    m_ByteCardData.RemoveAll();
    m_ByteCardData.Append(pRawData->cardData, pRawData->cardDataLength);  
    m_ByteTokenInfo.RemoveAll();
    m_ByteTokenInfo.Append(pRawData->tokenInfo, pRawData->tokenInfoLength);  
    m_ByteRNCert.RemoveAll();
    m_ByteRNCert.Append(pRawData->certRN, pRawData->certRNLength); 
    m_Challenge.RemoveAll();
    m_Challenge.Append(pRawData->challenge, pRawData->challengeLength);   
    m_Response.RemoveAll(); 
    m_Response.Append(pRawData->response, pRawData->responseLength);   
}

long CVirtualReader::GetChallengeResponse(unsigned char *pucOutputCh, unsigned long *pulOutLenCh, 
                                         unsigned char *pucOutputResp, unsigned long *pulOutLenResp, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    *pulOutLenCh = 0;
    *pulOutLenResp = 0;
    memset(ptStatus->cardSW, 0, 2);
    
    if(m_hCard > 0)
    {
        if(m_Challenge.GetSize() > 0 && m_Response.GetSize() > 0)
        {
            memcpy(pucOutputCh, m_Challenge.GetData(), m_Challenge.GetSize());
            *pulOutLenCh = m_Challenge.GetSize();
            memcpy(pucOutputResp, m_Response.GetData(), m_Response.GetSize());
            *pulOutLenResp = m_Response.GetSize();            
        }
    }

    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long CVirtualReader::GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_CARD_CMD_FAILED;
    pPins->pinsLength = 0;
    memset(ptStatus->cardSW, 0, 2);
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}
