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

#include "certpropimpl.h"
#include "certprop.h"

#include <tchar.h>
#include <string.h>


using namespace eIDMW;

DWORD ImportCertificates(LPCTSTR readerName, char * pCardSerialNumber, size_t cCardSerialNumber)
{
	char* szReaderName = NULL;

#ifdef UNICODE
	size_t count;
	count = wcstombs(NULL, readerName, 0) + 1; // number of bytes + 1 byte extra
	if (count == -1)
	{
		MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: Error in card reader name."));
		return false;
	}
	szReaderName = (char *)malloc( count );
	if(szReaderName == NULL)
	{
		return SCARD_E_NO_MEMORY;
	}
	count = wcstombs(szReaderName, readerName, count);
#else
	szReaderName = readerName;
#endif

	if ( !szReaderName || 0 == strlen(szReaderName) )
	{
#ifdef UNICODE
		if (szReaderName != NULL) {
			free(szReaderName);
			szReaderName = NULL;
		}
#endif
		MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: No reader name supplied."));
		return SCARD_E_UNKNOWN_READER;
	}

	PCCERT_CONTEXT		pCertContext = NULL;
	PCCERT_CONTEXT  	pCertContextIterator = NULL;
	DWORD				dwRet	= 1;
	bool				bSignatureContainerFound = false;
	bool				bAuthenticationContainerFound = false;

	const char * 		pInternalCardSerialNumber;
	wchar_t* 	 		pContainerNameSignature = NULL;
	wchar_t* 	 		pContainerNameAuthentication = NULL;
	HCERTSTORE 			hMyStore = NULL;
	unsigned long 		dwPropId;
	CRYPT_KEY_PROV_INFO *pCryptKeyProvInfo;
	DWORD            	cbData, dwLastError;

	try
	{
		CCardLayer cardlayer;
		CReader & cardreader = cardlayer.getReader(szReaderName);

		if (!cardreader.Connect())
		{
			MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: Unable to connect to card reader %s."),
				readerName);
#ifdef UNICODE
			if (szReaderName != NULL) {
				free(szReaderName);
				szReaderName = NULL;
			}
#endif
			return SCARD_E_READER_UNAVAILABLE;
		}
		std::string strSerial = cardreader.GetSerialNr();
		pInternalCardSerialNumber = strSerial.c_str();
		MWLOG(LEV_DEBUG, MOD_CSP,
			TEXT("ImportCertificates: Card serial number retrieved: %s."),
			pInternalCardSerialNumber);

		if (cCardSerialNumber < strlen(pInternalCardSerialNumber) + 1)
		{
			// buffer too small!
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("ImportCertificates: Buffer for card serial number too small. Buffer: %d, serial number length: %d"),
				cCardSerialNumber,
				strlen(pInternalCardSerialNumber) + 1);
		}
		else
		{
			strcpy (pCardSerialNumber, pInternalCardSerialNumber);
		}

		//
		// Look in certificate store for certificates linked to the smart card
		//

		dwPropId = CERT_KEY_PROV_INFO_PROP_ID;
		pCertContextIterator = NULL;

		hMyStore = CertOpenSystemStore(NULL, TEXT("MY"));
		if (hMyStore == NULL)
		{
			dwLastError = GetLastError();
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("ImportCertificates: Unable to open the system certificate store 'MY'. Error code: %d."),
				dwLastError);
#ifdef UNICODE
			if (szReaderName != NULL) {
				free(szReaderName);
				szReaderName = NULL;
			}
