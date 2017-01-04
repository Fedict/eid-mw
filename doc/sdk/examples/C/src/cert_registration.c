/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2011 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */

#include "cert_registration.h"
#include "base.h"
#include <Wincrypt.h>


CK_ULONG ByteArrayToString( CK_CHAR_PTR* destString ,CK_ULONG ulDestLen,CK_BYTE* byteArray, CK_ULONG ulArrayLen);

//**************************************************
// Use Minidriver if OS is Vista or later
//**************************************************
BOOL UseMinidriver( void )
{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsVistaorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsVistaorLater = (osvi.dwMajorVersion >= 6);

	return bIsWindowsVistaorLater;
}

//**************************************************
// Checks of older registered certificates are not
// still bound to the CSP when the minidriver is used
//**************************************************
BOOL ProviderNameCorrect (PCCERT_CONTEXT pCertContext )
{
	unsigned long dwPropId= CERT_KEY_PROV_INFO_PROP_ID; 
	DWORD cbData = 0;
	CRYPT_KEY_PROV_INFO * pCryptKeyProvInfo;

	if (!UseMinidriver())
		return TRUE;

	if(!(CertGetCertificateContextProperty(
		pCertContext,		// A pointer to the certificate where the property will be set.
		dwPropId,           // An identifier of the property to get.
		NULL,               // NULL on the first call to get the length.
		&cbData)))          // The number of bytes that must be allocated for the structure.
	{
		if (GetLastError() != CRYPT_E_NOT_FOUND) // The certificate does not have the specified property.
			return FALSE;
	}
	if(!(pCryptKeyProvInfo = (CRYPT_KEY_PROV_INFO *)malloc(cbData)))
	{
		return TRUE;
	}
	if(CertGetCertificateContextProperty(pCertContext, dwPropId, pCryptKeyProvInfo, &cbData))
	{
		if (!wcscmp(pCryptKeyProvInfo->pwszProvName, L"Belgium Identity Card CSP"))
			return FALSE;
	}
	return TRUE;
}

DWORD StoreAuthorityCert(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits)
{
	DWORD dwRet = 0;
	HCERTSTORE hMemoryStore = NULL;
	PCCERT_CONTEXT pDesiredCert = NULL;

	if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData, pCertContext->pCertInfo->Subject.pbData, pCertContext->pCertInfo->Subject.cbData	)	)
	{
		hMemoryStore = CertOpenSystemStore ((HCRYPTPROV_LEGACY)NULL, TEXT("ROOT"));
	}
	else
	{
		hMemoryStore = CertOpenSystemStore ((HCRYPTPROV_LEGACY)NULL, TEXT("CA"));
	}

	if (hMemoryStore == NULL)
	{
		dwRet = GetLastError();
		printf("StoreAuthorityCerts: Unable to open the system certificate store. Error code: %d.\n",dwRet);
		return dwRet;
	}

	pDesiredCert = CertFindCertificateInStore( hMemoryStore
		, X509_ASN_ENCODING
		, 0
		, CERT_FIND_EXISTING
		, pCertContext
		, NULL
		);
	if( pDesiredCert )
	{
		CertFreeCertificateContext(pDesiredCert);
	}
	else if (GetLastError())
	{
		CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
		CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH);
		if(CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
		{
			printf("StoreUserCerts: Certificate context added to store.\n");
			dwRet = 0;
		}
		else
		{
			dwRet = GetLastError();
			printf("StoreAuthorityCerts: Unable to add certificate context to store. Error code: %d.\n",dwRet);
		}
	}
	CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);

	return dwRet;

}

