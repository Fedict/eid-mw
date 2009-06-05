// BEIDApp.cpp mentation of the CBEIDApp class.
//
//////////////////////////////////////////////////////////////////////
#include <qapplication.h>
#include <qmessagebox.h>
#include <qfileinfo.h> 
#include <qlocale.h> 
#include <qdatetime.h> 
#include "../beidcommon/beidconst.h"

//#define _TIMINGREAD
#ifdef _TIMINGREAD
    QTime t;
#endif

#include "BEIDApp.h"
#include "OpenSCReader.h"
#include "VirtualReader.h"
#include "CommandAPDU.h"
#include "CertifManager.h"
#include "Verify.h"
#include "wxwalldlg.h"
#include "AutoUpdate.h"
#include <opensc/pkcs15.h>
#include "eiderrors.h"
#include "eidliblang.h"

#ifdef _WIN32
    #include "processapi.h"
#else
    #include <unistd.h>
#endif

#ifdef __LOGGING_CXX
    #include <log4cplus/logger.h>
    #include <log4cplus/fileappender.h>
    #include <log4cplus/layout.h>
    #define MYLOG4CPLUS_DEBUG(message) { \
        LOG4CPLUS_DEBUG(m_Logger, message) \
    }
const unsigned char std::__digit_val_table[128] = 
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,10,11,12,13,14,15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

    #ifdef _DEBUG
            #pragma comment( lib, "log4cplusd.lib" )
    #else
            #pragma comment( lib, "log4cplus.lib" )
    #endif
#else
    #define MYLOG4CPLUS_DEBUG(message)
#endif


#ifndef SC_NO_ERROR
    #define SC_NO_ERROR 0
#endif


#ifdef __LOGGING_CXX
    using namespace log4cplus;
    Logger m_Logger(Logger::getInstance("BEIDLIB"));
#endif

using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QMutex CBEIDApp::m_cs(true);
int CBEIDApp::m_iLang(0);

CBEIDApp::CBEIDApp(char *pszReaderName, int iOCSP, int iCRL)
{
    if(pszReaderName != NULL)
    {
        m_szReaderName = pszReaderName;
    }
    m_iOCSP = iOCSP;
    m_iCRL = iCRL;
    m_pReader = NULL;
    m_pCertifManager = NULL;
    m_bShowRootWarning = true;
    m_bAllowTestRoot = false;
    m_pAutoUpdate = NULL;

#ifdef __LOGGING_CXX
    SharedAppenderPtr fileAppend(new RollingFileAppender("C:\\beidlib.log"));
    fileAppend->setName("FileAppend");
    fileAppend->setLayout( std::auto_ptr<Layout>(new PatternLayout("%D{%d/%m/%Y %H:%M:%S} %-5p %c - %m%n")));
    Logger::getRoot().addAppender(fileAppend);
    int iLevel = getLogLevelManager().fromString("DEBUG");
    Logger::getRoot().setLogLevel(iLevel);
#endif

    CConfig oConfig;
    oConfig.Load();
    m_bAllowTestRoot = oConfig.AllowTestRoot();
    FillProcessName();   
    
    // Locale
    MapLanguage();
    // AutoUpdate
     if(oConfig.AllowAutoUpdate() )
     {
        m_pAutoUpdate = new CAutoUpdate();
     }
}   

CBEIDApp::~CBEIDApp()
{
    if(m_pAutoUpdate != NULL)
    {
        m_pAutoUpdate->wait();
        delete m_pAutoUpdate;
        m_pAutoUpdate = NULL;
    }
}

CCardReader *CBEIDApp::CreateReader()
{
    if(m_pReader != NULL)
    {
        delete m_pReader;
        m_pReader= NULL;
    }
    if("VIRTUAL" == m_szReaderName)
    {
        m_pReader = new CVirtualReader();
    }
    else
    {
        m_pReader = new COpenSCReader();
    }
    return m_pReader;
}

BOOL CBEIDApp::Init(long *plHandle, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    BOOL bRet = FALSE;
    *plHandle = 0;
    m_bShowRootWarning = true;
    MYLOG4CPLUS_DEBUG("*** Begin Init ***");

    CreateReader();
    if(m_pReader != NULL)
    {
        if(SC_NO_ERROR == m_pReader->EstablishContext(ptStatus))
        {
            MYLOG4CPLUS_DEBUG("EstablishContext OK");
            if(SC_NO_ERROR == m_pReader->Connect(m_szReaderName.c_str(), ptStatus))
            {
                MYLOG4CPLUS_DEBUG("Connect OK");
                *plHandle = m_pReader->GetHandle();
                m_pCertifManager = new CCertifManager();
                m_pCertifManager->SetCardReader(m_pReader);
                CVerify::SetCertifManager(m_pCertifManager); 
                bRet = TRUE;
            }
        }
    }
    MYLOG4CPLUS_DEBUG("*** End Init ***");
    if(bRet && m_pAutoUpdate != NULL)
    {
        BEID_VersionInfo tVersionInfo = {0};
        BEID_Status tStatus = {0};
        if(GetVersionInfo(&tVersionInfo, FALSE, NULL, &tStatus))
        {
            CheckVersion(&tVersionInfo);
        }
        m_pAutoUpdate->start();
    }
    return bRet;
}

BOOL CBEIDApp::Exit(BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    BOOL bRet = TRUE;
    MYLOG4CPLUS_DEBUG("*** Begin Exit ***");

    if(m_pCertifManager != NULL)
    {
        m_pCertifManager->Cleanup(); 
        delete m_pCertifManager;
        m_pCertifManager = NULL;
        CVerify::SetCertifManager(NULL); 
    }

    if(m_pReader != NULL)
    {
        if(SC_NO_ERROR == m_pReader->Disconnect(ptStatus))
        {	
            m_pReader->ReleaseContext(ptStatus);
            delete m_pReader;
            m_pReader= NULL;
        }
    }
    m_bShowRootWarning = true;
    MYLOG4CPLUS_DEBUG("*** End Exit ***");
    return bRet;
}