#endif
			return dwLastError;
		}
		while((pCertContextIterator = CertEnumCertificatesInStore(
			hMyStore,
			pCertContextIterator))) 	 // on the first call to the function,
			// this parameter is NULL
			// on all subsequent calls,
			// this parameter is the last pointer
			// returned by the function
		{
			if(!(CertGetCertificateContextProperty(
				pCertContextIterator, // A pointer to the certificate
				// where the property will be set.
				dwPropId,      		// An identifier of the property to get.
				// In this case,
				// CERT_KEY_PROV_INFO_PROP_ID
				NULL,          		// NULL on the first call to get the
				// length.
				&cbData)))     		// The number of bytes that must be
				// allocated for the structure.
			{
				dwLastError = GetLastError();
				MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: The property length for CERT_KEY_PROV_INFO_PROP_ID was not retrieved. Error code %d."),
					dwLastError);
				if(dwLastError == CRYPT_E_NOT_FOUND)
				{
					break;
				}
				CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
				return dwLastError;
			}
			if(!(pCryptKeyProvInfo =
				(CRYPT_KEY_PROV_INFO *)malloc(cbData)))
			{
				MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: Error in allocation of memory."));
#ifdef UNICODE
				if (szReaderName != NULL) {
					free(szReaderName);
					szReaderName = NULL;
				}
#endif
				CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
				return SCARD_E_NO_MEMORY;
			}
			if(CertGetCertificateContextProperty(
				pCertContextIterator,
				dwPropId,
				pCryptKeyProvInfo,
				&cbData))
			{
				MWLOG(LEV_DEBUG, MOD_CSP, TEXT("ImportCertificates: We are investigating key container %s."),
					pCryptKeyProvInfo->pwszContainerName);

				pContainerNameSignature    		= new(std::nothrow) wchar_t[strlen(pInternalCardSerialNumber)+12];
				if (pContainerNameSignature == NULL)
				{
#ifdef UNICODE
					if (szReaderName != NULL) {
						free(szReaderName);
						szReaderName = NULL;
					}
#endif
					CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
					return SCARD_E_NO_MEMORY;
				}

				pContainerNameAuthentication    = new(std::nothrow) wchar_t[strlen(pInternalCardSerialNumber)+17];
				if (pContainerNameAuthentication == NULL)
				{
					CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
					return SCARD_E_NO_MEMORY;
				}
				swprintf(pContainerNameSignature,                 // Signature key container name to look for
					TEXT("Signature(%S)"), pInternalCardSerialNumber);
				swprintf(pContainerNameAuthentication,            // Authentication key container name to look for
					TEXT("Authentication(%S)"), pInternalCardSerialNumber);

				// look for signature key container
				bSignatureContainerFound =
					(0 == wcscmp(pCryptKeyProvInfo->pwszContainerName, pContainerNameSignature)) ||      // Signature key container name found
					bSignatureContainerFound;
				if (bSignatureContainerFound)
					MWLOG(LEV_DEBUG, MOD_CSP, TEXT("ImportCertificates: We found key container %s."),
					pContainerNameSignature);

				// look for authentication key container
				bAuthenticationContainerFound =
					(0 == wcscmp(pCryptKeyProvInfo->pwszContainerName, pContainerNameAuthentication)) || // Authentication key container name
					bAuthenticationContainerFound;
				if (bAuthenticationContainerFound)
					MWLOG(LEV_DEBUG, MOD_CSP, TEXT("ImportCertificates: We found key container %s."),
					pContainerNameAuthentication);

				if (pContainerNameSignature)
					delete pContainerNameSignature;
				if (pContainerNameAuthentication)
					delete pContainerNameAuthentication;

				if (pCryptKeyProvInfo)
					free(pCryptKeyProvInfo);
				// certificate with this container name found
				if (bSignatureContainerFound &&
					bAuthenticationContainerFound) {
						break;
				}
			}
			else
			{
				if (pCryptKeyProvInfo)
					free(pCryptKeyProvInfo);
				MWLOG(LEV_INFO, MOD_CSP, TEXT("ImportCertificates: The property CERT_KEY_PROV_INFO_PROP_ID was not retrieved."));
			}

		} // End of while.
		if(!CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG))
		{
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("ImportCertificates: Unable to close the system certificate store 'MY'. Error code: %d."), GetLastError());
		}
		hMyStore = NULL;

		if (!(bSignatureContainerFound && bAuthenticationContainerFound))
		{
			//
			// At least one certificate not found - we'll add the certificates
			//
			for (unsigned long CertIdx=0;CertIdx<cardreader.CertCount();CertIdx++)
			{
				eIDMW::tCert	 cert;
				eIDMW::CByteArray       certData;
				cert			= cardreader.GetCert(CertIdx);
				certData        = cardreader.ReadFile(cert.csPath);
				// ----------------------------------------------------
				// create the certificate context with the certificate raw data
				// ----------------------------------------------------
				pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

				if( pCertContext == NULL )
				{
					dwLastError = GetLastError();
					if (dwLastError == E_INVALIDARG)
						MWLOG(LEV_INFO, MOD_CSP,
						TEXT("ImportCertificates: Unable to create certificate context. The certificate encoding type is not supported."),
						dwLastError);
					else
						MWLOG(LEV_INFO, MOD_CSP,
						TEXT("ImportCertificates: Unable to create certificate context. Error code: %d."),
						dwLastError);
				}
				else
				{
					unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
					CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

					// ----------------------------------------------------
					// Only store the context of the certificates with usages for an end-user
					// i.e. no CA or root certificates
					// ----------------------------------------------------
					if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
					{
						dwRet = StoreAuthorityCerts (pCertContext, KeyUsageBits);
					}
					else
					{
						dwRet = StoreUserCerts (pCertContext, KeyUsageBits, pInternalCardSerialNumber);
					}
					if (pCertContext)
						CertFreeCertificateContext(pCertContext);
				}
			}
		}
	}
	catch (eIDMW::CMWException e)
	{
		long err = e.GetError();
#ifdef UNICODE
		if (szReaderName != NULL) {
			free(szReaderName);
			szReaderName = NULL;
		}
#endif
		MWLOG(LEV_INFO, MOD_CSP,
			TEXT("ImportCertificates: An eID Middleware error was thrown. Error code: %d."),
			err);
		if(hMyStore != NULL)
			CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return err;
	}
