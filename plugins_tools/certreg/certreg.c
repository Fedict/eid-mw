/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2012 FedICT.
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

#include <windows.h>
#include <stdio.h>
#include "certreg.h"


BOOL ImportCertificate(BYTE* pbserialNumber,DWORD serialNumberLen,
												BYTE* pbcertificateData,DWORD dwcertificateDataLen,
												PCCERT_CONTEXT	*ppCertContext)
{
	//PCCERT_CONTEXT	pCertContext = NULL;
	BOOL			bImported	= FALSE;

	if ( !pbserialNumber || serialNumberLen == 0 || !pbcertificateData || dwcertificateDataLen == 0 )
	{
		return FALSE;
	}

	// ----------------------------------------------------
	// create the certificate context with the certificate raw data
	// ----------------------------------------------------
	*ppCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbcertificateData, dwcertificateDataLen);

	if( *ppCertContext )
	{
		unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
		CertGetIntendedKeyUsage(X509_ASN_ENCODING, (*ppCertContext)->pCertInfo, &KeyUsageBits, 1);

		// ----------------------------------------------------
		// Only store the context of the certificates with usages for an end-user 
		// i.e. no CA or root certificates
		// ----------------------------------------------------
		if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
		{
			if(StoreAuthorityCerts (*ppCertContext, KeyUsageBits) )                    
			{
				bImported = TRUE;
			}
		}
		else
		{
			if(StoreUserCerts (*ppCertContext, KeyUsageBits,pbserialNumber,serialNumberLen ))
			{
				bImported = TRUE;
			}
		}

		//pCertContext not released, we keep using it
	}
	return bImported;
}

