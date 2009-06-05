// pinpad.cpp: implementation of the CPinPad class.
//
//////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
typedef char * LPCTSTR;
#endif

#include "pinpad.h"
#include <qlibrary.h>
#include "TLVBuffer.h"
#include "scr.h"
#include "funcs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SCR_INIT_ID          100
#define SCR_VERIFY_ID      101
#define SCR_CHANGE_ID    102
#define SCR_CARD_HANDLE          999

#ifndef MAXIMUM_SMARTCARD_READERS
    #define MAXIMUM_SMARTCARD_READERS 10
#endif

typedef long (*SCR_INIT_PROC)(LPCTSTR, DWORD, SCR_SupportConstants*);
typedef long (*SCR_VERIFYPIN_PROC)(const SCR_Card*, BYTE, const SCR_PinUsage*, const SCR_Application*, BYTE *);
typedef long (*SCR_CHANGEPIN_PROC)(const SCR_Card*, BYTE, const SCR_Application*, BYTE *);

using namespace eidcommon;

CPinPad::CPinPad()
{
    m_pPinPadDlls = new PinPadObject[MAXIMUM_SMARTCARD_READERS];
    memset(m_pPinPadDlls, 0, sizeof(PinPadObject) * MAXIMUM_SMARTCARD_READERS);
}

CPinPad::~CPinPad()
{
    if(m_pPinPadDlls != NULL)
    {
        for(int i = 0; i < MAXIMUM_SMARTCARD_READERS; i++)
        {
            if(m_pPinPadDlls[i].pDll  != NULL)
            {
                delete m_pPinPadDlls[i].pDll;
            }
        }
        delete m_pPinPadDlls;
        m_pPinPadDlls = NULL;
    }
}