#ifdef UNICODE
	if (szReaderName != NULL) {
		free(szReaderName);
		szReaderName = NULL;
	}
#endif
	return dwRet;
}

DWORD RemoveCertificates (char * pSerialNumber)
{
	PCCERT_CONTEXT  	pCertContextIterator = NULL;
	PCCERT_CONTEXT  	pCertContextDup      = NULL;


	wchar_t* 	 		pContainerNameSignature;
	wchar_t* 	 		pContainerNameAuthentication;
	HCERTSTORE 			hMyStore;
	unsigned long 		dwPropId;
	CRYPT_KEY_PROV_INFO *pCryptKeyProvInfo;
	DWORD            	cbData, dwLastError;

	DWORD RemoveCert;
	HKEY hkSubKey;
	bool settingFound = false;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Software\\BEID\\configuretool"),0,KEY_READ,&hkSubKey))
	{
		cbData = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueEx(hkSubKey,TEXT("remove_certificate"), 
			NULL, NULL,(LPBYTE) &RemoveCert, &cbData))
		{
			settingFound = true;
		}
		RegCloseKey(hkSubKey);
	}
	if (!settingFound)
	{
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("Software\\BEID\\configuretool"),0,KEY_READ,&hkSubKey))
		{
			cbData = sizeof(DWORD);
			if (ERROR_SUCCESS == RegQueryValueEx (hkSubKey,TEXT("remove_certificate"), NULL, NULL,
				(LPBYTE) &RemoveCert, &cbData))
			{
				settingFound = true;
			}
			RegCloseKey(hkSubKey);
		}
	}

	if ( settingFound && 0 == RemoveCert )
	{
		// setting disabled
		return false;
	}
	hMyStore = CertOpenSystemStore(NULL, TEXT("MY"));

	if(hMyStore == NULL)
	{
		// couldn't open store
		return false;
	}
	dwPropId = CERT_KEY_PROV_INFO_PROP_ID;
	pCertContextIterator = NULL;

	while((pCertContextIterator = CertEnumCertificatesInStore(
		hMyStore,
		pCertContextIterator))) 	 // on the first call to the function,
		// this parameter is NULL
		// on all subsequent calls,
		// this parameter is the last pointer
		// returned by the function
	{
		cbData = 0;
		if(!(CertGetCertificateContextProperty(
			pCertContextIterator, // A pointer to the certificate
			// where the property will be set.
			dwPropId,      	// An identifier of the property to get.
			// In this case,
			// CERT_KEY_PROV_INFO_PROP_ID
			NULL,          	// NULL on the first call to get the
			// length.
			&cbData)))     	// The number of bytes that must be
			// allocated for the structure.
		{
			dwLastError = GetLastError();
			MWLOG(LEV_INFO, MOD_CSP, TEXT("RemoveCertificates: The property length for CERT_KEY_PROV_INFO_PROP_ID was not retrieved. Error code %d."),
				dwLastError);
				if(dwLastError == CRYPT_E_NOT_FOUND)
				{
					break;
				}
				CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
				return dwLastError;
		}
		if(!(pCryptKeyProvInfo =
			(CRYPT_KEY_PROV_INFO *)malloc(cbData)))
		{
			MWLOG(LEV_INFO, MOD_CSP, TEXT("RemoveCertificates: Error in allocation of memory."));
			CertCloseStore(hMyStore,CERT_CLOSE_STORE_FORCE_FLAG);
			return SCARD_E_NO_MEMORY;
		}
		if(CertGetCertificateContextProperty(
			pCertContextIterator,
			dwPropId,
			pCryptKeyProvInfo,
			&cbData))
		{
			MWLOG(LEV_DEBUG, MOD_CSP, TEXT("RemoveCertificates: We are investigating key container %s."),
				pCryptKeyProvInfo->pwszContainerName);

			pContainerNameSignature    		= new wchar_t[strlen(pSerialNumber)+12];
			pContainerNameAuthentication    = new wchar_t[strlen(pSerialNumber)+17];

			swprintf(pContainerNameSignature,                 // Signature key container name to look for
				TEXT("Signature(%S)"), pSerialNumber);
			swprintf(pContainerNameAuthentication,            // Authentication key container name to look for
				TEXT("Authentication(%S)"), pSerialNumber);

			if (0 == wcscmp(pCryptKeyProvInfo->pwszContainerName, pContainerNameSignature) ||      // Signature key container name found
				0 == wcscmp(pCryptKeyProvInfo->pwszContainerName, pContainerNameAuthentication) )  // Authentication key container name found
			{
				if (!(pCertContextDup = CertDuplicateCertificateContext(pCertContextIterator)))
				{
					dwLastError = GetLastError();
					MWLOG(LEV_INFO, MOD_CSP, TEXT("RemoveCertificates: Duplication of the certificate pointer failed. Error code: %d"),
						dwLastError);

					// Free resources
					if (pCryptKeyProvInfo)
						free(pCryptKeyProvInfo);
					if (pCertContextIterator)
						CertFreeCertificateContext(pCertContextIterator);
					if(pContainerNameSignature != NULL)
						delete pContainerNameSignature;
					if(pContainerNameAuthentication != NULL)
						delete pContainerNameAuthentication;


					CertCloseStore(hMyStore,CERT_CLOSE_STORE_FORCE_FLAG);
					return dwLastError;
				}
				//-------------------------------------------------------------------
				// Delete the certificate.
				if(!CertDeleteCertificateFromStore(pCertContextDup))
				{
					dwLastError = GetLastError();
					MWLOG(LEV_INFO, MOD_CSP, TEXT("RemoveCertificates: The deletion of the certificate failed. Error code: %d"),
						dwLastError);

					// Free resources
					if (pCryptKeyProvInfo)
						free(pCryptKeyProvInfo);
					if (pCertContextIterator)
						CertFreeCertificateContext(pCertContextIterator);
					if(pContainerNameSignature != NULL)
						delete pContainerNameSignature;
					if(pContainerNameAuthentication != NULL)
						delete pContainerNameAuthentication;

					CertCloseStore(hMyStore,CERT_CLOSE_STORE_FORCE_FLAG);
					return dwLastError;
				}
				MWLOG(LEV_DEBUG, MOD_CSP, TEXT("RemoveCertificates: Certificate removed from store."));
			}
			if(pContainerNameSignature != NULL)
				delete pContainerNameSignature;
			if(pContainerNameAuthentication != NULL)
				delete pContainerNameAuthentication;
		}
		else
		{
			MWLOG(LEV_INFO, MOD_CSP, TEXT("RemoveCertificates: The property CERT_KEY_PROV_INFO_PROP_ID was not retrieved."));
		}
		if (pCryptKeyProvInfo)
			free(pCryptKeyProvInfo);
	} // End of while.
	CertCloseStore(hMyStore,CERT_CLOSE_STORE_FORCE_FLAG);
	return true;
}

