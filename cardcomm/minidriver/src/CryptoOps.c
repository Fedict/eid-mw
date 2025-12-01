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
#include "smartcard.h"

/****************************************************************************************************/
#define __USE_SDK__

#ifndef __USE_SDK__
typedef struct _BCRYPT_PKCS1_PADDING_INFO 
{  
	LPCWSTR pszAlgId;
} BCRYPT_PKCS1_PADDING_INFO;

typedef struct _BCRYPT_PSS_PADDING_INFO 
{  
	LPCWSTR  pszAlgId;  
	ULONG    cbSalt;
} BCRYPT_PSS_PADDING_INFO;
#endif

/****************************************************************************************************/

//
// Function: CardRSADecrypt
//
// Purpose: Perform a private key decryption on the supplied data.  The
//          card module should assume that pbData is the length of the
//          key modulus.
//

#define WHERE "CardRSADecrypt()"
DWORD WINAPI   CardRSADecrypt
	(
	__in        PCARD_DATA              pCardData,
	__inout     PCARD_RSA_DECRYPT_INFO  pInfo
	)
{
	DWORD    dwReturn = 0;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
	/* 
	 * for ECC - only smart cards, this entry point is not defined and is
	 * set to NULL in the CARD_DATA structure returned from CardAcquireContext
	*/
	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function:  CardConstructDHAgreement
//
// Purpose: compute a DH secret agreement from a ECDH key on the card
// and the public portion of another ECDH key
//

#define WHERE "CardConstructDHAgreement()"
DWORD WINAPI   CardConstructDHAgreement
	(
	__in     PCARD_DATA pCardData,
	__in     PCARD_DH_AGREEMENT_INFO pAgreementInfo
	)
{
	DWORD    dwReturn = 0;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/* 
	* For RSA-only card minidrivers, this entry point is not defined and is
	* set to NULL in the CARD_DATA structure returned from CardAcquireContext
	*/
	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function:  CardDeriveKey
//
// Purpose: Generate a dervived session key using a generated agreed 
// secret and various other parameters.
//

#define WHERE "CardDeriveKey()"
DWORD WINAPI CardDeriveKey
	(
	__in    PCARD_DATA        pCardData,
	__in    PCARD_DERIVE_KEY  pAgreementInfo
	)
{
	DWORD    dwReturn = 0;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/* 
	* For RSA-only card minidrivers, this entry point is not defined and is
	* set to NULL in the CARD_DATA structure returned from CardAcquireContext
	*/
	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function:  CardDestroyDHAgreement
//

#define WHERE "CardDestroyDHAgreement()"
DWORD WINAPI   CardDestroyDHAgreement
	(
	__in PCARD_DATA pCardData,
	__in BYTE       bSecretAgreementIndex,
	__in DWORD      dwFlags
	)
{
	DWORD    dwReturn = 0;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/* 
	* For RSA-only card minidrivers, this entry point is not defined and is
	* set to NULL in the CARD_DATA structure returned from CardAcquireContext
	*/
	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardSignData
//
// Purpose: Sign input data using a specified key
//

#define WHERE "CardSignData()"
DWORD WINAPI   CardSignData
	(
	__in      PCARD_DATA          pCardData,
	__in      PCARD_SIGNING_INFO  pInfo
	)
{
	DWORD                      dwReturn       = 0;

	BCRYPT_PKCS1_PADDING_INFO  *PkcsPadInfo = NULL;
	BCRYPT_PSS_PADDING_INFO    *PssPadInfo  = NULL;

	unsigned int               uiHashAlgo   = HASH_ALGO_NONE;

	BYTE	bKeyNr = BELPIC_KEY_NON_REP;
	BYTE	bAlgoRef = BELPIC_SIGN_ALGO_RSASSA_PKCS1;

	VENDOR_SPECIFIC* pVendorSpec;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( pInfo == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( ( pInfo->dwVersion != CARD_SIGNING_INFO_BASIC_VERSION   ) &&
		( pInfo->dwVersion != CARD_SIGNING_INFO_CURRENT_VERSION ) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwVersion][0x%X]", pInfo->dwVersion);
		CLEANUP(ERROR_REVISION_MISMATCH);
	}

	if ( pInfo->pbData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->pbData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( pInfo->bContainerIndex == 0 )
	{
		bKeyNr = BELPIC_KEY_AUTH; //see CardGetContainerProperty	
	}
	else if (pInfo->bContainerIndex == 1 )
	{
		bKeyNr = BELPIC_KEY_NON_REP; //see CardGetContainerProperty	
	}
	else
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->bContainerIndex]");
		CLEANUP(SCARD_E_NO_KEY_CONTAINER);
	}

	pVendorSpec = pCardData->pvVendorSpecific;
	if (pVendorSpec->bBEIDCardType == BEID_ECC_CARD)
	{
		//bAlgoRef = BELPIC_SIGN_ALGO_ECDSA_SHA2_256;
		if ((pInfo->dwKeySpec == AT_ECDSA_P256) || (pInfo->dwKeySpec == AT_ECDSA_P384) || (pInfo->dwKeySpec == AT_ECDSA_P521))
		{
			if ((pInfo->dwKeySpec != AT_ECDSA_P256) && (pInfo->dwKeySpec != AT_ECDSA_P384))
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwKeySpec]");
				CLEANUP(SCARD_E_INVALID_PARAMETER);
			}
		}
		else
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Unsupported feature [pInfo->dwKeySpec]");
			CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
		}
	}
	else { //RSA card
		if (pInfo->dwKeySpec != AT_SIGNATURE)
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwKeySpec]");
			CLEANUP(SCARD_E_INVALID_PARAMETER);
		}
	}


	if ( pInfo->dwSigningFlags == 0xFFFFFFFF )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwSigningFlags]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( ( pInfo->dwSigningFlags & CARD_BUFFER_SIZE_ONLY ) == CARD_BUFFER_SIZE_ONLY)
	{
		LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwSigningFlags: CARD_BUFFER_SIZE_ONLY");
		if (pVendorSpec->bBEIDCardType == BEID_ECC_CARD)
		{
			if (pInfo->dwKeySpec == AT_ECDSA_P256)
			{
				pInfo->cbSignedData = 64;  // EC256 signature size
			}
			else
			{
				pInfo->cbSignedData = 96;  // EC384 signature size
			}
		}
		else
		{
			pInfo->cbSignedData = 256;
		}
		CLEANUP(SCARD_S_SUCCESS);
	}

	if ( pInfo->aiHashAlg == 0xFFFFFFFF )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->aiHashAlg][0x%X]",pInfo->aiHashAlg);
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if (pVendorSpec->bBEIDCardType != BEID_ECC_CARD) //no predefined padding in EC cards
	{
		//First check if padding info is provided
		if ((pInfo->dwSigningFlags & CARD_PADDING_INFO_PRESENT) == CARD_PADDING_INFO_PRESENT)
		{
			LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwSigningFlags: CARD_PADDING_INFO_PRESENT");
			if (pInfo->pPaddingInfo == NULL)
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->pPaddingInfo]");
				CLEANUP(SCARD_E_INVALID_PARAMETER);
			}

			switch (pInfo->dwPaddingType)
			{
			case CARD_PADDING_PKCS1:
				LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_PKCS1");

				PkcsPadInfo = (BCRYPT_PKCS1_PADDING_INFO *)pInfo->pPaddingInfo;

				if (PkcsPadInfo->pszAlgId == NULL)
				{
					LogTrace(LOGTYPE_INFO, WHERE, "PkcsPadInfo->pszAlgId = NULL: CMD PKCS#1 Sign...");

					uiHashAlgo = HASH_ALGO_NONE;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"MD2") == 0)
				{
					uiHashAlgo = HASH_ALGO_MD2;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"MD4") == 0)
				{
					uiHashAlgo = HASH_ALGO_MD4;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"MD5") == 0)
				{
					uiHashAlgo = HASH_ALGO_MD5;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA1") == 0)
				{
					uiHashAlgo = HASH_ALGO_SHA1;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA256") == 0)
				{
					uiHashAlgo = HASH_ALGO_SHA_256;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA384") == 0)
				{
					uiHashAlgo = HASH_ALGO_SHA_384;
				}
				else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA512") == 0)
				{
					uiHashAlgo = HASH_ALGO_SHA_512;
				}
				else
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "[PkcsPadInfo->pszAlgId] unsupported...");
					CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
				}
				break;
			case CARD_PADDING_PSS:
				//check card belpic applet version : if its < 1.7 -> not supported
				//first need to create workaround for incompatible card and minidriver architecture
				//(beidcard does not allow setting algo between pinpvalidation and signature,
				//and base csp doesn't show the padding algo before pin validation )
				if (bKeyNr == BELPIC_KEY_AUTH)
				{
					PssPadInfo = (BCRYPT_PSS_PADDING_INFO*)pInfo->pPaddingInfo;
					if (wcscmp(PssPadInfo->pszAlgId, L"SHA1") == 0)
					{
						uiHashAlgo = HASH_ALGO_NONE;
						bAlgoRef = BELPIC_SIGN_ALGO_RSASSA_PSS_SHA1;
					}
					else if (wcscmp(PssPadInfo->pszAlgId, L"SHA256") == 0)
					{
						uiHashAlgo = HASH_ALGO_NONE;
						bAlgoRef = BELPIC_SIGN_ALGO_RSASSA_PSS_SHA256;
					}
				}
				else
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "CARD_PADDING_PSS unsupported...");
					CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
				}
				LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_PSS");
				//memcpy (&PssPadInfo, pInfo->pPaddingInfo, sizeof(PssPadInfo));
				break;
			case CARD_PADDING_NONE:
				LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_NONE");
				uiHashAlgo = HASH_ALGO_NONE;
				break;
			default:
				LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: UNSUPPORTED");
				break;
			}
		}
		else
		{ //no padding info is provided, defaulting to PKCS1
			LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->pPaddingInfo] unsupported...");

			switch (pInfo->aiHashAlg)
			{
			case CALG_MD2:
				uiHashAlgo = HASH_ALGO_MD2;
				break;
			case CALG_MD4:
				uiHashAlgo = HASH_ALGO_MD4;
				break;
			case CALG_MD5:
				uiHashAlgo = HASH_ALGO_MD5;
				break;
			case CALG_SHA1: //CALG_SHA: same value
				uiHashAlgo = HASH_ALGO_SHA1;
				break;
			case CALG_SHA_256:
				uiHashAlgo = HASH_ALGO_SHA_256;
				break;
			case CALG_SHA_384:
				uiHashAlgo = HASH_ALGO_SHA_384;
				break;
			case CALG_SHA_512:
				uiHashAlgo = HASH_ALGO_SHA_512;
				break;
			case CALG_SSL3_SHAMD5:
				uiHashAlgo = HASH_ALGO_NONE;
				break;
			case CALG_TLS1PRF:
			case CALG_MAC:
			case CALG_HASH_REPLACE_OWF:
			case CALG_HUGHES_MD5:
			case CALG_HMAC:
				CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
				break;
			default:
				LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->aiHashAlg] is zero");
				CLEANUP(SCARD_E_INVALID_PARAMETER);
				/*uiHashAlgo = HASH_ALGO_NONE;*/
				break;
			}
		}
	}
	else //no predefined padding in EC cards, do not add a PKCS#1 header
	{
	//First check if padding info is provided
		if ((pInfo->dwSigningFlags & CARD_PADDING_INFO_PRESENT) == CARD_PADDING_INFO_PRESENT)
		{
			LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwSigningFlags: CARD_PADDING_INFO_PRESENT");
			CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
		}
		else
		{
			LogTrace(LOGTYPE_INFO, WHERE, "No CARD_PADDING_INFO_PRESENT");
		}

	//the hash lengths (i.e. DTBS) are limited to 160, 224, 256, 384 and 512 bits
		uiHashAlgo = HASH_ALGO_NONE;
	}