bool CBEIDApp::TestSignature(long lSignatureCheck)
{
    bool bRet = false;
    if(lSignatureCheck == BEID_SIGNATURE_VALID)
    {
        bRet = true;
        MYLOG4CPLUS_DEBUG("Valid Signature");
    }
    else if(lSignatureCheck == BEID_SIGNATURE_VALID_WRONG_RRNCERT)
    {
        MYLOG4CPLUS_DEBUG("Valid Signature But Wrong RN Cert");
        if(m_bAllowTestRoot)
        {
            MYLOG4CPLUS_DEBUG("AllowTestRoot Enabled");
            // Warning
            if(m_bShowRootWarning)
            {
                if(QMessageBox::Yes == AskTestCard())
                {
                    bRet = true;
                    m_bShowRootWarning = false;
                    MYLOG4CPLUS_DEBUG("Test Card dialogbox accepted");
                }
            }
            else
            {
                bRet = true;
            }
        }
    }
    return bRet;
}

BOOL CBEIDApp::GetID(BEID_ID_Data *ptData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    MYLOG4CPLUS_DEBUG("*** Begin GetID ***");
    // Firewall Check
    if(!FireWallAllowed("id"))
    {
        ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
        MYLOG4CPLUS_DEBUG("Firewall blocked request");
        MYLOG4CPLUS_DEBUG("*** Exit GetID ***");
        return FALSE;
    }

    if(m_pReader != NULL)
    {
        unsigned char ucID[BEID_MAX_RAW_ID_LEN] = {0};
        unsigned long ulLen = BEID_MAX_RAW_ID_LEN;
        unsigned char ucFileID[6] = {0};
        memcpy(ucFileID, g_ucMF, 2);
        memcpy(ucFileID + 2, g_ucDFID, 2);
        memcpy(ucFileID + 4, g_ucID, 2);

#ifdef _TIMINGREAD
            t.start();
#endif
        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileID, sizeof(ucFileID), ucID, &ulLen, ptStatus)))
        {
            bool bFill = false;
            if(SC_NO_ERROR == (lRet = ReadCertificates(ptCertifCheck, ptStatus)))
            {
                MYLOG4CPLUS_DEBUG("ReadCertificates OK");
                lRet = VerifySignatureID(ucID, ulLen, ptCertifCheck, ptStatus, pRawData);
                bFill = TestSignature(ptCertifCheck->signatureCheck);
            }
            if(!bFill)
            {
                ptStatus->general = BEID_E_INVALID_ROOT_CERT;
                lRet = -1;
            }
            else
            {
                if (SC_NO_ERROR == VerifyCertificates(ptCertifCheck))
                {
                    MYLOG4CPLUS_DEBUG("VerifyCertificates OK");
                    if(pRawData != NULL)
                    {
                        memcpy(pRawData->idData, ucID, ulLen);
                        pRawData->idLength = ulLen;
                    }
                    // Parse TLV
                    CTLVBuffer oTLVBuffer;
                    oTLVBuffer.ParseTLV(ucID, ulLen);
                    FillIDData(oTLVBuffer, ptData);
                    m_pCertifManager->FillCertifs(ptCertifCheck);
                    MYLOG4CPLUS_DEBUG("ID retrieved and validated OK");
                }
                else
                {
                    ptStatus->general = BEID_E_VALIDATION;
                    lRet = -1;
                }
            }
        }
    }

    MYLOG4CPLUS_DEBUG("*** Exit GetID ***");
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::GetAddress(BEID_Address *ptAddress, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(!FireWallAllowed("address"))
    {
        ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
        return FALSE;
    }
    if(m_pReader != NULL)
    {        
        unsigned char ucAddress[BEID_MAX_RAW_ADDRESS_LEN] = {0};
        unsigned long ulLen = BEID_MAX_RAW_ADDRESS_LEN;
        unsigned char ucFileID[6] = {0};
        memcpy(ucFileID, g_ucMF, 2);
        memcpy(ucFileID + 2, g_ucDFID, 2);
        memcpy(ucFileID + 4, g_ucADDR, 2);

        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileID, sizeof(ucFileID), ucAddress, &ulLen, ptStatus)))
        {
            bool bFill = false;
            CTLVBuffer oTLVBuffer;
            if(SC_NO_ERROR == (lRet = ReadCertificates(ptCertifCheck, ptStatus)))
            {
                // Parse TLV
                oTLVBuffer.ParseTLV(ucAddress, ulLen);
                unsigned long ulRealLen = oTLVBuffer.GetLengthForSignature();
                lRet =VerifySignatureAddress(ucAddress, ulRealLen, ptCertifCheck, ptStatus, pRawData);
                bFill = TestSignature(ptCertifCheck->signatureCheck);
            }
            if(!bFill)
            {
                ptStatus->general = BEID_E_INVALID_ROOT_CERT;
                lRet = -1;
            }
            else
            {
                if(SC_NO_ERROR == VerifyCertificates(ptCertifCheck))
                {
                    if(pRawData != NULL)
                    {
                        memcpy(pRawData->addrData, ucAddress, ulLen);
                        pRawData->addrLength = ulLen;
                    }

                    oTLVBuffer.ParseTLV(ucAddress, ulLen);
                    FillAddressData(oTLVBuffer, ptAddress);
                    m_pCertifManager->FillCertifs(ptCertifCheck);
                }
                else
                {
                    ptStatus->general = BEID_E_VALIDATION;
                    lRet = -1;
                }
            }
        }
    }
    
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::GetPicture(BEID_Bytes *ptPicture, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(!FireWallAllowed("photo"))
    {
        ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
        return FALSE;
    }
    if(m_pReader != NULL)
    {        
        unsigned char ucPicture[BEID_MAX_PICTURE_LEN] = {0};
        unsigned long ulLen = BEID_MAX_PICTURE_LEN;
        unsigned char ucFileID[6] = {0};
        memcpy(ucFileID, g_ucMF, 2);
        memcpy(ucFileID + 2, g_ucDFID, 2);
        memcpy(ucFileID + 4, g_ucPHOTO, 2);

        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileID, sizeof(ucFileID), ucPicture, &ulLen, ptStatus)))
        {
            if(ptPicture->length < ulLen)
            {
                // Buffer too small
                ptStatus->general = BEID_E_INSUFFICIENT_BUFFER;
                return FALSE;
            }
            bool bFill = false;
            if(SC_NO_ERROR == (lRet = ReadCertificates(ptCertifCheck, ptStatus)))
            {
                lRet = VerifyHashPicture(ucPicture, ulLen,  ptCertifCheck, ptStatus, pRawData);
                bFill = TestSignature(ptCertifCheck->signatureCheck);
            }
            if(!bFill)
            {
                ptPicture->length = 0;
                ptStatus->general = BEID_E_INVALID_ROOT_CERT;
                lRet = -1;
            }
            else
            {
                if(SC_NO_ERROR == VerifyCertificates(ptCertifCheck))
                {
                    if(pRawData != NULL)
                    {
                        memcpy(pRawData->pictureData, ucPicture, ulLen);
                        pRawData->pictureLength = ulLen;
                    }

                    memcpy(ptPicture->data, ucPicture, ulLen);
                    ptPicture->length = ulLen;
                    m_pCertifManager->FillCertifs(ptCertifCheck);
                }
                else
                {
                    ptPicture->length = 0;
                    ptStatus->general = BEID_E_VALIDATION;
                    lRet = -1;
                }
            }
        }
    }
    
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

