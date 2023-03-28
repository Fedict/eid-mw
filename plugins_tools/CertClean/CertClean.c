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
#include "winreg.h"

int main()
{
	int retVal = 0;

#ifdef BEID_ALL_USERS
	retVal = CleanAllUsers();
#else
	retVal = CleanSingleUser();
#endif

	return retVal;
}

/*
	Search for, and remove if found, all Root signed Belgium root Ca certificates
	from HKLM, HKCU and all other users' cert stores
	*/
int CleanAllUsers(void)
{
	HCERTSTORE		hMemoryStore = NULL;   // memory store handle
	HKEY			hKeyHandle = NULL;
	DWORD			cSubKeys = 0;
	DWORD			cbMaxSubKeyLen = 0;

	//local machine store
	hMemoryStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (HCRYPTPROV_LEGACY)NULL,
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Local Machine CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}

	//current use store
	hMemoryStore = CertOpenSystemStoreA((HCRYPTPROV)NULL, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Current User CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}

	//find the userids:
	//open HKEY_USERS with query access
	if (RegOpenKeyExA(HKEY_USERS, NULL, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hKeyHandle) == ERROR_SUCCESS)
	{
		//if (hKeyHandle != NULL)
			//Query HKEY_USERS for userid's
		if (RegQueryInfoKeyA(hKeyHandle,//HKEY      hKey
			NULL, //LPSTR     lpClass
			NULL, //LPDWORD   lpcchClass,
			NULL, //LPDWORD   lpReserved
			&cSubKeys, //LPDWORD   lpcSubKeys,
			&cbMaxSubKeyLen, //LPDWORD   lpcbMaxSubKeyLen,
			NULL,	//LPDWORD   lpcbMaxClassLen,
			NULL,	//LPDWORD   lpcValues,
			NULL,	//LPDWORD   lpcbMaxValueNameLen,
			NULL,	//LPDWORD   lpcbMaxValueLen,
			NULL,	//LPDWORD   lpcbSecurityDescriptor,
			NULL	//PFILETIME lpftLastWriteTime
		) == ERROR_SUCCESS)
		{
			DWORD nameLen = cbMaxSubKeyLen + 1;
			LPSTR Name = (LPSTR)malloc(nameLen);
			if (Name == NULL)
				return -1;

			for (DWORD i = 0; i < cSubKeys; i++)
			{
				//loop through the subkeys
				//reset nameLen;
				nameLen = cbMaxSubKeyLen + 1;

				if (RegEnumKeyExA(
					hKeyHandle,	//HKEY      hKey,
					i,	//DWORD     dwIndex,
					Name,	//LPSTR     lpName,
					&nameLen,	//LPDWORD   lpcchName,
					NULL,	//LPDWORD   lpReserved,
					NULL,	//LPSTR     lpClass,
					NULL,	//LPDWORD   lpcchClass,
					NULL	//PFILETIME lpftLastWriteTime
				) == ERROR_SUCCESS)
				{
					if (nameLen == 0)
						return -2;

					if(nameLen < (cbMaxSubKeyLen + 1))
						Name[nameLen] = 0;
					else
						return -3;

					HKEY	CAKey = NULL;
					LPCSTR	CAPath = "\\SOFTWARE\\Microsoft\\SystemCertificates\\CA\\Certificates";
					DWORD	CAPathLen = (DWORD)strlen("\\SOFTWARE\\Microsoft\\SystemCertificates\\CA\\Certificates") + 1 ;

					DWORD subKeyLen = nameLen + CAPathLen;
					LPCSTR subKEy = (LPCSTR)malloc(subKeyLen);
					if (subKEy == NULL)
						return -1;

					strcpy_s((LPSTR)subKEy, subKeyLen, Name);
					strcat_s((LPSTR)subKEy, subKeyLen, CAPath);

					//check if CA cert store exists for this subkey
					if (RegOpenKeyExA(
						HKEY_USERS,	//HKEY   hKey,
						subKEy,	//LPCSTR lpSubKey,
						0,	//DWORD  ulOptions,
						KEY_QUERY_VALUE,	//REGSAM samDesired,
						&CAKey	//PHKEY  phkResult
					) == ERROR_SUCCESS)
					{
						printf("CA Cert store found in HKEY_USERS\\%s\n", subKEy);
						RegCloseKey(CAKey);
						//ok, CA cert exists, check for RS belgian rootCA's (Name is userID) and delete them

						DWORD userCALen = (DWORD)(nameLen + strlen("\\CA") + 1);
						LPCSTR userCA = (LPCSTR)malloc(userCALen);
						if (userCA == NULL)
							return -1;

						strcpy_s((LPSTR)userCA, userCALen, Name);
						strcat_s((LPSTR)userCA, userCALen, "\\CA");
						//userID store
						hMemoryStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (HCRYPTPROV_LEGACY)NULL,
							CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_USERS, userCA);//"userid\CA"
						if (hMemoryStore != NULL)
						{
							printf("Opened %s CA store\n", Name);
							CleanRSFromMemStore(hMemoryStore);
							CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
							hMemoryStore = NULL;
						}

						if (userCA != NULL)
						{
							free((LPSTR)userCA);
							userCA = NULL;
						}
					}
					if (subKEy != NULL)
					{
						free((LPSTR)subKEy);
						subKEy = NULL;
					}
				}
			}
			if (Name != NULL)
			{
				free(Name);
				Name = NULL;
			}
		}

	}
	return 0;
}

/* 
	Search for, and remove if found, all Root signed Belgium root Ca certificates
	from HKLM and HKCU cert stores
	*/
int CleanSingleUser (void)
{
	HCERTSTORE		hMemoryStore = NULL;   // memory store handle

//local machine store
	hMemoryStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (HCRYPTPROV_LEGACY)NULL,
		CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Local Machine CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}

	//current use store
	hMemoryStore = CertOpenSystemStoreA((HCRYPTPROV)NULL, "CA");
	if (hMemoryStore != NULL)
	{
		printf("Opened Current User CA store\n");
		CleanRSFromMemStore(hMemoryStore);
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}
	return 0;
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

	return 0;
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
							printf("E_ACCESSDENIED: cert in HKLM cert store?\n");//certs in HKLM are also shown in user stores, but cannot be removed from there
						else if (retVal == ERROR_ACCESS_DENIED)
							printf("ERROR_ACCESS_DENIED\n");//not enough permissions
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