#ifdef _DEBUG
	LogTrace(LOGTYPE_INFO, WHERE, "BeidMSE [key=0x%.2x, Hashalgo=0x%.2x]", bKeyNr, uiHashAlgo);
#endif

	if (bKeyNr == BELPIC_KEY_AUTH && pVendorSpec->bBEIDCardType == BEID_RSA_CARD) //only for AUTH key in RSA cards
	{
		dwReturn = BeidMSE(pCardData,bKeyNr,bAlgoRef);
		if ( dwReturn != 0 )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "BeidMSE() returned [0x%X]", dwReturn);
			CLEANUP(dwReturn);
		}
	}

#ifdef _DEBUG
	LogTrace(LOGTYPE_INFO, WHERE, "Data to be Signed...[%d]", pInfo->cbData);
	LogDump (pInfo->cbData, (char *)pInfo->pbData);
#endif

	dwReturn = BeidSignData(pCardData, 
		uiHashAlgo,
		pInfo->cbData, 
		pInfo->pbData, 
		&(pInfo->cbSignedData), 
		&(pInfo->pbSignedData));
	if ( dwReturn != 0 )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "BeidSignData() returned [0x%X]", dwReturn);
		CLEANUP(dwReturn);
	}

#ifdef _DEBUG
	LogTrace(LOGTYPE_INFO, WHERE, "Signature Data...[%d]", pInfo->cbSignedData);
	LogDump (pInfo->cbSignedData, (char *)pInfo->pbSignedData);