DWORD StoreUserCert (PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, CK_BYTE* cardSerialNumber, CK_ULONG cardSerialNumberLen)
{
	unsigned long dwFlags 		= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
	PCCERT_CONTEXT  pDesiredCert	= NULL;
	PCCERT_CONTEXT  pPrevCert		= NULL;
	DWORD dwRet					= 0;
	wchar_t* pContainerName		= NULL;
	size_t pContainerNameCharLen = cardSerialNumberLen+20;
	CK_CHAR_PTR pcardSerialNrString = NULL;
	wchar_t* pProviderName		= NULL;
	CK_ULONG counter=0;
	HCERTSTORE hMyStore 		= CertOpenSystemStore((HCRYPTPROV_LEGACY)NULL, TEXT("MY"));


	CRYPT_KEY_PROV_INFO cryptKeyProvInfo;
	unsigned long dwPropId = CERT_KEY_PROV_INFO_PROP_ID;

	if (hMyStore == NULL)
	{
		dwRet = GetLastError();
		printf("StoreUserCerts: Unable to open the system certificate store. Error code: %d.\n",dwRet);
		return dwRet;
	}

		// ----------------------------------------------------
		// look if we already have a certificate with the same 
		// subject (contains name and NNR) in the store
		// If the certificate is not found --> NULL
		// ----------------------------------------------------
		do
		{
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject) , pPrevCert)))
			{
				// ----------------------------------------------------
				// If the certificates are identical and function 
				// succeeds, the return value is nonzero, or TRUE.
				// ----------------------------------------------------
				if(FALSE == CertCompareCertificate(X509_ASN_ENCODING,pCertContext->pCertInfo,pDesiredCert->pCertInfo) || !ProviderNameCorrect(pDesiredCert) )
				{
					// ----------------------------------------------------
					// certificates are not identical, but have the same 
					// subject (contains name and NNR),
					// so we remove the one that was already in the store
					// ----------------------------------------------------
					if(FALSE == CertDeleteCertificateFromStore(pDesiredCert))
					{
						if (E_ACCESSDENIED == GetLastError())
						{
							continue;
						}
					}
					pPrevCert = NULL;
					continue;
				}
			}
			pPrevCert = pDesiredCert;
		}while (NULL != pDesiredCert);

	// ----------------------------------------------------
	// look if we already have the certificate in the store
	// If the certificate is not found --> NULL
	// ----------------------------------------------------
	if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL)))
	{
		// ----------------------------------------------------
		// certificate is already in the store, then just return
		// ----------------------------------------------------
		CertFreeCertificateContext(pDesiredCert);
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return 0;
	}

	pContainerName = (wchar_t*)malloc(sizeof(wchar_t) * (pContainerNameCharLen));
	if(pContainerName == NULL)
		return E_OUTOFMEMORY;
	pcardSerialNrString = (CK_CHAR_PTR)malloc(cardSerialNumberLen*2 + 1);
	if(pcardSerialNrString == NULL)
		return E_OUTOFMEMORY;

	if(-1 == ByteArrayToString( &pcardSerialNrString, cardSerialNumberLen*2 + 1,cardSerialNumber ,cardSerialNumberLen))
		return -1;

	if(pContainerName == NULL)
	{
		CertFreeCertificateContext(pDesiredCert);
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return -1;
	}
	if (UseMinidriver())
	{
		if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
		{
			swprintf_s(pContainerName,pContainerNameCharLen,L"NR_%hS",pcardSerialNrString);
		}
		else
		{
			swprintf_s(pContainerName,pContainerNameCharLen,L"DS_%hS",pcardSerialNrString);
		}
		cryptKeyProvInfo.pwszProvName			= L"Microsoft Base Smart Card Crypto Provider";
		cryptKeyProvInfo.dwKeySpec			= AT_SIGNATURE;
	}
	else
	{
		if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
		{
			swprintf_s(pContainerName,pContainerNameCharLen,L"Signature(%hS)",pcardSerialNrString);
		}
		else
		{
			swprintf_s(pContainerName,pContainerNameCharLen,L"Authentication(%hS)",pcardSerialNrString);
		}

		cryptKeyProvInfo.pwszProvName		= L"Belgium Identity Card CSP";
		cryptKeyProvInfo.dwKeySpec		= AT_KEYEXCHANGE;
	}
	cryptKeyProvInfo.pwszContainerName	= pContainerName;

	cryptKeyProvInfo.dwProvType = PROV_RSA_FULL;
	cryptKeyProvInfo.dwFlags = 0;
	cryptKeyProvInfo.cProvParam = 0;
	cryptKeyProvInfo.rgProvParam = NULL;