//*****************************************************
// store the user certificates 
//*****************************************************
BOOL StoreUserCerts (PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, BYTE* pbserialNumber,DWORD dwserialNumberLen)
{
	unsigned long	dwFlags			= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
	PCCERT_CONTEXT  pDesiredCert	= NULL;
	PCCERT_CONTEXT  pPrevCert		= NULL;
	char* cardSerialNumber = NULL;
	WCHAR*   containerName = NULL;
	CRYPT_KEY_PROV_INFO cryptKeyProvInfo;
	CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo	= NULL;
	unsigned long		 dwPropId			= CERT_KEY_PROV_INFO_PROP_ID; 
	unsigned long	ulID			= 0;
	int i;
	CRYPT_ALGORITHM_IDENTIFIER algIdentifier;
	BOOL isECDsa = 0;

	//First parameter is not used and should be set to NULL.
	HCERTSTORE		hMyStore		= CertOpenSystemStore((HCRYPTPROV)NULL, "MY");

	if ( NULL != hMyStore )
	{
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
				if(FALSE == CertCompareCertificate(X509_ASN_ENCODING,pCertContext->pCertInfo,pDesiredCert->pCertInfo) ||
					!ProviderNameCorrect(pDesiredCert) )
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
							//QString strCaption(tr("Deleting former certificate"));
							//QString strMessage(tr("Error deleting former certificate"));
							//QMessageBox::information(NULL,strCaption,strMessage);
						}
					}
					//pDesiredCert and pPrevCert are freed, so set pPrevCert = NULL and search again
					pPrevCert = NULL;
					continue;
				}
			}
			pPrevCert = pDesiredCert;
		}while (NULL != pDesiredCert);

		// ----------------------------------------------------
		// check if the certificate is already in the store
		// ----------------------------------------------------
		if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL)) )
		{
			// ----------------------------------------------------
			// certificate is already in the store, then just return
			// ----------------------------------------------------
			CertFreeCertificateContext(pDesiredCert);
			CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
			return TRUE;
		}

		//Check signature algoritm (RSA or ECDSA)
		algIdentifier = pCertContext->pCertInfo->SignatureAlgorithm;

		char objOid[256];
		if (sprintf_s(objOid, 256, "%s", algIdentifier.pszObjId) != -1)
		{
			if (strcmp(objOid, "1.2.840.10045.4.3") == 0 || strcmp(objOid, "1.2.840.10045.4.1") == 0 || //szOID_ECDSA_SPECIFIED of szOID_ECDSA_SHA1
				strcmp(objOid, "1.2.840.10045.4.3.2") == 0 || strcmp(objOid, "1.2.840.10045.4.3.3") == 0 || //szOID_ECDSA_SHA256 of szOID_ECDSA_SHA384
				strcmp(objOid, "1.2.840.10045.4.3.4") == 0) //szOID_ECDSA_SHA512
			{
				isECDsa = 1;
			}
		}
		else
		{
			//could not identify signature algoritm
			return FALSE;
		}

		// ----------------------------------------------------
		// Initialize the CRYPT_KEY_PROV_INFO data structure.
		// Note: pwszContainerName and pwszProvName can be set to NULL 
		// to use the default container and provider.
		// ----------------------------------------------------
		//memset(&cryptKeyProvInfo,0,sizeof(CRYPT_KEY_PROV_INFO));
		pCryptKeyProvInfo	= &cryptKeyProvInfo;

		// ----------------------------------------------------
		// Get the serial number
		// ----------------------------------------------------
		cardSerialNumber = (char*)malloc(2*dwserialNumberLen+1);
		containerName = (WCHAR*)malloc(sizeof(WCHAR)*(2*dwserialNumberLen + 20));

		for (i=0; i < dwserialNumberLen; i++) {
			sprintf(cardSerialNumber + 2*i, "%02X", pbserialNumber[i]);
		}
		cardSerialNumber[2*dwserialNumberLen] = 0;

		if( (cardSerialNumber != NULL) && (containerName != NULL) )
		{
			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				swprintf(containerName, (2 * dwserialNumberLen + 4), L"NR_%S", cardSerialNumber);
			}
			else
			{
				swprintf(containerName, (2 * dwserialNumberLen + 4), L"DS_%S", cardSerialNumber);
			}
			if (isECDsa)
			{
				pCryptKeyProvInfo->pwszProvName = MS_SMART_CARD_KEY_STORAGE_PROVIDER;
				// Special dwKeySpec indicating a CNG NCRYPT_KEY_HANDLE instead of a CAPI1 HCRYPTPROV
				pCryptKeyProvInfo->dwKeySpec = 0;// CERT_NCRYPT_KEY_SPEC;
			}
			else
			{
				pCryptKeyProvInfo->pwszProvName = L"Microsoft Base Smart Card Crypto Provider";
				pCryptKeyProvInfo->dwKeySpec = AT_SIGNATURE;
			}
			pCryptKeyProvInfo->pwszContainerName = containerName;
			if (isECDsa)
			{
				pCryptKeyProvInfo->dwProvType = 0;
			}
			else
			{
				pCryptKeyProvInfo->dwProvType = PROV_RSA_FULL;
			}
			pCryptKeyProvInfo->dwFlags				= 0;
			pCryptKeyProvInfo->cProvParam			= 0;
			pCryptKeyProvInfo->rgProvParam			= NULL;

			// Set the property.
			if (CertSetCertificateContextProperty(
				pCertContext,       // A pointer to the certificate
				// where the propertiy will be set.
				dwPropId,           // An identifier of the property to be set. 
				// In this case, CERT_KEY_PROV_INFO_PROP_ID
				// is to be set to provide a pointer with the
				// certificate to its associated private key 
				// container.
				dwFlags,            // The flag used in this case is   
				// CERT_STORE_NO_CRYPT_RELEASE_FLAG
				// indicating that the cryptographic 
				// context aquired should not
				// be released when the function finishes.
				pCryptKeyProvInfo   // A pointer to a data structure that holds
				// infomation on the private key container to
				// be associated with this certificate.
				))
			{
				// Set friendly names for the certificates
				//CRYPT_DATA_BLOB tpFriendlyName	= {0, 0};
				//unsigned long	ulID			= 0;

				if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
				{
					ulID = 0x03;
				}
				else
				{
					ulID = 0x02;
				}

				/*QString			strFriendlyName;
				strFriendlyName = QString::fromUtf8(cert.getOwnerName());
				int iFriendLen = (strFriendlyName.length() + 1) * sizeof(QChar);

				tpFriendlyName.pbData = new unsigned char[iFriendLen];

				memset(tpFriendlyName.pbData, 0, iFriendLen);
				memcpy(tpFriendlyName.pbData, strFriendlyName.utf16(), iFriendLen - sizeof(QChar));

				tpFriendlyName.cbData = iFriendLen;

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
				{*/
				if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
				}
				else
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH);
				}
				CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL);
				//m_certContexts[readerName].push_back(pCertContext);
				//}
				/*
				if (NULL != tpFriendlyName.pbData)
				{
				delete [] (tpFriendlyName.pbData);
				tpFriendlyName.pbData = NULL;
				}*/
			}
			free	(cardSerialNumber);
			free (containerName);
		}
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMyStore = NULL;
	}
	return TRUE;
}