#endif

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardQueryKeySizes
//

#define WHERE "CardQueryKeySizes()"
DWORD WINAPI   CardQueryKeySizes
	(
	__in      PCARD_DATA       pCardData,
	__in      DWORD            dwKeySpec,
	__in      DWORD            dwFlags,
	__in      PCARD_KEY_SIZES  pKeySizes
	)
{
	DWORD    dwReturn       = 0;
	DWORD    dwVersion      = 0;
	int      iUnSupported   = 0;
	int      iInValid       = 0;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( dwFlags != 0 )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwKeySpec]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( pKeySizes == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pKeySizes]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	dwVersion = (pKeySizes->dwVersion == 0) ? 1 : pKeySizes->dwVersion;
	if ( dwVersion != CARD_KEY_SIZES_CURRENT_VERSION )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pKeySizes->dwVersion]");
		CLEANUP(ERROR_REVISION_MISMATCH );
	}

	switch(dwKeySpec)
	{
	case AT_ECDHE_P521 :
	case AT_ECDSA_P521 :
		iUnSupported++;
		break;
	//supported EC keys
	case AT_ECDHE_P256:
	case AT_ECDSA_P256:
		pKeySizes->dwMinimumBitlen = 256;
		pKeySizes->dwDefaultBitlen = 256;
		pKeySizes->dwMaximumBitlen = 256;
		pKeySizes->dwIncrementalBitlen = 1;
		break;
	case AT_ECDHE_P384:
	case AT_ECDSA_P384:
		pKeySizes->dwMinimumBitlen = 384;
		pKeySizes->dwDefaultBitlen = 384;
		pKeySizes->dwMaximumBitlen = 384;
		pKeySizes->dwIncrementalBitlen = 1;
		break;
	//for RSA keys
	case AT_KEYEXCHANGE:
	case AT_SIGNATURE  :
		pKeySizes->dwMinimumBitlen = 2048;
		pKeySizes->dwDefaultBitlen = 2048;
		pKeySizes->dwMaximumBitlen = 2048;
		pKeySizes->dwIncrementalBitlen = 0;
		break;
	default:
		iInValid++;
		break;
	}
	if ( iInValid )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwKeySpec][%d]", dwKeySpec);
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( iUnSupported )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Unsupported parameter [dwKeySpec][%d]", dwKeySpec);
		CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
	}

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
