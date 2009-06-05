// OpenSCReader.cpp: implementation of the COpenSCReader class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenSCReader.h"
#include "eidlib.h"
#include "eiderrors.h"
#include "../beidcommon/config.h"
#include "../beidcommon/TLVBuffer.h"
#include <opensc/opensc.h>
#include <opensc/pkcs15.h>
#include <string.h>
#include <stdlib.h>


struct pcsc_slot_data {	unsigned long pcsc_card; }; /* comes from reader-pcsc.c */
#define GET_SLOT_DATA(r) ((struct pcsc_slot_data *) (r)->drv_data)


using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenSCReader::COpenSCReader()
{
    m_pCtx = NULL;
    m_pReader = NULL;
    m_pCard = NULL;
    m_p15Card = NULL;
}

COpenSCReader::~COpenSCReader()
{
    CleanUp();
}

void COpenSCReader::CleanUp()
{
}

const char *COpenSCReader::GetConnectedName()
{
    if(m_pReader != NULL && strlen(m_pReader->name) > 0)
    {
        return m_pReader->name;
    }
    return NULL;
}

long COpenSCReader::Connect(const char *pszReaderName, BEID_Status *ptStatus)
{
    long lRet = SC_ERROR_NO_READERS_FOUND;
    struct sc_reader *pReader = NULL;

    if(m_pCtx != NULL)
    {
        if(pszReaderName == NULL || strlen(pszReaderName) == 0)
        {
            // Find a reader
            for(int i = 0; i < m_pCtx->reader_count; i++)
            {
                pReader = m_pCtx->reader[i];
                if(pReader != NULL)
                {
                    lRet = sc_connect_card(pReader, 0, &m_pCard);
                    if(SC_NO_ERROR == lRet)
                    {
                        m_pReader = pReader;
                        if(0 == strcmp(m_pReader->driver->short_name, "pcsc"))
                        {
                            struct pcsc_slot_data *pslot = GET_SLOT_DATA(m_pCard->slot);
	                        if(pslot != NULL)
                            {
                                m_hCard = pslot->pcsc_card;
                            }
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            for(int i = 0; i < m_pCtx->reader_count; i++)
            {
                pReader = m_pCtx->reader[i];
                if(0 == strcmp(pReader->name, pszReaderName))
                {
                    break;
                }
                pReader = NULL;
            }
            if(pReader != NULL)
            {
                lRet = sc_connect_card(pReader, 0, &m_pCard);
                if(SC_NO_ERROR == lRet)
                {
                    m_pReader = pReader;
                }
            }
        }
        if(m_pCard != NULL)
        {
            if(SC_NO_ERROR != sc_pkcs15_bind(m_pCard, &m_p15Card))
            {
                m_p15Card = NULL;
            }
        }
    }

    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::Disconnect(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pCard != NULL)
    {
        if(m_p15Card != NULL)
        {
            sc_pkcs15_unbind(m_p15Card);
            m_p15Card = NULL;
        }
        lRet = sc_disconnect_card(m_pCard, 0);
        m_pCard = NULL;
        m_pReader = NULL;
        m_hCard = 0;
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::EstablishContext(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    lRet = sc_establish_context(&m_pCtx, "eidlib");
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::ReleaseContext(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
	if (m_pCtx != NULL)
    {
		lRet = sc_release_context(m_pCtx);
        m_pCtx = NULL;
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::BeginTransaction(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pCard != NULL)
    {
        lRet = sc_lock(m_pCard);
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::EndTransaction(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pCard != NULL)
    {
        lRet = sc_unlock(m_pCard);
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::ReadFile(const unsigned char *ucFile, int iFileLen, unsigned char *pucOutput, unsigned long *pulOutLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_p15Card != NULL)
    {
        struct sc_path tPath ={0};
   		sc_append_path_id(&tPath, ucFile, iFileLen);
        tPath.count = -1;
        tPath.type = SC_PATH_TYPE_PATH;
        size_t outLen = *pulOutLen;
        if(SC_NO_ERROR != (lRet = sc_pkcs15_read_file2(m_p15Card, &tPath, pucOutput, &outLen, NULL)))
        {
            *pulOutLen = 0;
        }
        else
        {
            *pulOutLen = outLen;
        }
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::SelectFile(const unsigned char *ucFile, int iFileLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        struct sc_path tPath ={0};
   		sc_append_path_id(&tPath, ucFile, iFileLen);
        lRet = sc_select_file(m_pCard, &tPath, NULL);
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::ReadBinary(unsigned char *pszOutput, unsigned long *pulOutLen, BEID_Status *ptStatus, int iOffset /* 0 */, int iCount /* MAX_FILE_SIZE */)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        unsigned char szBuffer[MAX_FILE_SIZE] = {0};

        long lBytesRead = sc_read_binary(m_pCard, iOffset, szBuffer, iCount, 0);
        if(lBytesRead > 0)
        {
            memcpy(pszOutput, szBuffer, lBytesRead);
            *pulOutLen = lBytesRead;
        }
        else
        {
            ConvertOpenSCError(lBytesRead, ptStatus);
            lRet = lBytesRead;
        }
    }
    return lRet;
}

long COpenSCReader::Transmit(CCommandAPDU *pCmd, CResponseAPDU *pResp)
{
    long lRet = SC_NO_ERROR;
    if(m_pCard != NULL)
    {
        struct sc_apdu tApdu = {0};
	    sc_format_apdu(m_pCard, &tApdu, pCmd->GetCase(), pCmd->GetInstructionByte(), 
                                pCmd->GetParameter1Byte(), pCmd->GetParameter2Byte());
        tApdu.cla = pCmd->GetClassByte(); 
	    tApdu.le = pCmd->GetLeByte();
	    tApdu.lc = pCmd->GetInputDataBytesLen();
        tApdu.data = pCmd->GetInputDataBytes();
	    tApdu.datalen = pCmd->GetInputDataBytesLen() ;
        tApdu.resp = pResp->GetBytes();
	    tApdu.resplen = pResp->GetLength();
        if(SC_NO_ERROR == (lRet = sc_transmit_apdu(m_pCard, &tApdu)))
        {
            // Set returned length and fill SW bytes
            pResp->SetLength(tApdu.resplen+2);
            pResp->SetByte(tApdu.sw1, tApdu.resplen);
            pResp->SetByte(tApdu.sw2, tApdu.resplen + 1);
        }
    }
    return lRet;
}

long COpenSCReader::Transmit(const unsigned char *pucSend, int iSendLen, unsigned char *pucRecv, unsigned long *pulRecvLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pCard != NULL)
    {
        unsigned char ucBuf[SC_MAX_APDU_BUFFER_SIZE] = {0};
        int iLen = iSendLen;
        struct sc_apdu tApdu = {0};
		tApdu.cla = *pucSend++;
		tApdu.ins = *pucSend++;
		tApdu.p1 = *pucSend++;
		tApdu.p2 = *pucSend++;
		tApdu.resp = pucRecv;
		tApdu.resplen = *pulRecvLen;
		iLen -= 4;
		if (iLen > 1) 
        {
			tApdu.lc = *pucSend++;
			iLen--;
			memcpy(ucBuf, pucSend, tApdu.lc);
			tApdu.data = ucBuf;
			tApdu.datalen = tApdu.lc;
			iLen -= tApdu.lc;
			if (iLen) 
            {
				tApdu.le = *pucSend++;
				if (tApdu.le == 0)
					tApdu.le = 256;
				iLen--;
				tApdu.cse = SC_APDU_CASE_4_SHORT;
			} 
            else
            {
				tApdu.cse = SC_APDU_CASE_3_SHORT;
            }
		} 
        else if (iLen == 1) 
        {
			tApdu.le = *pucSend++;
			if (tApdu.le == 0)
				tApdu.le = 256;
			iLen--;
			tApdu.cse = SC_APDU_CASE_2_SHORT;
		} 
        else
        {
			tApdu.cse = SC_APDU_CASE_1;
        }

        if(SC_NO_ERROR == (lRet = sc_transmit_apdu(m_pCard, &tApdu)))
        {
            *pulRecvLen = tApdu.resplen + 2;
            pucRecv[tApdu.resplen] = tApdu.sw1;
            pucRecv[tApdu.resplen + 1] = tApdu.sw2;
        }
    }
    ConvertOpenSCError(lRet, ptStatus);
    return lRet;
}

long COpenSCReader::SelectFile(const unsigned char *ucFile, int iFileLen, unsigned char ucP1, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        CCommandAPDU oCmd(0x00, 0xA4, ucP1, 0x0C, (unsigned char *)ucFile, iFileLen);
        CResponseAPDU oResp(2);
        lRet = Transmit(&oCmd, &oResp);
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::GetCardData(unsigned char *pucOutput, unsigned long *pulOutLen, bool bSigned, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        if(!bSigned)
        {
            memcpy(pucOutput, ((struct belpic_priv_data *) (m_pCard->drv_data))->carddata, BEID_MAX_CARD_DATA_LEN);
            *pulOutLen = BEID_MAX_CARD_DATA_LEN;
        }
        else
        {
            unsigned char ucSize = 0x9C;
            CCommandAPDU oCmd(0x80, 0xE4, bSigned ? 0x02 : 0x00, 0x00, ucSize);
            CResponseAPDU oRespTotal(ucSize + 2);
            lRet = Transmit(&oCmd, &oRespTotal);
            if(SC_NO_ERROR == lRet && CheckSW(ptStatus))
            {
                memcpy(pucOutput, oRespTotal.GetBytes(), oRespTotal.GetRespLength());
                *pulOutLen = oRespTotal.GetRespLength();
            }
            else
            {
                *pulOutLen = 0;
            }
        }
        if(*pulOutLen > 0)
        {
            ConvertOpenSCError(lRet, ptStatus);
        }
        else
        {
            ptStatus->general = BEID_E_CARD;
        }
    }
    return lRet;
}

long COpenSCReader::GetPINStatus(unsigned char *pucOutput, unsigned long *pulOutLen, unsigned char ucPinReference, bool bSigned, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        BOOL bContinue = TRUE;
        unsigned char ucChopSize = bSigned ? 0x81 : 0x01;
        CCommandAPDU oCmd(0x80, 0xEA, bSigned ? 0x02 : 0x00, ucPinReference, ucChopSize);
        CResponseAPDU oRespTotal(ucChopSize + 2);
        lRet = Transmit(&oCmd, &oRespTotal);
        if(SC_NO_ERROR == lRet && CheckSW(ptStatus))
        {
            memcpy(pucOutput, oRespTotal.GetBytes(), oRespTotal.GetRespLength());
            *pulOutLen = oRespTotal.GetRespLength();
        }
        else
        {
            *pulOutLen = 0;
        }

        if(*pulOutLen > 0)
        {
            ConvertOpenSCError(lRet, ptStatus);
        }
        else
        {
            ptStatus->general = BEID_E_CARD;
        }
    }
    return lRet;
}

long COpenSCReader::UpdateBinary(unsigned char *pucInput, unsigned long ulInLen, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_pCard != NULL)
    {
        lRet = sc_update_binary(m_pCard, 0, pucInput, ulInLen, 0);
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    unsigned char ucPinReference = pPinData->id; 
    *piTriesLeft = -1;
    memset(ptStatus->cardSW, 0, 2);
    
    if(m_p15Card != NULL)
    {
        if(SC_NO_ERROR == (lRet = BeginTransaction(ptStatus)))
        {
            struct sc_pkcs15_pin_info *pPinInfo = FindPIN(ucPinReference);
            if(pPinInfo != NULL)
            {
                int iSSO = m_pCard->ctx->allow_sso;
                m_pCard->ctx->allow_sso = 0;
                lRet = sc_pkcs15_verify_pin(m_p15Card, pPinInfo, (const unsigned char *)pszPin, pszPin != NULL ? strlen(pszPin) : 0);
                *piTriesLeft = pPinInfo->tries_left;
                m_pCard->ctx->allow_sso = iSSO;
            }

            EndTransaction(ptStatus);
        }
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    unsigned char ucPinReference = pPinData->id; 
    *piTriesLeft = -1;
    memset(ptStatus->cardSW, 0, 2);
    
    if(m_p15Card != NULL)
    {
        if(SC_NO_ERROR == (lRet = BeginTransaction(ptStatus)))
        {
            struct sc_pkcs15_pin_info *pPinInfo = FindPIN(ucPinReference);
            if(pPinInfo != NULL)
            {
                lRet = sc_pkcs15_change_pin(m_p15Card, pPinInfo, (const unsigned char *)pszOldPin, pszOldPin != NULL ? strlen(pszOldPin) : 0,
                        (const unsigned char *)pszNewPin, pszNewPin != NULL ? strlen(pszNewPin) : 0);
                *piTriesLeft = pPinInfo->tries_left;
            }        
            EndTransaction(ptStatus);
        }
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

struct sc_pkcs15_pin_info *COpenSCReader::FindPIN(unsigned char ucPinReference)
{
    int iNumber = 0;
    struct sc_pkcs15_pin_info *pPINRet = NULL;
    struct sc_pkcs15_object *pObjs[32];
	iNumber = sc_pkcs15_get_objects(m_p15Card, SC_PKCS15_TYPE_AUTH_PIN, pObjs, 32);
    for (int i = 0; i < iNumber; i++) 
    {
        pPINRet = (struct sc_pkcs15_pin_info *)pObjs[i]->data;
        if(pPINRet->reference == ucPinReference)
        {
            break;
        }
        pPINRet = NULL;
    }
    return pPINRet;
}

void *COpenSCReader::GetCard()
{
    return m_pCard;
}

void *COpenSCReader::GetP15Card()
{
    return m_p15Card;
}

void COpenSCReader::FlushCache()
{
    CleanUp();
    if(m_p15Card != NULL)
    {
        sc_pkcs15_cache_clear(m_p15Card);
    }
}

void COpenSCReader::SetRawData(BEID_Raw *pRawData)
{

}

long COpenSCReader::GetChallengeResponse(unsigned char *pucOutputCh, unsigned long *pulOutLenCh, 
                                         unsigned char *pucOutputResp, unsigned long *pulOutLenResp, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    
    if(m_pCard != NULL)
    {
        // Generate Challenge
        CCommandAPDU oCmd(0x00, 0x84, 0x00, 0x00, BEID_MAX_CHALLENGE_LEN);
        CResponseAPDU oResp( BEID_MAX_CHALLENGE_LEN + 2);
        lRet = Transmit(&oCmd, &oResp);
        ptStatus->cardSW[0] = oResp.GetSW1();
        ptStatus->cardSW[1] = oResp.GetSW2();
        if(SC_NO_ERROR == lRet && CheckSW(ptStatus))
        {
            CByteArray oTemp;
            oTemp.Append(0x94);
            oTemp.Append(oResp.GetRespLength());
            oTemp.Append(oResp.GetBytes(), oResp.GetRespLength()); 
            CCommandAPDU oCmdResp(0x00, 0x88, 0x02, 0x81, oTemp.GetData(), oTemp.GetSize(), BEID_MAX_RESPONSE_LEN);
            CResponseAPDU oRespCh(BEID_MAX_RESPONSE_LEN + 2);
            lRet = Transmit(&oCmdResp, &oRespCh);
            ptStatus->cardSW[0] = oRespCh.GetSW1();
            ptStatus->cardSW[1] = oRespCh.GetSW2();            
            if(SC_NO_ERROR == lRet && CheckSW(ptStatus))
            {
                memcpy(pucOutputCh, oResp.GetBytes(), oResp.GetRespLength());
                *pulOutLenCh = oResp.GetRespLength();
                memcpy(pucOutputResp, oRespCh.GetBytes(), oRespCh.GetRespLength());
                *pulOutLenResp = oRespCh.GetRespLength();
            }
            else
            {
                ptStatus->general = BEID_E_CARD;
            }
        }
        else
        {
            ptStatus->general = BEID_E_CARD;
        }
    }

    if(lRet != SC_NO_ERROR)
    {
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}

long COpenSCReader::GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ptStatus->cardSW, 0, 2);
    if(m_p15Card != NULL)
    {          
        if(SC_NO_ERROR == (lRet = BeginTransaction(ptStatus)))
        {
            // List pins
            int iNumber = 0;
            struct sc_pkcs15_object *pObjs[32];
            struct sc_pkcs15_pin_info *pPinInfo = NULL;
	        iNumber = sc_pkcs15_get_objects(m_p15Card, SC_PKCS15_TYPE_AUTH_PIN, pObjs, 32);
            pPins->pinsLength = iNumber;
            for (int i = 0; i < iNumber; i++) 
            {
                pPinInfo = (struct sc_pkcs15_pin_info *)pObjs[i]->data;
                pPins->pins[i].id = pPinInfo->reference;
                pPins->pins[i].pinType = pPinInfo->type;
                char *pszUsage = CTLVBuffer::Hexify(pPinInfo->auth_id.value, pPinInfo->auth_id.len);
                pPins->pins[i].usageCode = atol(pszUsage);
                pPins->pins[i].flags = pPinInfo->flags;
                strcpy(pPins->pins[i].label, pObjs[i]->label);
                pPins->pins[i].triesLeft = pPinInfo->tries_left;
                delete [] pszUsage;
                pPinInfo = NULL;
            }        
            EndTransaction(ptStatus);
        }
        ConvertOpenSCError(lRet, ptStatus);
    }
    return lRet;
}