DWORD StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits)
{

	DWORD dwRet = 0;
	HCERTSTORE hMemoryStore = NULL; // memory store handle
	PCCERT_CONTEXT pDesiredCert = NULL;


	if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData
		, pCertContext->pCertInfo->Subject.pbData
		, pCertContext->pCertInfo->Subject.cbData
		)
		)
	{
		hMemoryStore = CertOpenSystemStore (NULL, TEXT("ROOT"));
	}
	else
	{
		hMemoryStore = CertOpenSystemStore (NULL, TEXT("CA"));
	}

	if (hMemoryStore == NULL)
	{
		dwRet = GetLastError();
		MWLOG(LEV_INFO, MOD_CSP,
			TEXT("StoreAuthorityCerts: Unable to open the system certificate store. Error code: %d."),
			dwRet);
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
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("StoreUserCerts: Certificate context added to store."));
			dwRet = 0;
		}
		else
		{
			dwRet = GetLastError();
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("StoreAuthorityCerts: Unable to add certificate context to store. Error code: %d."),
				dwRet);
		}
	}
	CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
	hMemoryStore = NULL;

	return dwRet;

}

//*****************************************************
// store the user certificates of the card in a specific reader
//*****************************************************
DWORD StoreUserCerts (PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* cardSerialNumber)
{
	unsigned long dwFlags 		= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
	DWORD dwRet					= 0;

	PCCERT_CONTEXT pDesiredCert = NULL;
	HCERTSTORE hMyStore 		= CertOpenSystemStore(NULL, TEXT("MY"));

	if (hMyStore == NULL)
	{
		dwRet = GetLastError();
		MWLOG(LEV_INFO, MOD_CSP,
			TEXT("StoreUserCerts: Unable to open the system certificate store. Error code: %d."),
			dwRet);
		return dwRet;
	}
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

	// ----------------------------------------------------
	// Initialize the CRYPT_KEY_PROV_INFO data structure.
	// Note: pwszContainerName and pwszProvName can be set to NULL
	// to use the default container and provider.
	// ----------------------------------------------------
	CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo = new(std::nothrow) CRYPT_KEY_PROV_INFO;
	if(pCryptKeyProvInfo == NULL)
	{
		MWLOG(LEV_ERROR, MOD_CSP,
			TEXT("StoreUserCerts: Unable to open the system certificate store. Error code: %d."),
			dwRet);
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return 0;
	}
	unsigned long dwPropId = CERT_KEY_PROV_INFO_PROP_ID;

	// ----------------------------------------------------
	// Get the serial number
	// ----------------------------------------------------
	wchar_t* 	 pContainerName = new(std::nothrow) wchar_t[strlen(cardSerialNumber)+17];
	if(pContainerName == NULL)
	{
		MWLOG(LEV_ERROR, MOD_CSP,
			TEXT("StoreUserCerts: Unable to open the system certificate store. Error code: %d."),
			dwRet);
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return 0;
	}

	if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
		swprintf(pContainerName,TEXT("Signature(%S)"), cardSerialNumber);
	else
		swprintf(pContainerName,TEXT("Authentication(%S)"), cardSerialNumber);

	MWLOG(LEV_INFO, MOD_CSP,
		TEXT("StoreUserCerts: Try to add certificate to store with key container name %s."),
		pContainerName);

	pCryptKeyProvInfo->pwszContainerName = pContainerName;
	pCryptKeyProvInfo->pwszProvName = TEXT("Belgium Identity Card CSP");
	pCryptKeyProvInfo->dwProvType = PROV_RSA_FULL;
	pCryptKeyProvInfo->dwFlags = 0;
	pCryptKeyProvInfo->cProvParam = 0;
	pCryptKeyProvInfo->rgProvParam = NULL;
	pCryptKeyProvInfo->dwKeySpec = AT_KEYEXCHANGE;

	// Set the property.
	if (CertSetCertificateContextProperty(
		pCertContext, 		// A pointer to the certificate
		// where the property will be set.
		dwPropId, 			// An identifier of the property to be set.
		// In this case, CERT_KEY_PROV_INFO_PROP_ID
		// is to be set to provide a pointer with the
		// certificate to its associated private key
		// container.
		dwFlags, 			// The flag used in this case is
		// CERT_STORE_NO_CRYPT_RELEASE_FLAG
		// indicating that the cryptographic
		// context acquired should not
		// be released when the function finishes.
		pCryptKeyProvInfo 	// A pointer to a data structure that holds
		// infomation on the private key container to
		// be associated with this certificate.
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
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("StoreUserCerts: Certificate context added to store."));
			dwRet = 0;
		}
		else
		{
			dwRet = GetLastError();
			MWLOG(LEV_INFO, MOD_CSP,
				TEXT("StoreUserCerts: Unable to add certificate context to store. Error code: %d."),
				dwRet);
		}
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMyStore = NULL;

	}
	if (pCryptKeyProvInfo != NULL)
	{
		delete pCryptKeyProvInfo;
		pCryptKeyProvInfo = NULL;
	}
	if(pContainerName != NULL)
		delete pContainerName;

	return dwRet;
}

