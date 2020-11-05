// CertClean.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "CertClean.h"

#include <stdio.h>

int main()
{
	int retVal = 0;
/* Search for, and remove if found, all Root signed Belgium root Ca certificates*/

	HCERTSTORE		hMemoryStore = NULL;   // memory store handle
	//current use store
	hMemoryStore = CertOpenSystemStoreA((HCRYPTPROV)NULL, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Current User CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
	}

	//local machine store
	hMemoryStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (HCRYPTPROV_LEGACY)NULL,
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Local Machine CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
	}
	hMemoryStore = NULL;

	return retVal;
}

/*	Remove Root Signed Belgium Root CA's from the given memory store
	caller may not provide NULL as memory store*/
int CleanRSFromMemStore(HCERTSTORE hMemoryStore)
{
	printf("Search for BELGIUM_ROOTCA1_RS\n");
	CleanCertificate(hMemoryStore, BELGIUM_ROOTCA1_RS, sizeof(BELGIUM_ROOTCA1_RS));
	printf("Search for BELGIUM_ROOTCA2_RS\n");
	CleanCertificate(hMemoryStore, BELGIUM_ROOTCA2_RS, sizeof(BELGIUM_ROOTCA2_RS));
	printf("Search for BELGIUM_ROOTCA3_RS\n");
	CleanCertificate(hMemoryStore, BELGIUM_ROOTCA3_RS, sizeof(BELGIUM_ROOTCA3_RS));
	printf("Search for BELGIUM_ROOTCA4_RS\n");
	CleanCertificate(hMemoryStore, BELGIUM_ROOTCA4_RS, sizeof(BELGIUM_ROOTCA4_RS));
}

/* Remove the specified certificate from the specified cert store*/
int CleanCertificate(HCERTSTORE hMemoryStore, BYTE* pbcertificateData, DWORD dwcertificateDataLen)
{
	int				retVal = 0;
	PCCERT_CONTEXT  pDesiredCert = NULL;
	PCCERT_CONTEXT  pPrevCert = NULL;

	if (!pbcertificateData || dwcertificateDataLen == 0)
	{
		return -3;
	}

	// create the certificate context with the certificate raw data
	const PCCERT_CONTEXT pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbcertificateData, dwcertificateDataLen);

	if (pCertContext != NULL)
	{
		// look if we have a certificate with the same subject (contains name and NNR) in the store
		// If the certificate is not found --> NULL
		do
		{
			pDesiredCert = CertFindCertificateInStore(hMemoryStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject), pPrevCert);
			if (pDesiredCert != NULL)
			{
				// Check if the certificates are identical
				if (CertCompareCertificate(X509_ASN_ENCODING, pCertContext->pCertInfo, pDesiredCert->pCertInfo))
				{
					printf("certificate found\n");
					// certificates are identical, so we remove the one from the store
					if (CertDeleteCertificateFromStore(pDesiredCert) == FALSE)
					{
						retVal = GetLastError(); //E_ACCESSDENIED
						printf("CertDeleteCertificateFromStore failed with 0x%.8x\n", retVal);
						if (retVal == E_ACCESSDENIED)
							printf("E_ACCESSDENIED\n");
						else if (retVal == ERROR_ACCESS_DENIED)
							printf("ERROR_ACCESS_DENIED\n");
						//break here to avoid looping for ever
						break;
					}
					else
					{
						printf("certificate removed\n");
					}
					//pDesiredCert and pPrevCert are freed, so set pPrevCert = NULL and search again
					pPrevCert = NULL;
					continue;
				}
			}
			pPrevCert = pDesiredCert;

		} while (NULL != pDesiredCert);

		CertFreeCertificateContext(pCertContext);
	}
	return retVal;
}