//*****************************************************
// store the authority certificates of the card
//*****************************************************
BOOL StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits)
{
	BOOL			bRet		 = FALSE;
	HCERTSTORE		hMemoryStore = NULL;   // memory store handle
	PCCERT_CONTEXT  pDesiredCert = NULL;

	if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData
		, pCertContext->pCertInfo->Subject.pbData
		, pCertContext->pCertInfo->Subject.cbData
		)
		)
	{
		hMemoryStore = CertOpenSystemStoreA ((HCRYPTPROV)NULL, "ROOT");
	}
	else
	{
		hMemoryStore = CertOpenSystemStoreA ((HCRYPTPROV)NULL, "CA");
	}

	if(NULL != hMemoryStore)
	{
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
			bRet = TRUE;
		}
		else
		{
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH);
			if(CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
			{
				bRet = TRUE;
			}
		}
		CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}
	return bRet;
}

//**************************************************
// Checks if older registered certificates are not
// still bound to the CSP when the minidriver is used
//**************************************************
BOOL ProviderNameCorrect (PCCERT_CONTEXT pCertContext )
{
	unsigned long dwPropId= CERT_KEY_PROV_INFO_PROP_ID; 
	DWORD cbData = 0;
	CRYPT_KEY_PROV_INFO * pCryptKeyProvInfo;

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


//*****************************************************
// store the user certificates 
//*****************************************************
BOOL DeleteIfUserCert (HWND hTextEdit,PCCERT_CONTEXT pCertContext)
{
	PCCERT_CONTEXT  pDesiredCert	= NULL;
	PCCERT_CONTEXT  pPrevCert		= NULL;
	HCERTSTORE		hMyStore = NULL;

	if( pCertContext != NULL)
	{
		unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
		CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

		if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	//First parameter is not used and should be set to NULL.
	hMyStore		= CertOpenSystemStore((HCRYPTPROV)NULL, "MY");

	if ( NULL != hMyStore )
	{
		// ----------------------------------------------------
		// search for a certificate with the same subject (contains name and NNR) in the store
		// If the certificate is not found --> NULL
		// ----------------------------------------------------
		do
		{
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject) , pPrevCert)))
			{
				// ----------------------------------------------------
				// certificate with the same subject (contains name and NNR) found,
				// so we remove it from the store
				// ----------------------------------------------------
				if(FALSE == CertDeleteCertificateFromStore(pDesiredCert))
				{
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"certificate deletion failed \r\n");
					if (E_ACCESSDENIED == GetLastError())
					{
						;
						//QString strCaption(tr("Deleting former certificate"));
						//QString strMessage(tr("Error deleting former certificate"));
						//QMessageBox::information(NULL,strCaption,strMessage);
					}
				}
				else
				{
					SendMessage(hTextEdit, EM_REPLACESEL,0,  (LPARAM)"certificate deleted \r\n");
				}
			}
			//pPrevCert = pDesiredCert;

		}
		while (NULL != pDesiredCert);
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
	}
	CertFreeCertificateContext(pDesiredCert);
	CertFreeCertificateContext(pCertContext);

	return TRUE;
}