/*
			// Set friendly names for the certificates
			DWORD dwsize = 0;
			dwsize = CertGetNameStringW(pCertContext, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, NULL, dwsize);
			auto_vec<WCHAR> pname(new WCHAR[dwsize]);
			dwsize = CertGetNameStringW(pCertContext, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, pname.get(), dwsize);
			CRYPT_DATA_BLOB tpFriendlyName = {0, 0};
			tpFriendlyName.pbData = (BYTE *)pname.get();
			tpFriendlyName.cbData = dwsize * sizeof(WCHAR);

			if (CertSetCertificateContextProperty(
				pCertContext,       // A pointer to the certificate
				// where the propertiy will be set.
				CERT_FRIENDLY_NAME_PROP_ID,           // An identifier of the property to be set. 
				// In this case, CERT_KEY_PROV_INFO_PROP_ID
				// is to be set to provide a pointer with the
				// certificate to its associated private key 
				// container.
				dwFlags,            // The flag used in this case is   
				// CERT_STORE_NO_CRYPT_RELEASE_FLAG
				// indicating that the cryptographic 
				// context aquired should not
				// be released when the function finishes.
				&tpFriendlyName   // A pointer to a data structure that holds
				// infomation on the private key container to
				// be associated with this certificate.
				))
*/

	// Set the property.
	if (CertSetCertificateContextProperty(
		pCertContext, 		// A pointer to the certificate where the property will be set.
		dwPropId, 			// An identifier of the property to be set.
		// In this case, CERT_KEY_PROV_INFO_PROP_ID is to be set to provide a pointer with the certificate to its associated private key container.
		dwFlags, 			// The flag used in this case is
		// CERT_STORE_NO_CRYPT_RELEASE_FLAG indicating that the cryptographic context acquired should not be released when the function finishes.
		&cryptKeyProvInfo 	// A pointer to a data structure that holds infomation on the private key container to be associated with this certificate.
		))
	{
		if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
		{
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
		}
		else
		{
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH);
		}
		if (CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
		{
			printf("StoreUserCerts: Certificate context added to store.");
			dwRet = 0;
		}
		else
		{
			dwRet = GetLastError();
			printf("StoreUserCerts: Unable to add certificate context to store. Error code: %d.",dwRet);
		}
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMyStore = NULL;
	}

	if(pContainerName != NULL)
		free (pContainerName);

	return dwRet;
}

DWORD ImportCertificate(CK_BYTE* certData, DWORD certSize, CK_BYTE* cardSerialNumber, CK_ULONG cardSerialNumberLen)
{
	PCCERT_CONTEXT		pCertContext = NULL;
	DWORD				dwRet = 0;
	// ------------------------------------------------------------
	// create the certificate context with the certificate raw data
	// ------------------------------------------------------------
	pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData,certSize);

	if( pCertContext == NULL )
	{
		dwRet = GetLastError();
		if (dwRet == E_INVALIDARG)
			printf("ImportCertificates: Unable to create certificate context. The certificate encoding type is not supported.Error code: %d.\n",dwRet);
		else
			printf("ImportCertificates: Unable to create certificate context. Error code: %d.\n",dwRet);
	}
	else
	{
		unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
		CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);
		// ----------------------------------------------------------------------
		// Only store the context of the certificates with usages for an end-user
		// i.e. no CA or root certificates
		// ----------------------------------------------------------------------
		if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
		{
			dwRet = StoreAuthorityCert (pCertContext, KeyUsageBits);
		}
		else
		{
			dwRet = StoreUserCert (pCertContext, KeyUsageBits, cardSerialNumber, cardSerialNumberLen);
		}
		if (pCertContext)
			CertFreeCertificateContext(pCertContext);
	}
	return dwRet;
}

char ToHex(CK_BYTE uc)
{
    return (char)(uc <= 9 ? '0' + uc : 'A' - 10 + uc);
}

CK_ULONG ByteArrayToString( CK_CHAR_PTR* destString ,CK_ULONG ulDestLen,CK_BYTE* byteArray, CK_ULONG ulArrayLen)
{
	unsigned long ulOffset = 0;
	CK_ULONG i;
	CK_CHAR_PTR pcbuf = *destString;

	if((ulDestLen+1) < ulArrayLen)
		return 1;

	for (i = 0; i < ulArrayLen; i++)
	{
		pcbuf[ulOffset++] = ToHex(byteArray[i] / 16);
    pcbuf[ulOffset++] = ToHex(byteArray[i] % 16);
	}
	pcbuf[ulOffset] = '\0';

	return 0;
}