void CBEIDApp::FillIDData(CTLVBuffer & oTLVBuffer, BEID_ID_Data *ptData)
{
    ptData->version = 0;
    oTLVBuffer.FillShortData(0x00, &ptData->version);
    if(ptData->version > EIDLIB_CURRENT_EF_ID)
    {
        if(m_pAutoUpdate != NULL)
        {
            m_pAutoUpdate->RetrieveNewVersion(true);
        }
        return;
    }

    if (ptData->version == 0)
    {
        oTLVBuffer.FillASCIIData(0x01, ptData->cardNumber);
        oTLVBuffer.FillBinaryStringData(0x02, ptData->chipNumber);
        oTLVBuffer.FillDateData(0x03, ptData->validityDateBegin);
        oTLVBuffer.FillDateData(0x04, ptData->validityDateEnd);
        oTLVBuffer.FillUTF8Data(0x05,  ptData->municipality);
        oTLVBuffer.FillASCIIData(0x06, ptData->nationalNumber);
        oTLVBuffer.FillUTF8Data(0x07,  ptData->name);
        oTLVBuffer.FillUTF8Data(0x08,  ptData->firstName1);
        oTLVBuffer.FillUTF8Data(0x09,  ptData->firstName3);
        char szNat[100] = {0};
        oTLVBuffer.FillUTF8Data(0x0A,  szNat);
        oTLVBuffer.FillUTF8Data(0x0B,  ptData->birthLocation);
        oTLVBuffer.FillDateData(0x0C,  ptData->birthDate);
        CTLV *pTagData = NULL;
        if(NULL != (pTagData = oTLVBuffer.GetTagData(0x0D)))
        {
            unsigned char *pData = pTagData->GetData(); 
            if(pData[0] == 'M')
                strcpy(ptData->sex, "M");
            else
                strcpy(ptData->sex, "F");
        }
        else
        {
            oTLVBuffer.FillASCIIData(0x0D, ptData->sex);
        }
        oTLVBuffer.FillUTF8Data(0x0E,  ptData->nobleCondition);
        oTLVBuffer.FillLongData(0x0F, &ptData->documentType);
        if(ptData->documentType < 10)
        {
            strcpy(ptData->nationality, "be");
        }
        ptData->extendedMinority = 0;
        ptData->whiteCane = 0;
        ptData->yellowCane = 0;
        int iStatus = 0;
        oTLVBuffer.FillIntData(0x10, &iStatus);
        switch(iStatus)
        {
            case 1:
                ptData->whiteCane = TRUE;
            break;
            case 2:
                ptData->extendedMinority = TRUE;
            break;
            case 3:
                ptData->whiteCane = TRUE;
                ptData->extendedMinority = TRUE;
            break;
            case 4:
                ptData->yellowCane = TRUE;
            break;
            case 5:
                ptData->yellowCane = TRUE;
                ptData->extendedMinority = TRUE;
            break;
        }
        oTLVBuffer.FillBinaryData(0x11, ptData->hashPhoto);
    }
}

void CBEIDApp::FillAddressData(CTLVBuffer & oTLVBuffer, BEID_Address *ptData)
{
    ptData->version = 0;
    oTLVBuffer.FillShortData(0x00, &ptData->version);
    if(ptData->version > EIDLIB_CURRENT_EF_ADDRESS)
    {
        if(m_pAutoUpdate != NULL)
        {
            m_pAutoUpdate->RetrieveNewVersion(true);
        }
        return;
    }

    if (ptData->version == 0)
    {
        oTLVBuffer.FillUTF8Data(0x01, ptData->street);
        oTLVBuffer.FillASCIIData(0x02, ptData->zip);
        oTLVBuffer.FillUTF8Data(0x03, ptData->municipality);
        strcpy(ptData->country, "be");
    }
}

long CBEIDApp::VerifySignatureID(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    MYLOG4CPLUS_DEBUG("*** Begin VerifySignatureID ***");
    long lRet = SC_NO_ERROR;
    ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
    unsigned char ucSGNID[BEID_MAX_SIGNATURE_LEN] = {0};
    unsigned long ulSGNIDLen = BEID_MAX_SIGNATURE_LEN;
    unsigned char ucFileSGNID[6] = {0};
    memcpy(ucFileSGNID, g_ucMF, 2);
    memcpy(ucFileSGNID + 2, g_ucDFID, 2);
    memcpy(ucFileSGNID + 4, g_ucSIGID, 2);

    if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileSGNID, sizeof(ucFileSGNID), ucSGNID, &ulSGNIDLen, ptStatus)))
    {
#ifdef _TIMINGREAD
        QString str;
        str.sprintf("Time elapsed: %d ms", t.elapsed());
        ::OutputDebugString( str.ascii() );
        ::MessageBox(NULL, str.ascii(), "Time", MB_OK);
#endif
        ptCertifCheck->signatureCheck = CVerify::VerifySignature(pData, ulLenData, ucSGNID, ulSGNIDLen); 
        if(BEID_SIGNATURE_PROCESSING_ERROR == ptCertifCheck->signatureCheck)
        {
             MYLOG4CPLUS_DEBUG("VerifySignature ID Processing Error");
             ptStatus->general = BEID_E_VERIFICATION;
        }
        else
        {
            if(pRawData != NULL)
            {
                memcpy(pRawData->idSigData, ucSGNID, ulSGNIDLen);
                pRawData->idSigLength = ulSGNIDLen;
            }
        }
    }
    MYLOG4CPLUS_DEBUG("*** End VerifySignatureID ***");
    return lRet;
}

