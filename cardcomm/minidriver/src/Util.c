/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2008-2009 FedICT.
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
/****************************************************************************************************/

#include "globmdrv.h"

#include "log.h"
#include "util.h"
#include "smartcard.h"

/****************************************************************************************************/

#define WHERE "BeidGetPubKey"
DWORD BeidGetPubKey(PCARD_DATA  pCardData, DWORD cbCertif, PBYTE pbCertif, DWORD *pcbPubKey, PBYTE *ppbPubKey)
{
	DWORD          dwReturn = 0;
	PCCERT_CONTEXT pCertContext = NULL; 
	DWORD          cbDecoded = 0;

#ifdef _DEBUG
	BLOBHEADER     BlobHeader;
	RSAPUBKEY      RsaPubKey;
#endif

	pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING,   // The encoding type
		pbCertif,            // The encoded data from the certificate retrieved
		cbCertif);           // The length of the encoded data
	if ( pCertContext == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertCreateCertificateContext() returned [0x%02x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	/* Calculate the needed memory for decoding the public key info */
	if ( !CryptDecodeObject(X509_ASN_ENCODING,
		RSA_CSP_PUBLICKEYBLOB,
		(((*((*pCertContext).pCertInfo)).SubjectPublicKeyInfo).PublicKey).pbData,
		(((*((*pCertContext).pCertInfo)).SubjectPublicKeyInfo).PublicKey).cbData,
		0,
		NULL,
		pcbPubKey) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CryptDecodeObjectEx() returned [0x%02x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	/* Allocate memory for the target buffer */
	*ppbPubKey = pCardData->pfnCspAlloc(*pcbPubKey);
	if ( *ppbPubKey == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbPubKey]");
		CLEANUP(SCARD_E_NO_MEMORY);
	}

	if ( !CryptDecodeObject(X509_ASN_ENCODING,
		RSA_CSP_PUBLICKEYBLOB,
		(((*((*pCertContext).pCertInfo)).SubjectPublicKeyInfo).PublicKey).pbData,
		(((*((*pCertContext).pCertInfo)).SubjectPublicKeyInfo).PublicKey).cbData,
		0,
		*ppbPubKey,
		pcbPubKey) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CryptDecodeObjectEx() returned [0x%02x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	/* The ALG_ID should be CALG_RSA_SIGN (0x00002400) */
	(*ppbPubKey)[5] = 0x24;

#ifdef _DEBUG
	/*
	* The data in memory is stored in little endian (the least significant byte (LSB) goes first), 
	* so the modulus and all other variables in the structure will follow that format.
	* http://msdn2.microsoft.com/en-us/library/aa387453(VS.85).aspx (BLOBHEADER)
	* http://msdn2.microsoft.com/en-us/library/aa387685(VS.85).aspx (RSAPUBKEY)
	*
	*/

	memcpy (&BlobHeader, *ppbPubKey, sizeof(BLOBHEADER));
	memcpy (&RsaPubKey, *ppbPubKey + sizeof(BLOBHEADER), sizeof(RSAPUBKEY));
#endif

cleanup:

	if ( pCertContext != NULL )
	{
		CertFreeCertificateContext(pCertContext);
		pCertContext = NULL;
	}

	return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidCreateMSRoots"
DWORD BeidCreateMSRoots(PCARD_DATA  pCardData, DWORD *pcbMSRoots, PBYTE *ppbMSRoots)
{
	DWORD          dwReturn = 0;

	DWORD          cbCertif;
	PBYTE          pbCertif;

	HCERTSTORE     hMemoryStore = NULL;         // A memory store handle
	PCCERT_CONTEXT pCertContext = NULL; 
	CERT_BLOB      blob;

	dwReturn = BeidReadCert(pCardData, CERT_CA, &cbCertif, &pbCertif);
	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadCert[CERT_CA] returned [%d]", dwReturn);
		CLEANUP(SCARD_E_UNEXPECTED);
	}

#ifdef _DEBUG
	LogDumpBin ("C:\\SmartCardMinidriverTest\\certca.crt", cbCertif, (char *)pbCertif);
#endif

	//-------------------------------------------------------------------
	//  Open a memory certificate store. After the store is open,
	//  certificates can be added to it.
	hMemoryStore = CertOpenStore(CERT_STORE_PROV_MEMORY,    // A memory store
		0,                         // Encoding type 
		// Not used with a memory store
		(HCRYPTPROV)NULL,          // Use the default provider
		0,                         // No flags
		NULL);                     // Not needed
	if ( hMemoryStore == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertOpenStore returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING,   // The encoding type
		pbCertif,            // The encoded data from the certificate retrieved
		cbCertif);           // The length of the encoded data
	if ( pCertContext == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertCreateCertificateContext() returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	//-------------------------------------------------------------------
	// Add the certificate from the My store to the new memory store.
	if( CertAddCertificateContextToStore(hMemoryStore,                // Store handle
		pCertContext,                // Pointer to a certificate
		CERT_STORE_ADD_REPLACE_EXISTING,
		NULL) == FALSE )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertAddCertificateContextToStore() returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	/* Clear Certificate Context */
	if ( pCertContext != NULL )
	{
		CertFreeCertificateContext(pCertContext);
		pCertContext = NULL;
	}
	if ( pbCertif != NULL )
	{
		pCardData->pfnCspFree(pbCertif);
		pbCertif = NULL;
	}

	dwReturn = BeidReadCert(pCardData, CERT_ROOTCA, &cbCertif, &pbCertif);
	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadCert[CERT_ROOTCA] returned [%d]", dwReturn);
		CLEANUP(SCARD_E_UNEXPECTED);
	}
#ifdef _DEBUG
	LogDumpBin ("C:\\SmartCardMinidriverTest\\certroot.crt", cbCertif, (char *)pbCertif);
#endif

	pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING,   // The encoding type
		pbCertif,            // The encoded data from the certificate retrieved
		cbCertif);           // The length of the encoded data
	if ( pCertContext == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertCreateCertificateContext() returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}
	//-------------------------------------------------------------------
	// Add the certificate from the My store to the new memory store.
	if( CertAddCertificateContextToStore(hMemoryStore,                // Store handle
		pCertContext,                // Pointer to a certificate
		CERT_STORE_ADD_REPLACE_EXISTING,
		NULL) == FALSE )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertAddCertificateContextToStore() returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}

	/* Get Needed memory size */
	blob.cbData = 0;
	blob.pbData = NULL;
	if ( CertSaveStore(hMemoryStore,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		CERT_STORE_SAVE_AS_PKCS7,
		CERT_STORE_SAVE_TO_MEMORY,
		&blob,
		0) == FALSE )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "CertSaveStore() returned [0x%x]", GetLastError());
		CLEANUP(SCARD_E_UNEXPECTED);
	}
	*pcbMSRoots = blob.cbData;
	if (ppbMSRoots != NULL) 
	{
		*ppbMSRoots = pCardData->pfnCspAlloc(blob.cbData);
		if ( *ppbMSRoots == NULL )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbMSRoots]");
			CLEANUP(SCARD_E_NO_MEMORY);
		}

		blob.pbData = *ppbMSRoots;
		if ( CertSaveStore(hMemoryStore,
			X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			CERT_STORE_SAVE_AS_PKCS7,
			CERT_STORE_SAVE_TO_MEMORY,
			&blob,
			0) == FALSE )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "CertSaveStore() returned [0x%x]", GetLastError());
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}


cleanup:
	if ( pCertContext != NULL )
	{
		CertFreeCertificateContext(pCertContext);
		pCertContext = NULL;
	}
	if ( pbCertif != NULL )
	{
		pCardData->pfnCspFree(pbCertif);
		pbCertif = NULL;
	}
	if ( hMemoryStore != NULL )
	{
		CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
	}

	return (dwReturn);
}