DWORD CertProp() {
	SCARDCONTEXT    	hSC;
	LONG            	lReturn;
	LONG            	lReturn2;
	DWORD           	cch = SCARD_AUTOALLOCATE;
	DWORD             dwI, dwIPrev, dwRdrCount,dwChanges;
	LPTSTR          	pmszReaders = NULL;
	LPTSTR          	pmszReadersPrev = NULL;
	LPTSTR          	pReader;

	char							userData[MAX_CARD_SERIAL_SIZE];
	SCARD_READERSTATE 	rgscState[MAXIMUM_SMARTCARD_READERS];
	SCARD_READERSTATE 	rgscStatePrev[MAXIMUM_SMARTCARD_READERS];

	memset (rgscState, 0, sizeof (SCARD_READERSTATE) * MAXIMUM_SMARTCARD_READERS);
	memset (rgscStatePrev, 0, sizeof (SCARD_READERSTATE) * MAXIMUM_SMARTCARD_READERS);

	lReturn = SCardEstablishContext (SCARD_SCOPE_SYSTEM,
		NULL,
		NULL,
		&hSC);
	if ( SCARD_S_SUCCESS != lReturn ) {
		MWLOG(LEV_INFO, MOD_CSP, TEXT("CertProp: Failed SCardEstablishContext: 0x%08X"), lReturn);
		return lReturn;
	}
	for (;;) {
		// Retrieve the list the readers.
		// hSC was set by a previous call to SCardEstablishContext.
		pmszReaders = NULL;
		cch = SCARD_AUTOALLOCATE;
		lReturn = SCardListReaders(hSC,
			NULL,
			(LPTSTR)&pmszReaders,
			&cch );
		switch( lReturn )
		{
		case SCARD_E_NO_READERS_AVAILABLE:
			// Wait and try again.
			Sleep(CARDREADER_POLL_INTERVAL);
			break;
		case SCARD_S_SUCCESS:
			// Do something with the multi string of readers.
			// Output the values.
			// A double-null terminates the list of values.
			pReader = pmszReaders;
			for ( dwI = 0; dwI < MAXIMUM_SMARTCARD_READERS; dwI++ )
			{
				if ( '\0' == *pReader )
					break;

				rgscState[dwI].szReader = pReader;
				rgscState[dwI].dwCurrentState = SCARD_STATE_UNAWARE;
				// get previous state for already connected readers
				for ( dwIPrev = 0; dwIPrev < MAXIMUM_SMARTCARD_READERS; dwIPrev++ )
				{
					if (0 == lstrcmp(rgscState[dwI].szReader, rgscStatePrev[dwIPrev].szReader))
					{
						rgscState[dwI].dwCurrentState = rgscStatePrev[dwIPrev].dwCurrentState;
						break;
					}
				}
				pReader = pReader + _tcslen(pReader) + 1;
			}
			dwRdrCount = dwI;
			// If any readers are available, proceed.
			if ( 0 != dwRdrCount ) {
				Sleep(100);
				lReturn = SCardGetStatusChange(hSC,
					CARDREADER_POLL_INTERVAL,
					//		INFINITE,                // infinite wait - does not work as there is
					// no status change when a card reader is attached
					// so we have to poll
					rgscState,
					dwRdrCount );
				Sleep(100);
				// see if service is stopped
				//if (WaitForSingleObject( ghSvcStopEvent,0) == WAIT_OBJECT_0)
				//	return 0;

				for ( dwI=0; dwI < dwRdrCount; dwI++)
				{
					dwChanges = (rgscState[dwI].dwEventState ^ rgscState[dwI].dwCurrentState) & MAXWORD;

					if (  SCARD_STATE_PRESENT & rgscState[dwI].dwEventState & dwChanges
						&& !(SCARD_STATE_MUTE & rgscState[dwI].dwEventState & dwChanges))  //
					{
						//
						// Card inserted
						//
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card inserted in reader '%s'.\n"),
							rgscState[dwI].szReader );
						rgscState[dwI].pvUserData = userData;
						ImportCertificates(rgscState[dwI].szReader, (char*)rgscState[dwI].pvUserData, MAX_CARD_SERIAL_SIZE);
					}
					if ((  SCARD_STATE_IGNORE  & rgscState[dwI].dwEventState & dwChanges
						&& SCARD_STATE_UNAVAILABLE & rgscState[dwI].dwEventState & dwChanges)  // card reader removed
						|| SCARD_STATE_EMPTY & rgscState[dwI].dwEventState & dwChanges)            // card removed from reader
					{
						//
						// Card or card reader removed
						//
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card or card reader '%s' removed.\n"),
							rgscState[dwI].szReader );
						if (rgscState[dwI].pvUserData != NULL)
						{
							RemoveCertificates ((char*)rgscState[dwI].pvUserData);
						}
					}

#ifdef DEBUG
					if (  SCARD_STATE_PRESENT & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_PRESENT     for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_IGNORE & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_IGNORE      for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_CHANGED & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_CHANGED     for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_UNKNOWN & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_UNKNOWN     for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_UNAVAILABLE & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_UNAVAILABLE for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_EMPTY & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_EMPTY       for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_EXCLUSIVE & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_EXCLUSIVE   for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_INUSE & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_INUSE       for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
					if (  SCARD_STATE_MUTE & rgscState[dwI].dwEventState & dwChanges)
					{
						MWLOG(LEV_DEBUG, MOD_CSP, TEXT("CertProp: Card status changed to SCARD_STATE_MUTE        for reader '%s'.\n"),
							rgscState[dwI].szReader );
					}
#endif
					rgscState[dwI].dwCurrentState = rgscState[dwI].dwEventState;
				}
			}
			break;
		default:
			// SCardListReaders failed.
			MWLOG(LEV_INFO, MOD_CSP, TEXT("CertProp: Failed SCardListReaders: 0x%08X"), lReturn);
			return lReturn;
		}// end switch
		if (pmszReadersPrev != NULL)
		{
			// free previous readers
			lReturn2 = SCardFreeMemory( hSC,
				pmszReadersPrev );
			if ( SCARD_S_SUCCESS != lReturn2 )
			{
				MWLOG(LEV_INFO, MOD_CSP, TEXT("CertProp: Failed SCardFreeMemory: 0x%08X"), lReturn2);
			}
		}
		// keep previous readers pointer to free the memory later on
		pmszReadersPrev = pmszReaders;

		// copy states for next round
		memcpy(rgscStatePrev, rgscState, sizeof(SCARD_READERSTATE) * MAXIMUM_SMARTCARD_READERS);

	}// end while


	// make sure we clean up on exit
	// should be placed elsewhere...
	if (pmszReadersPrev != NULL){
		lReturn2 = SCardFreeMemory( hSC,
			pmszReadersPrev );
		if ( SCARD_S_SUCCESS != lReturn2 )
		{
			MWLOG(LEV_INFO, MOD_CSP, TEXT("CertProp: Failed SCardFreeMemory: 0x%08X"), lReturn2);
		}
	}
	// free previous readers
	if (pmszReaders != NULL) {
		lReturn2 = SCardFreeMemory( hSC,
			pmszReaders );
		if ( SCARD_S_SUCCESS != lReturn2 )
		{
			MWLOG(LEV_INFO, MOD_CSP, TEXT("CertProp: Failed SCardFreeMemory: 0x%08X"), lReturn2);
		}
	}
}