long CBEIDApp::VerifySignatureAddress(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    long lRet = SC_NO_ERROR;
    ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
    unsigned char ucSGNAddress[BEID_MAX_SIGNATURE_LEN] = {0};
    unsigned long ulSGNAddressLen = BEID_MAX_SIGNATURE_LEN;
    unsigned char ucFileSGNID[6] = {0};
    memcpy(ucFileSGNID, g_ucMF, 2);
    memcpy(ucFileSGNID + 2, g_ucDFID, 2);
    memcpy(ucFileSGNID + 4, g_ucSIGADDR, 2);

    if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileSGNID, sizeof(ucFileSGNID), ucSGNAddress, &ulSGNAddressLen, ptStatus)))
    {
        // Read EF(SGN#ID)
        unsigned char ucSGNID[BEID_MAX_SIGNATURE_LEN] = {0};
        unsigned long ulSGNIDLen = BEID_MAX_SIGNATURE_LEN;
        unsigned char ucFileSGNID[6] = {0};
        memcpy(ucFileSGNID, g_ucMF, 2);
        memcpy(ucFileSGNID + 2, g_ucDFID, 2);
        memcpy(ucFileSGNID + 4, g_ucSIGID, 2);

        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileSGNID, sizeof(ucFileSGNID), ucSGNID, &ulSGNIDLen, ptStatus)))
        {
            unsigned char szRealBuf[BEID_MAX_RAW_ADDRESS_LEN + BEID_MAX_SIGNATURE_LEN] = {0};
            memcpy(szRealBuf, pData, ulLenData);
            memcpy(szRealBuf + ulLenData, ucSGNID, ulSGNIDLen);
            ptCertifCheck->signatureCheck = CVerify::VerifySignature(szRealBuf, ulLenData + ulSGNIDLen, ucSGNAddress, ulSGNAddressLen); 
            if(BEID_SIGNATURE_PROCESSING_ERROR == ptCertifCheck->signatureCheck)
            {
                 ptStatus->general = BEID_E_VERIFICATION;
            }
            else
            {
                if(pRawData != NULL)
                {
                    memcpy(pRawData->addrSigData, ucSGNAddress, ulSGNAddressLen);
                    pRawData->addrSigLength = ulSGNAddressLen;
                }
            }
        }
    }
    return lRet;
}

long CBEIDApp::VerifyHashPicture(unsigned char *pData, unsigned long ulLenData, BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    long lRet = SC_NO_ERROR;
    ptCertifCheck->signatureCheck = BEID_SIGNATURE_INVALID;
    unsigned char ucHash[BEID_MAX_HASH_PICTURE_LEN] = {0};
    lRet = ReadHashData(ucHash, ptStatus);
    if(SC_NO_ERROR == lRet)
    {
        ptCertifCheck->signatureCheck = CVerify::VerifyHash(pData, ulLenData, ucHash); 
        if(BEID_SIGNATURE_PROCESSING_ERROR == ptCertifCheck->signatureCheck)
        {
            ptStatus->general = BEID_E_VERIFICATION;
        }
    }
    return lRet;
}

long CBEIDApp::ReadCertificates(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus, bool bAll /* false */)
{
    long lRet = SC_NO_ERROR;
    MYLOG4CPLUS_DEBUG("*** Begin ReadCertificates ***");

    if(m_pCertifManager != NULL)
    {
        lRet = m_pCertifManager->ReadRNCertif(ptStatus);
        if(SC_NO_ERROR == lRet && (0 < m_iOCSP || 0 < m_iCRL || bAll))
        {
            lRet = m_pCertifManager->ReadCertifsP15(ptStatus);
        }
    }
    MYLOG4CPLUS_DEBUG("*** End ReadCertificates ***");
    return lRet;
}

long CBEIDApp::ReadHashData(unsigned char ucPicHash[20], BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    memset(ucPicHash, 0, sizeof(ucPicHash));
    if(m_pReader != NULL)
    {  
        unsigned char ucID[BEID_MAX_RAW_ID_LEN] = {0};
        unsigned long ulLen = BEID_MAX_RAW_ID_LEN;
        unsigned char ucFileID[6] = {0};
        memcpy(ucFileID, g_ucMF, 2);
        memcpy(ucFileID + 2, g_ucDFID, 2);
        memcpy(ucFileID + 4, g_ucID, 2);

        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileID, sizeof(ucFileID), ucID, &ulLen, ptStatus)))
        {
            // Parse TLV
            CTLVBuffer oTLVBuffer;
            oTLVBuffer.ParseTLV(ucID, ulLen);
            oTLVBuffer.FillBinaryData(0x11, ucPicHash);
        }
    }
    return lRet;
}

