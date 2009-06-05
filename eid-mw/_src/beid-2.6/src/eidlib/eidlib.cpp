// eidlib.cpp : Defines the entry point for the DLL application.
//

#include "eidlib.h"
#include "BEIDApp.h"
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif

static CBEIDApp *pInit = NULL;

EIDLIB_API BEID_Status BEID_InitEx(char *pszReaderName, long lOCSP, long lCRL, long *plHandle, long lInterfaceVersion, long lInterfaceCompatVersion)
{
    BEID_Status tStatus = {0};

    assert(BEID_INTERFACE_VERSION >= BEID_INTERFACE_COMPAT_VERSION); 

    if ( (lInterfaceVersion > BEID_INTERFACE_VERSION) || 
         (lInterfaceCompatVersion < BEID_INTERFACE_COMPAT_VERSION))
    {
        tStatus.general = BEID_E_INCORRECT_VERSION; 
        return tStatus;
    }
    if(plHandle == NULL || (BEID_OCSP_CRL_MANDATORY == lOCSP && BEID_OCSP_CRL_MANDATORY == lCRL))
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }

    *plHandle = 0;
    if (pInit != NULL)
    {
        /* First cleanup via Exit */
        BEID_Exit();
    }
    pInit = new CBEIDApp(pszReaderName, lOCSP, lCRL);
    if (!pInit->Init(plHandle, &tStatus))
    {
        delete pInit;
        pInit = NULL;
    }

    return tStatus;
}