long CPinPad::HandlePinPad(const void *lpInBuffer, unsigned long nInBufferSize, void *lpOutBuffer, unsigned long *lpBytesReturned, SCARDHANDLE hReal /* 0 */)
{
    long lRet = SCARD_F_INTERNAL_ERROR;
    *lpBytesReturned = 0;

    // Parse TLV
    CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV((unsigned char *)lpInBuffer, nInBufferSize);

    long lOp = 0;
    char szPinPad[256] = {0};
    oTLVBuffer.FillLongData(0x01, &lOp); // Function ID 
    oTLVBuffer.FillASCIIData(0x02, szPinPad); // PinPad Dll

    bool bFound = false;
    int i = 0;
    int iIndex = -1;
    for(i = 0; i < MAXIMUM_SMARTCARD_READERS; i++)
    {
        if(0 == strcmp(m_pPinPadDlls[i].szPinPadDll, szPinPad))
        {
            bFound = true;
            break;
        }
        if(m_pPinPadDlls[i].pDll == NULL && iIndex == -1)
        {
            iIndex = i;
            break;
        }
    }
    QLibrary *pPinPadLoader = NULL;
    if (bFound)
    {
        pPinPadLoader = m_pPinPadDlls[i].pDll;
    }
    else
    {
        pPinPadLoader = new QLibrary(szPinPad);
        pPinPadLoader->load();
        m_pPinPadDlls[iIndex].pDll = pPinPadLoader;
        strcpy(m_pPinPadDlls[iIndex].szPinPadDll, szPinPad);
    }

    if(pPinPadLoader && !pPinPadLoader->isLoaded())
    {
        return -5000;
    }

    if(pPinPadLoader && pPinPadLoader->isLoaded())
    {
        if(SCR_INIT_ID == lOp)
        {
            char szReader[256] = {0};
            long lVersion = 0;
            oTLVBuffer.FillASCIIData(0x03, szReader); // Reader Name 
            oTLVBuffer.FillLongData(0x04, &lVersion); // Version
            SCR_SupportConstants supported;
            SCR_INIT_PROC scr_init = (SCR_INIT_PROC)pPinPadLoader->resolve("SCR_Init");
            if(scr_init != NULL)
            {
                lRet = scr_init(szReader, lVersion, &supported);
                ((unsigned char *)lpOutBuffer)[0] = supported;
                *lpBytesReturned = 1;
            }
        }
        else if(SCR_VERIFY_ID == lOp || SCR_CHANGE_ID == lOp)
        {
            unsigned long ulLen = 0;
            SCR_Card card = {0};
            SCR_PinUsage usage;
            memset(&usage, 0, sizeof(SCR_PinUsage));
            SCR_Application application = {0};
            unsigned char pinID = 0;
            unsigned char *pCardData = NULL;
            char *pszLanguage = NULL;
            char *pszShortUsage = NULL;
            char *pszLongUsage = NULL;
            unsigned char *pAppData = NULL;
            char *pszShortApp = NULL;
            char *pszLongApp = NULL;
            unsigned char ucRet[2] = {0};

            oTLVBuffer.FillLongData(0x03, (long *)&card.hCard); // SCR_Card Handle 
            // We need real SCARD HANDLE here
            if(hReal > 0)
            {
                card.hCard = hReal;
            }
            pszLanguage = new char[256];
            memset(pszLanguage, 0, 256);
            oTLVBuffer.FillASCIIData(0x04, pszLanguage); // SCR_Card language
            card.language = pszLanguage;
            if(oTLVBuffer.GetTagData(0x05)) // SCR_Card id
            {
                pCardData = new unsigned char[256];
                memset(pCardData, 0, 256);
                oTLVBuffer.FillBinaryData(0x05, pCardData, &ulLen);  
                card.id.data = pCardData;
                card.id.length = ulLen;
            }
            oTLVBuffer.FillBinaryData(0x06, &pinID, &ulLen); // PinID  
            if(oTLVBuffer.GetTagData(0x07)) // SCR_PinUsage code
            {
                oTLVBuffer.FillLongData(0x07, (long *)&usage.code);
                if(oTLVBuffer.GetTagData(0x08)) // SCR_PinUsage shortstring
                {
                    pszShortUsage = new char[256];
                    memset(pszShortUsage, 0, 256);
                    oTLVBuffer.FillASCIIData(0x08, pszShortUsage);
                    usage.shortString = pszShortUsage;
                }
                if(oTLVBuffer.GetTagData(0x09)) // SCR_PinUsage longstring
                {
                    pszLongUsage = new char[256];
                    memset(pszLongUsage, 0, 256);
                    oTLVBuffer.FillASCIIData(0x09, pszLongUsage);
                    usage.longString = pszLongUsage;
                }
            }
            if(oTLVBuffer.GetTagData(0x0A)) // SCR_Application id
            {
                pAppData = new unsigned char[256];
                memset(pAppData, 0, 256);
                oTLVBuffer.FillBinaryData(0x0A, pAppData, &ulLen);  
                application.id.data = pAppData;
                application.id.length = ulLen;
            }
            if(oTLVBuffer.GetTagData(0x0B)) // SCR_Application shortstring
            {
                pszShortApp = new char[256];
                memset(pszShortApp, 0, 256);
                oTLVBuffer.FillASCIIData(0x0B, pszShortApp);
                application.shortString = pszShortApp;

            }
            if(oTLVBuffer.GetTagData(0x0C)) // SCR_Application longstring
            {
                pszLongApp = new char[256];
                memset(pszLongApp, 0, 256);
                oTLVBuffer.FillASCIIData(0x0C, pszLongApp);
                application.longString = pszLongApp;
            }
            if(SCR_VERIFY_ID == lOp)
            {
                SCR_VERIFYPIN_PROC scr_verify = (SCR_VERIFYPIN_PROC)pPinPadLoader->resolve("SCR_VerifyPIN");
                if(scr_verify != NULL)
                {
                    lRet = scr_verify(&card, pinID, &usage, &application, ucRet);
                    memcpy(lpOutBuffer, ucRet, 2);
                    *lpBytesReturned = 2;
                }
            }
            else if (SCR_CHANGE_ID == lOp)
            {
                SCR_CHANGEPIN_PROC scr_change = (SCR_CHANGEPIN_PROC)pPinPadLoader->resolve("SCR_ChangePIN");
                if(scr_change != NULL)
                {
                    lRet = scr_change(&card, pinID, &application, ucRet);
                    memcpy(lpOutBuffer, ucRet, 2);
                    *lpBytesReturned = 2;
                }
            }
            if(pCardData)
                delete [] pCardData;
            if(pszLanguage)
                delete [] pszLanguage;
            if(pszShortUsage)
                delete [] pszShortUsage;
            if(pszLongUsage)
                delete [] pszLongUsage;
            if(pAppData)
                delete [] pAppData;
            if(pszShortApp)
                delete [] pszShortApp;
            if(pszLongApp)
                delete [] pszLongApp;
        }
    }
    return lRet;
}