BOOL CBEIDApp::BeginTransaction(BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        lRet = m_pReader->BeginTransaction(ptStatus);
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::EndTransaction(BEID_Status *ptStatus)
{
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        lRet = m_pReader->EndTransaction(ptStatus);
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}
        
BOOL CBEIDApp::SelectApplication(BEID_Bytes *ptApplication, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        if(SC_NO_ERROR == (lRet = m_pReader->BeginTransaction(ptStatus)))
        {
            lRet = m_pReader->SelectFile(g_ucMF, sizeof(g_ucMF), 0x02, ptStatus);
            if(lRet == SC_NO_ERROR)
            {
                lRet = m_pReader->SelectFile(ptApplication->data , ptApplication->length, 0x04, ptStatus);
            }
            m_pReader->EndTransaction(ptStatus);
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::FlushCache(BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pCertifManager != NULL)
    {
        m_pCertifManager->Cleanup(); 
    }
    if(m_pReader != NULL)
    {
        m_pReader->FlushCache();
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::SendAPDU(BEID_Bytes *ptCmdAPDU, BEID_Pin *pPinData, BEID_Bytes *ptRespAPDU, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    // Firewall Check
    if(!FireWallAllowed("sendapdu"))
    {
        return FALSE;
    }
    unsigned char ucPinReference = pPinData->id; 
    if(m_pReader != NULL)
    {
        unsigned char ucStatus[2] = {0};
        unsigned long ulRespLen = ptRespAPDU->length;
        lRet = m_pReader->Transmit(ptCmdAPDU->data, ptCmdAPDU->length, ptRespAPDU->data, &ptRespAPDU->length, ptStatus);  
        if(SC_NO_ERROR == lRet)
        {
            // Check for 'Security Status not satisfied' error
            if(ptRespAPDU->length > 1 && ucPinReference > 0x00)
            {
                ucStatus[0] = ptRespAPDU->data[ptRespAPDU->length - 2];
                ucStatus[1] = ptRespAPDU->data[ptRespAPDU->length - 1];
                if(0x69 == ucStatus[0] && 0x82 == ucStatus[1])
                {
                    long iLeft = -1;
                    lRet = VerifyPIN(pPinData, NULL, &iLeft, ptStatus);
                    if(SC_NO_ERROR == lRet)
                    {
                        // Retry command
                        ptRespAPDU->length = ulRespLen;
                        memset(ptRespAPDU->data, 0, ulRespLen);
                        lRet = m_pReader->Transmit(ptCmdAPDU->data, ptCmdAPDU->length, ptRespAPDU->data, &ptRespAPDU->length, ptStatus);  
                    }
                }
            }
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}


BOOL CBEIDApp::GetVersionInfo(BEID_VersionInfo *ptVersionInfo, BOOL bSignature, BEID_Bytes *ptSignedStatus, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        unsigned char ucOutData[BEID_MAX_CARD_DATA_SIG_LEN] = {0};
        unsigned long ulOutLen = sizeof(ucOutData);
        // Get Card Data
        lRet = m_pReader->GetCardData(ucOutData, &ulOutLen, bSignature ? true : false, ptStatus);  
        if(SC_NO_ERROR == lRet && ulOutLen > 0)
        {
            memcpy(ptVersionInfo, ucOutData, BEID_MAX_CARD_DATA_LEN);
            ptVersionInfo->GlobalOSVersion = 0x0100 * ucOutData[22] + ucOutData[23]; 
            ptVersionInfo->AppletVersion = ucOutData[21] == 0x01 ? 0x10 : ucOutData[21];
            if(bSignature)
            {
                if(ptSignedStatus->length < ulOutLen - BEID_MAX_CARD_DATA_LEN)
                {
                    // Buffer too small
                    ptStatus->general = BEID_E_INSUFFICIENT_BUFFER;
                    return FALSE;
                }
                memcpy(ptSignedStatus->data, ucOutData + BEID_MAX_CARD_DATA_LEN, ulOutLen - BEID_MAX_CARD_DATA_LEN);
                ptSignedStatus->length = ulOutLen - BEID_MAX_CARD_DATA_LEN;
            }
            if(pRawData != NULL)
            {
                memcpy(pRawData->cardData, ucOutData, ulOutLen);
                pRawData->cardDataLength = ulOutLen;
            }
            // TokenInfo
            lRet = GetTokenInfo(ptVersionInfo, ptStatus, pRawData);
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

long CBEIDApp::GetTokenInfo(BEID_VersionInfo *ptVersionInfo, BEID_Status *ptStatus, BEID_Raw *pRawData /* NULL */)
{
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        unsigned char ucToken[64] = {0};
        unsigned long ulLen = sizeof(ucToken);
        unsigned char ucFileToken[6] = {0};
        memcpy(ucFileToken, g_ucMF, 2);
        memcpy(ucFileToken + 2, g_ucDFCert, 2);
        memcpy(ucFileToken + 4, g_ucTokenInfo, 2);

        if(SC_NO_ERROR == (lRet = m_pReader->ReadFile(ucFileToken, sizeof(ucFileToken), ucToken, &ulLen, ptStatus)))
        {
            if(ulLen >= TOKENINFO_OFFSET + 4)
            {
                ptVersionInfo->GraphPerso = ucToken[TOKENINFO_OFFSET];
                ptVersionInfo->ElecPerso = ucToken[TOKENINFO_OFFSET+1];
                ptVersionInfo->ElecPersoInterface = ucToken[TOKENINFO_OFFSET+2];
                ptVersionInfo->Reserved = ucToken[TOKENINFO_OFFSET+3];
            }
            if(pRawData != NULL)
            {
                memcpy(pRawData->tokenInfo, ucToken, ulLen);
                pRawData->tokenInfoLength = ulLen;
            }
        }
    }
    return lRet;
}

BOOL CBEIDApp::GetPINStatus(BEID_Pin *pPinData, long *piTriesLeft, BOOL bSignature, BEID_Bytes *ptSignedStatus, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    unsigned char ucPinReference = pPinData->id; 
    *piTriesLeft = -1;
    if(m_pReader != NULL)
    {
        unsigned char ucOutData[BEID_MAX_SIGNATURE_LEN + 1] = {0};
        unsigned long ulOutLen = sizeof(ucOutData);
        // Get Card Data
        lRet = m_pReader->GetPINStatus(ucOutData, &ulOutLen, ucPinReference, bSignature ? true : false, ptStatus);  
        if(SC_NO_ERROR == lRet)
        {
            if(ulOutLen > 0)
            {
                *piTriesLeft = ucOutData[0]; 
                if(bSignature)
                {
                    if(ptSignedStatus->length < ulOutLen - 1)
                    {
                        // Buffer too small
                        ptStatus->general = BEID_E_INSUFFICIENT_BUFFER;
                        return FALSE;
                    }
                    memcpy(ptSignedStatus->data, ucOutData + 1, ulOutLen - 1);
                    ptSignedStatus->length = ulOutLen - 1;
                }
            }
            else
            {
                // Card Error
                if (0 == memcmp(ptStatus->cardSW, "\x6D\x00", 2))
                {
                    ptStatus->general = BEID_E_UNSUPPORTED_FUNCTION;
                } 
            }
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::VerifyPIN(BEID_Pin *pPinData, const char *pszPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    *piTriesLeft = -1;
    if(m_pReader != NULL)
    {
        lRet = m_pReader->VerifyPIN(pPinData, pszPin, piTriesLeft, ptStatus);
    }

    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    *piTriesLeft = -1;
    if(m_pReader != NULL)
    {
        lRet = m_pReader->ChangePIN(pPinData, pszOldPin, pszNewPin, piTriesLeft, ptStatus);
    }

    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::ReadFile(BEID_Bytes *ptFileID, BEID_Bytes *ptOutData, BEID_Pin *pPinData, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    // Firewall Check
    if(!FireWallAllowed("readfile"))
    {
        return FALSE;
    }

    unsigned char ucPinReference = pPinData->id; 
    if(m_pReader != NULL)
    {
        unsigned long ulOutDataLen = ptOutData->length;
        if(SC_NO_ERROR == (lRet = m_pReader->BeginTransaction(ptStatus)))
        {
            lRet = m_pReader->SelectFile(ptFileID->data, ptFileID->length, 0x02, ptStatus);
            if(SC_NO_ERROR == lRet)
            {
                lRet = m_pReader->ReadBinary(ptOutData->data, &ptOutData->length, ptStatus);   
            }

            if(ucPinReference > 0x00)
            {
                // Check for 'Security Status not satisfied' error
                if(0x69 == ptStatus->cardSW[0] && 0x82 == ptStatus->cardSW[1])
                {
                    long iLeft = -1;
                    lRet = VerifyPIN(pPinData, NULL, &iLeft, ptStatus);
                    if(SC_NO_ERROR == lRet)
                    {
                        ptOutData->length = ulOutDataLen;
                        memset(ptOutData->data, 0, ulOutDataLen);
                        lRet = m_pReader->ReadBinary(ptOutData->data, &ptOutData->length, ptStatus);   
                    }
                }
            }
            m_pReader->EndTransaction(ptStatus);     
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::WriteFile(BEID_Bytes *ptFileID, BEID_Bytes *ptInData, BEID_Pin *pPinData, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    // Firewall Check
    if(!FireWallAllowed("writefile"))
    {
        return FALSE;
    }
    unsigned char ucPinReference = pPinData->id;
    if(m_pReader != NULL)
    {
        if(SC_NO_ERROR == (lRet = m_pReader->BeginTransaction(ptStatus)))
        {
            lRet = m_pReader->SelectFile(ptFileID->data, ptFileID->length, 0x02, ptStatus);
            if(SC_NO_ERROR == lRet)
            {
                lRet = m_pReader->UpdateBinary(ptInData->data, ptInData->length, ptStatus);   
            }

            if(ucPinReference > 0x00)
            {
                // Check for 'Security Status not satisfied' error
                if(0x69 == ptStatus->cardSW[0] && 0x82 == ptStatus->cardSW[1])
                {
                    long iLeft = -1;
                    lRet = VerifyPIN(pPinData, NULL, &iLeft, ptStatus);
                    if(SC_NO_ERROR == lRet)
                    {
                        // Retry command
                        lRet = m_pReader->UpdateBinary(ptInData->data, ptInData->length, ptStatus);   
                    }
                }
            }
            m_pReader->EndTransaction(ptStatus);
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

long CBEIDApp::VerifyCertificates(BEID_Certif_Check *ptCertifCheck)
{
    long lRet = SC_NO_ERROR;

    ptCertifCheck->usedPolicy = BEID_POLICY_NONE;

    // First check the mandatory
    if (BEID_OCSP_CRL_MANDATORY == m_iOCSP || (BEID_OCSP_CRL_OPTIONAL == m_iOCSP && BEID_OCSP_CRL_OPTIONAL == m_iCRL))
    {
        ptCertifCheck->usedPolicy = BEID_POLICY_OCSP;
        lRet = m_pCertifManager->VerifyCertsOCSP();
    }
    else if(BEID_OCSP_CRL_MANDATORY == m_iCRL)
    {
        ptCertifCheck->usedPolicy = BEID_POLICY_CRL;
        lRet = m_pCertifManager->VerifyCertsCRL(); 
    }

    // In case OCSP or CRL checking failed with error, return
    if(SC_NO_ERROR != lRet && (BEID_OCSP_CRL_MANDATORY == m_iOCSP || BEID_OCSP_CRL_MANDATORY == m_iCRL))
    {
        return lRet;
    }

    // First check if some certificates are not validated
    if(ptCertifCheck->usedPolicy != BEID_POLICY_NONE && m_pCertifManager->FindNotValidated())
    {
        lRet = -1;
    }

    if(SC_NO_ERROR != lRet || ptCertifCheck->usedPolicy == BEID_POLICY_NONE)
    {
        // Check the optional
        if (BEID_OCSP_CRL_OPTIONAL == m_iCRL)
        {
            ptCertifCheck->usedPolicy |= BEID_POLICY_CRL;            
            lRet = m_pCertifManager->VerifyCertsCRL(); 
        }
        else if(BEID_OCSP_CRL_OPTIONAL == m_iOCSP)
        {
            ptCertifCheck->usedPolicy |= BEID_POLICY_OCSP;            
            lRet = m_pCertifManager->VerifyCertsOCSP();
        }
        else
        {
            lRet = SC_NO_ERROR;
        }
    }
    return lRet;
}

BOOL CBEIDApp::FireWallAllowed(char *pszType)
{
    CConfig oConfig;
    oConfig.Load();
    BOOL bRet = FALSE; 
    MYLOG4CPLUS_DEBUG("*** Begin FireWallAllowed ***");

    if(!oConfig.AllowFirewall())
    {
        MYLOG4CPLUS_DEBUG("*** FireWall Disabled ***");
        return TRUE;
    }
    if("VIRTUAL" == m_szReaderName)
    {
        return TRUE;
    }

    // Check Application
    if(m_szProcessName.length() > 0)
    {
        std::string strTemp("Checking process ");
        strTemp.append(m_szProcessName); 
        MYLOG4CPLUS_DEBUG(strTemp);
        if(oConfig.GetProgramAccess(m_szProcessName, pszType))
        {
            MYLOG4CPLUS_DEBUG("Access granted");
            bRet = TRUE;
        }
        else
        {
            CWallDialog dialog;
            dialog.FillData(m_szProcessName.c_str(), pszType); 
            dialog.exec();
            switch(dialog.GetResult())
            {
                case ID_BUTTON_YES : 
                    bRet = TRUE;
                    MYLOG4CPLUS_DEBUG("YES Clicked");
                    break;
                case ID_BUTTON_ALWAYS :
                    bRet = TRUE;
                    MYLOG4CPLUS_DEBUG("ALWAYS Clicked");
                    oConfig.UpdateProgramAccess(m_szProcessName, pszType);
                    break;
                case ID_BUTTON_ALWAYS_ALL :
                    bRet = TRUE;
                    MYLOG4CPLUS_DEBUG("ALWAYS ALL Clicked");
                    oConfig.UpdateProgramAccess(m_szProcessName, "*");
                    break;
            }
        }
    }
    MYLOG4CPLUS_DEBUG("*** End FireWallAllowed ***");
    return bRet;
}

BOOL CBEIDApp::GetRawData(BEID_Raw *pRawData, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        // ID data
        BEID_ID_Data idData = {0};
        BEID_Certif_Check tCheck = {0};
        if(!GetID(&idData, &tCheck, ptStatus, pRawData))
        {
            return FALSE;
        }
        // Address Data
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));
        BEID_Address addrData = {0};
        if(!GetAddress(&addrData, &tCheck, ptStatus, pRawData))
        {
            return FALSE;
        }
        // Picture
        memset(&tCheck, 0, sizeof(BEID_Certif_Check));
        BYTE buffer[BEID_MAX_PICTURE_LEN] = {0};
        BEID_Bytes tBytes = {0};
        tBytes.length = BEID_MAX_PICTURE_LEN;
        tBytes.data = buffer;
        if(!GetPicture(&tBytes, &tCheck, ptStatus, pRawData))
        {
            return FALSE;
        }
        // VersionInfo
        BEID_VersionInfo tVersion = {0};
        if(!GetVersionInfo(&tVersion, FALSE, NULL, ptStatus, pRawData))
        {
            return FALSE;
        }
        // RN Cert
        eidlib::CCertif *pCertifRN = m_pCertifManager->GetCertif("RN");
        if(pCertifRN != NULL)
        {
            memcpy(pRawData->certRN, pCertifRN->GetData(), pCertifRN->GetLength());
            pRawData->certRNLength = pCertifRN->GetLength();  
        }

        // Challenge Response
        unsigned char ucChallenge[BEID_MAX_CHALLENGE_LEN] = {0};
        unsigned long ulChLen = sizeof(ucChallenge);
        unsigned char ucResponse[BEID_MAX_RESPONSE_LEN] = {0};
        unsigned long ulRespLen = sizeof(ucResponse);
        if(SC_NO_ERROR == (lRet = m_pReader->GetChallengeResponse(ucChallenge, &ulChLen, ucResponse, &ulRespLen, ptStatus)))
        {
            memcpy(pRawData->challenge, ucChallenge, ulChLen);
            pRawData->challengeLength = ulChLen;
            memcpy(pRawData->response, ucResponse, ulRespLen);
            pRawData->responseLength = ulRespLen;
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::SetRawData(BEID_Raw *pRawData, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        m_pReader->SetRawData(pRawData); 
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

void CBEIDApp::FillProcessName()
{
    unsigned long ulPid = 0;
#ifdef _WIN32
    ulPid = ::GetCurrentProcessId();
#else
    ulPid = (unsigned long)getpid();
#endif
    MYLOG4CPLUS_DEBUG("ProcessID = " << ulPid);

    if(ulPid > 0)
    {
#ifdef _WIN32
        CProcessApi papi;
        MYLOG4CPLUS_DEBUG("Trying Init of ProcessAPI...");
        if (papi.Init(false))
        {
            MYLOG4CPLUS_DEBUG("SUCCESS Init of ProcessAPI");
            MYLOG4CPLUS_DEBUG("Trying ProcessesGetList...");
            DWORD pl = papi.ProcessesGetList();
            if (pl)
            {
                MYLOG4CPLUS_DEBUG("SUCCESS ProcessesGetList");
                CProcessApi::tProcessInfo pi;
                MYLOG4CPLUS_DEBUG("Walking ProcessesGetList...");
                while (papi.ProcessesWalk(pl, &pi))
                {
                    MYLOG4CPLUS_DEBUG("Found ProcessID = " << pi.pid << " ProcessName = " << pi.FileName);
                    if(pi.pid == ulPid)
                    {
                        QFileInfo fName(pi.FileName);
                        m_szProcessName = fName.fileName();
                        MYLOG4CPLUS_DEBUG("Match Found ProcessName = " << m_szProcessName);
                        break;
                    }
                }
            }
            papi.ProcessesFreeList(pl);
        }
#endif
#ifndef _WIN32
        char *pszLine = NULL;
        char *pszToken = NULL;
        char *pszCmd = NULL;
        char szLine[256] = {0};
        char szCommand[64] = {0};
        sprintf(szCommand, "ps -p %ld 2>/dev/null", ulPid);
        FILE *fp = popen(szCommand, "r");
        if(fp != NULL)
        {
            // read the header line
            if (NULL != fgets(szLine, sizeof(szLine), fp))
            {
                for (pszLine = szLine; ; pszLine = NULL)
                {
                    if (NULL == (pszToken = strtok(pszLine, " \t\n")))
                    {
                        pclose(fp);
                        return;
                    }

                    if (0 == strcmp("COMMAND", pszToken) || 0 == strcmp("CMD", pszToken))
                    { 
                        //  We found the COMMAND column 
                        pszCmd = pszToken;
                        break;
                    }
                }
                if (NULL != fgets(szLine, sizeof(szLine), fp))
                {
                   // Grab the "word" underneath the command heading...
                   if (NULL != (pszToken = strtok(pszCmd, " \t\n")))
                   {
                        m_szProcessName = pszToken;
                   }
                }
            }
            pclose(fp);
        }
#endif
    }
}

bool CBEIDApp::CheckVersion(BEID_VersionInfo *ptVersionInfo)
{
    bool bAsk = false;
    // Check Applet Interface Version
    if(ptVersionInfo->GlobalOSVersion > EIDLIB_CURRENT_GLOBALOS_VERSION)
    {
        bAsk = true;
    }
    // Check TokenInfo
    if(ptVersionInfo->ElecPersoInterface > EIDLIB_CURRENT_ELECTRO_PERSO_ITF)
    {
        bAsk = true;
    }
    if(bAsk && m_pAutoUpdate != NULL)
    {
        m_pAutoUpdate->RetrieveNewVersion(true);
    }
    return bAsk;
}

int CBEIDApp::AskDownload(QString & strMessage)
{
    QMutexLocker locker( &m_cs );
    QMessageBox mb(pLangDlgTitle[m_iLang],
        (const char *)strMessage,
        QMessageBox::Question,
        QMessageBox::Yes,
        QMessageBox::No | QMessageBox::Default,
        QMessageBox::NoButton, 0, 0, true, Qt::WStyle_DialogBorder | Qt::WStyle_StaysOnTop);
    mb.setButtonText( QMessageBox::Yes, pLangYes[m_iLang] );
    mb.setButtonText( QMessageBox::No, pLangNo[m_iLang] );
    return mb.exec();
}

int CBEIDApp::AskTestCard()
{
    if("VIRTUAL" != m_szReaderName)
    {
        QMutexLocker locker( &m_cs );
        QMessageBox mb( pLangDlgTitle[m_iLang], pLangRootWarning[m_iLang],
            QMessageBox::Warning,
            QMessageBox::Yes,
            QMessageBox::No | QMessageBox::Default,
            QMessageBox::NoButton, 0, 0, true, Qt::WStyle_DialogBorder | Qt::WStyle_StaysOnTop);
        mb.setButtonText( QMessageBox::Yes, pLangYes[m_iLang]);
        mb.setButtonText( QMessageBox::No, pLangNo[m_iLang] );
        return mb.exec();
    }
    return QMessageBox::Yes;
}

BOOL CBEIDApp::GetPINs(BEID_Pins *pPins, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    pPins->pinsLength = 0;
    if(m_pReader != NULL)
    {
        lRet = m_pReader->GetPINs(pPins, ptStatus);
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::VerifyCertificatesCRL(BEID_Certif_Check *ptCertifCheck, BOOL bDownload, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    ptCertifCheck->usedPolicy = BEID_POLICY_CRL;
    CCertifManager *pCertifManager = new CCertifManager();
    pCertifManager->SetCardReader(m_pReader);
    for (int i = 0; i < ptCertifCheck->certificatesLength; i++)
    {
        pCertifManager->AddCertif(ptCertifCheck->certificates[i].certif, 
            ptCertifCheck->certificates[i].certifLength, ptCertifCheck->certificates[i].certifLabel);
    }
    CCertifManager *pOldCertifManager = CVerify::GetCertifManager();
    CVerify::SetCertifManager(pCertifManager); 

    lRet = pCertifManager->VerifyCertsCRL(bDownload ? true : false);
    pCertifManager->FillCertifs(ptCertifCheck);
    CVerify::SetCertifManager(pOldCertifManager); 
    pCertifManager->Cleanup();
    delete pCertifManager;

    ConvertOpenSCError(lRet, ptStatus);

    return (lRet == SC_NO_ERROR);
}

BOOL CBEIDApp::VerifyCertificatesOCSP(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    ptCertifCheck->usedPolicy = BEID_POLICY_OCSP;
    CCertifManager *pCertifManager = new CCertifManager();
    pCertifManager->SetCardReader(m_pReader);
    for (int i = 0; i < ptCertifCheck->certificatesLength; i++)
    {
        pCertifManager->AddCertif(ptCertifCheck->certificates[i].certif, 
            ptCertifCheck->certificates[i].certifLength, ptCertifCheck->certificates[i].certifLabel);
    }
    CCertifManager *pOldCertifManager = CVerify::GetCertifManager();
    CVerify::SetCertifManager(pCertifManager); 

    lRet = pCertifManager->VerifyCertsOCSP();
    pCertifManager->FillCertifs(ptCertifCheck);
    CVerify::SetCertifManager(pOldCertifManager); 
    pCertifManager->Cleanup();
    delete pCertifManager;

    ConvertOpenSCError(lRet, ptStatus);

    return (lRet == SC_NO_ERROR);
}

BOOL CBEIDApp::ReadBinary(BEID_Bytes *ptFileID, int iOffset, int iCount, BEID_Bytes *ptOutData, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        if(ptFileID->length > 2)
        {
            lRet = m_pReader->ReadFile(ptFileID->data, ptFileID->length, ptOutData->data, &ptOutData->length, ptStatus);              
        }
        else
        {
            if(SC_NO_ERROR == (lRet = m_pReader->BeginTransaction(ptStatus)))
            {
                if (SelectApplication(&gtBelpicAID, ptStatus))
                {                
                    lRet = m_pReader->SelectFile(ptFileID->data, ptFileID->length, 0x02, ptStatus);
                    if(SC_NO_ERROR == lRet)
                    {
                        lRet = m_pReader->ReadBinary(ptOutData->data, &ptOutData->length, ptStatus, iOffset, iCount);   
                    }
                }
                m_pReader->EndTransaction(ptStatus);     
            }
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::GetCertificates(BEID_Certif_Check *ptCertifCheck, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    ptCertifCheck->usedPolicy = BEID_POLICY_NONE;
    if(m_pReader != NULL)
    {
        if(SC_NO_ERROR == (lRet = ReadCertificates(ptCertifCheck, ptStatus, true)))
        {
            m_pCertifManager->FillCertifs(ptCertifCheck);
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::GetRawFile(BEID_Bytes *pRawFile, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        BEID_Raw rawData = {0};
        GetRawData(&rawData, ptStatus);
        if(BEID_OK == ptStatus->general)
        {
            BEID_Bytes tBytes = {0};
            tBytes.length = 8192;
            tBytes.data = new BYTE[tBytes.length];
            memset(tBytes.data, 0, tBytes.length);        
            CTLVBuffer oTLV;
            oTLV.MakeFileTLV(&rawData, &tBytes);
            if(pRawFile->length < tBytes.length)
            {
                // Buffer too small
                ptStatus->general = BEID_E_INSUFFICIENT_BUFFER;
                lRet = SC_ERROR_BUFFER_TOO_SMALL;
            }
            else
            {
                memcpy(pRawFile->data, tBytes.data, tBytes.length);
                pRawFile->length = tBytes.length;
            }
            delete [] tBytes.data;
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

BOOL CBEIDApp::SetRawFile(BEID_Bytes *pRawFile, BEID_Status *ptStatus)
{
    QMutexLocker locker( &m_cs );
    long lRet = SC_NO_ERROR;
    if(m_pReader != NULL)
    {
        BEID_Raw rawData = {0};
        CTLVBuffer oTLV;
        if(oTLV.ParseFileTLV(pRawFile->data, pRawFile->length))
        {        
            oTLV.FillBinaryData(0x01, rawData.idData, &rawData.idLength);
            oTLV.FillBinaryData(0x02, rawData.idSigData, &rawData.idSigLength);
            oTLV.FillBinaryData(0x03, rawData.addrData, &rawData.addrLength);
            oTLV.FillBinaryData(0x04, rawData.addrSigData, &rawData.addrSigLength);
            oTLV.FillBinaryData(0x05, rawData.pictureData, &rawData.pictureLength);
            oTLV.FillBinaryData(0x06, rawData.cardData, &rawData.cardDataLength);
            oTLV.FillBinaryData(0x07, rawData.tokenInfo, &rawData.tokenInfoLength);
            oTLV.FillBinaryData(0x08, rawData.certRN, &rawData.certRNLength);
            oTLV.FillBinaryData(0x09, rawData.challenge, &rawData.challengeLength);
            oTLV.FillBinaryData(0x0A, rawData.response, &rawData.responseLength);
            SetRawData(&rawData, ptStatus);
        }
        else
        {
            ptStatus->general = BEID_E_UNKNOWN;
            lRet = SC_ERROR_INTERNAL;
        }
    }
    return lRet == SC_NO_ERROR ? TRUE : FALSE;
}

void CBEIDApp::MapLanguage()
{
    m_iLang = 0;
    QLocale oLocal;
    QLocale::Language lng = oLocal.language();
    if(QLocale::Dutch == lng)
    {
        m_iLang = 1;
    }
    else if(QLocale::French == lng)
    {
        m_iLang = 2;
    }
    else if(QLocale::German == lng)
    {
        m_iLang = 3;
    }
}