EIDLIB_API BEID_Status BEID_Exit()
{
    BEID_Status tStatus = {0};

    if(pInit != NULL)
    {
        pInit->Exit(&tStatus);
        delete pInit;
        pInit = NULL;
    }
    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetID(BEID_ID_Data *ptData, BEID_Certif_Check *ptCertifCheck)
{
    BEID_Status tStatus = {0};
    
    if(ptData == NULL || ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetID(ptData, ptCertifCheck, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetAddress(BEID_Address *ptAddress, BEID_Certif_Check *ptCertifCheck)
{
    BEID_Status tStatus = {0};
    
    if(ptAddress == NULL || ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetAddress(ptAddress, ptCertifCheck, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetPicture(BEID_Bytes *ptPicture, BEID_Certif_Check *ptCertifCheck)
{
    BEID_Status tStatus = {0};
    
    if(ptPicture == NULL || ptPicture->data == NULL || ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetPicture(ptPicture, ptCertifCheck, &tStatus);

    return tStatus;
}


EIDLIB_API BEID_Status BEID_BeginTransaction()
{
    BEID_Status tStatus = {0};
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }
    pInit->BeginTransaction(&tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_EndTransaction()
{
    BEID_Status tStatus = {0};
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }
    pInit->EndTransaction(&tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_SelectApplication(BEID_Bytes *ptApplication)
{
    BEID_Status tStatus = {0};
    
    if(ptApplication == NULL || ptApplication->data == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->SelectApplication(ptApplication, &tStatus);

    return tStatus;
}


EIDLIB_API BEID_Status BEID_FlushCache()
{
    BEID_Status tStatus = {0};
    
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->FlushCache(&tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_SendAPDU(BEID_Bytes *ptCmdAPDU,	BEID_Pin *pPinData, BEID_Bytes *ptRespAPDU)
{
    BEID_Status tStatus = {0};
    
    if(ptCmdAPDU == NULL || ptCmdAPDU->data == NULL || ptRespAPDU == NULL || 
        ptRespAPDU->data == NULL || pPinData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->SendAPDU(ptCmdAPDU, pPinData, ptRespAPDU, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetVersionInfo(BEID_VersionInfo *ptVersionInfo, BOOL bSignature, BEID_Bytes *ptSignedStatus)
{
    BEID_Status tStatus = {0};
    
    if(ptVersionInfo == NULL || (bSignature && (ptSignedStatus == NULL || ptSignedStatus->data == NULL)))
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetVersionInfo(ptVersionInfo, bSignature, ptSignedStatus, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetPINStatus(BEID_Pin *pPinData, long *piTriesLeft, BOOL bSignature, BEID_Bytes *ptSignedStatus)
{
    BEID_Status tStatus = {0};
    
    if(piTriesLeft == NULL || (bSignature && (ptSignedStatus == NULL || ptSignedStatus->data == NULL)))
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetPINStatus(pPinData, piTriesLeft, bSignature, ptSignedStatus, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_VerifyPIN(BEID_Pin *pPinData, char *pszPin, long *piTriesLeft)
{
    BEID_Status tStatus = {0};
    
    if(piTriesLeft == NULL || pPinData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }
    if(pszPin != NULL && strlen(pszPin) > BELPIC_MAX_USER_PIN_LEN)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    pInit->VerifyPIN(pPinData, pszPin, piTriesLeft, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_ChangePIN(BEID_Pin *pPinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft)
{
    BEID_Status tStatus = {0};
    
    if(piTriesLeft == NULL ||  pPinData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }
    if(pszOldPin != NULL && strlen(pszOldPin) > BELPIC_MAX_USER_PIN_LEN)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pszNewPin != NULL && strlen(pszNewPin) > BELPIC_MAX_USER_PIN_LEN)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    pInit->ChangePIN(pPinData, pszOldPin, pszNewPin, piTriesLeft, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_ReadFile(BEID_Bytes *ptFileID, BEID_Bytes *ptOutData, BEID_Pin *pPinData)
{
    BEID_Status tStatus = {0};
    
    if(ptFileID == NULL || ptFileID->data == NULL || ptOutData == NULL || 
        ptOutData->data == NULL || pPinData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->ReadFile(ptFileID, ptOutData, pPinData, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_WriteFile(BEID_Bytes *ptFileID, BEID_Bytes *ptInData, BEID_Pin *pPinData)
{
    BEID_Status tStatus = {0};
    
    if(ptFileID == NULL || ptFileID->data == NULL || ptInData == NULL || 
        ptInData->data == NULL || pPinData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->WriteFile(ptFileID, ptInData, pPinData, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetRawData(BEID_Raw *pRawData)
{
    BEID_Status tStatus = {0};

    if(pRawData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetRawData(pRawData, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_SetRawData(BEID_Raw *pRawData)
{
    BEID_Status tStatus = {0};

    if(pRawData == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->SetRawData(pRawData, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetPINs(BEID_Pins *Pins)
{
    BEID_Status tStatus = {0};

    if(Pins == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetPINs(Pins, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_VerifyCRL(BEID_Certif_Check *ptCertifCheck, BOOL bDownload)
{
    BEID_Status tStatus = {0};

    if(ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->VerifyCertificatesCRL(ptCertifCheck, bDownload, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_VerifyOCSP(BEID_Certif_Check *ptCertifCheck)
{
    BEID_Status tStatus = {0};

    if(ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->VerifyCertificatesOCSP(ptCertifCheck, &tStatus);

    return tStatus;
}


EIDLIB_API BEID_Status BEID_ReadBinary(BEID_Bytes *ptFileID, int iOffset, int iCount, BEID_Bytes *ptOutData)
{
    BEID_Status tStatus = {0};
    
    if(ptFileID == NULL || ptFileID->data == NULL || ptOutData == NULL || 
        ptOutData->data == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->ReadBinary(ptFileID, iOffset, iCount, ptOutData, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetCertificates(BEID_Certif_Check *ptCertifCheck)
{
    BEID_Status tStatus = {0};
    
    if(ptCertifCheck == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetCertificates(ptCertifCheck, &tStatus);

    return tStatus;
}

EIDLIB_API BEID_Status BEID_GetRawFile(BEID_Bytes *ptRawFile)
{
    BEID_Status tStatus = {0};
    
    if(ptRawFile == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->GetRawFile(ptRawFile, &tStatus);

    return tStatus;
}


EIDLIB_API BEID_Status BEID_SetRawFile(BEID_Bytes *ptRawFile)
{
    BEID_Status tStatus = {0};
    
    if(ptRawFile == NULL)
    {
        tStatus.general = BEID_E_BAD_PARAM;
        return tStatus;
    }
    if(pInit == NULL)
    {
        tStatus.general = BEID_E_NOT_INITIALIZED;
        return tStatus;
    }

    pInit->SetRawFile(ptRawFile, &tStatus);

    return tStatus;
}
