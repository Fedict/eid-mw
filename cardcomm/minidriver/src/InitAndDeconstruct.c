/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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

/****************************************************************************************************/

#define MINIMUM_VERSION_SUPPORTED      CARD_DATA_VERSION_SIX
#define CURRENT_VERSION_SUPPORTED      CARD_DATA_VERSION_SIX

#define SUPPORTED_CARDS                3

/* Supported ATRs by the Mini Driver */
CARD_ATR    CardAtr[] = 
            { 
               {{0x3B,0x98,0x13,0x40,0x0A,0xA5,0x03,0x01,0x01,0x01,0xAD,0x13,0x11}, 13},
               {{0x3B,0x98,0x94,0x40,0x0A,0xA5,0x03,0x01,0x01,0x01,0xAD,0x13,0x10}, 13},
               {{0x3B,0x98,0x94,0x40,0xFF,0xA5,0x03,0x01,0x01,0x01,0xAD,0x13,0x10}, 13}
            };

/****************************************************************************************************/


//
// Function: CardAcquireContext
//
// Purpose: Initialize the CARD_DATA structure which will be used by
//          the CSP to interact with a specific card.
//

#define WHERE "CardAcquireContext()"
DWORD WINAPI   CardAcquireContext
               (
                  IN    PCARD_DATA  pCardData,
                  __in	DWORD       dwFlags
               )
{
   DWORD                   dwReturn    = 0;
   
   int                     iAtr        = 0;
   int                     iCardCnt    = 0;
   int                     iLgCnt      = 0;
   int                     i           = 0;

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
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /* The lowest supported version */
   if ( pCardData->dwVersion < MINIMUM_VERSION_SUPPORTED )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->dwVersion][%d]",pCardData->dwVersion);
      CLEANUP(ERROR_REVISION_MISMATCH);
   }
   /* Set the version to what we support, but don't exceed the requested version */
   pCardData->dwVersion = min(pCardData->dwVersion, CURRENT_VERSION_SUPPORTED);

   /* Check ATR */
   if ( pCardData->pbAtr == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pbAtr]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   for ( iAtr = 0 ; iAtr < SUPPORTED_CARDS ; iAtr++ )
   {
      if ( pCardData->cbAtr == CardAtr[iAtr].cbAtr )
      {
         if ( memcmp(pCardData->pbAtr, CardAtr[iAtr].pbAtr, pCardData->cbAtr) == 0 )
         {
            iCardCnt++;
            break;
         }

         iLgCnt++;
      }
   }
   if ( iCardCnt == 0 )
   {
      if ( iLgCnt == 0 )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->cbAtr]");
         CLEANUP(SCARD_E_INVALID_PARAMETER);
      }
      else
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Unknown card");
         CLEANUP(SCARD_E_UNKNOWN_CARD);
      }
   }

   /* Card name */
   if ( pCardData->pwszCardName == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pwszCardName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   /* Memory management functions */
   if ( ( pCardData->pfnCspAlloc   == NULL ) ||
        ( pCardData->pfnCspReAlloc == NULL ) ||
        ( pCardData->pfnCspFree    == NULL ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pfnCspAlloc], [pCardData->pfnCspReAlloc] or [pCardData->pfnCspFree]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   /* Card context */
   if ( pCardData->hSCardCtx == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "[pCardData->hSCardCtx = 0] not Fatal...");
      //CLEANUP(SCARD_E_INVALID_HANDLE);
   }

   /* Card handle */
   if ( pCardData->hScard == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hScard]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }

   LogTrace(LOGTYPE_INFO, WHERE, "Context, handle:[0x%02X][0x%02X]", pCardData->hSCardCtx, pCardData->hScard);

   /********************************/
   /* Initialize function pointers */
   /********************************/

   /* InitAndDeconstruct.c */
   pCardData->pfnCardDeleteContext           = CardDeleteContext;

   /* CardPinOps.c */
   pCardData->pfnCardAuthenticatePin         = CardAuthenticatePin;
   pCardData->pfnCardGetChallenge            = CardGetChallenge;
   pCardData->pfnCardAuthenticateChallenge   = CardAuthenticateChallenge;
   pCardData->pfnCardDeauthenticate          = CardDeauthenticate;
   pCardData->pfnCardAuthenticateEx          = CardAuthenticateEx;
   pCardData->pfnCardGetChallengeEx          = CardGetChallengeEx;
   pCardData->pfnCardDeauthenticateEx        = CardDeauthenticateEx; /* This can be an optional export */
   pCardData->pfnCardChangeAuthenticatorEx   = CardChangeAuthenticatorEx;
   pCardData->pfnCardUnblockPin              = CardUnblockPin;
   pCardData->pfnCardChangeAuthenticator     = CardChangeAuthenticator;

   /* PubDataOps.c */
   pCardData->pfnCardCreateDirectory         = CardCreateDirectory;
   pCardData->pfnCardDeleteDirectory         = CardDeleteDirectory;
   pCardData->pfnCardReadFile                = CardReadFile;
   pCardData->pfnCardCreateFile              = CardCreateFile;
   pCardData->pfnCardGetFileInfo             = CardGetFileInfo;
   pCardData->pfnCardWriteFile               = CardWriteFile;
   pCardData->pfnCardDeleteFile              = CardDeleteFile;
   pCardData->pfnCardEnumFiles               = CardEnumFiles;
   pCardData->pfnCardQueryFreeSpace          = CardQueryFreeSpace;

   /* CardCap.c */
   pCardData->pfnCardQueryCapabilities       = CardQueryCapabilities;

   /* CardAndContProp.c */
   pCardData->pfnCardGetContainerProperty    = CardGetContainerProperty;
   pCardData->pfnCardSetContainerProperty    = CardSetContainerProperty;
   pCardData->pfnCardGetProperty             = CardGetProperty;
   pCardData->pfnCardSetProperty             = CardSetProperty;

   /* KeyContainer.c */
   pCardData->pfnCardCreateContainer         = CardCreateContainer;
   pCardData->pfnCardDeleteContainer         = CardDeleteContainer;
   pCardData->pfnCardGetContainerInfo        = CardGetContainerInfo;

   /* CryptoOps.c */
   pCardData->pfnCardRSADecrypt              = CardRSADecrypt;

#ifdef _ECC_
   pCardData->pfnCardConstructDHAgreement    = CardConstructDHAgreement;
   pCardData->pfnCardDeriveKey               = CardDeriveKey;
   pCardData->pfnCardDestroyDHAgreement      = CardDestroyDHAgreement;
#else
   pCardData->pfnCardConstructDHAgreement    = NULL;
   pCardData->pfnCardDeriveKey               = NULL;
   pCardData->pfnCardDestroyDHAgreement      = NULL;
#endif

   pCardData->pfnCardSignData                = CardSignData;
   pCardData->pfnCardQueryKeySizes           = CardQueryKeySizes;

   /* Not defined */
   pCardData->pfnCspGetDHAgreement           = NULL;

	/* Vendor specific */
	pCardData->pvVendorSpecific               = pCardData->pfnCspAlloc(sizeof(VENDOR_SPECIFIC));
	memset(pCardData->pvVendorSpecific, 0, sizeof(VENDOR_SPECIFIC));
cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeleteContext
//
// Purpose: Free resources consumed by the CARD_DATA structure.
//

#define WHERE "CardDeleteContext()"
DWORD WINAPI   CardDeleteContext
               (
                  __inout PCARD_DATA  pCardData
               )
{
   DWORD             dwReturn    = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   LogTrace(LOGTYPE_INFO, WHERE, "Context:[0x%08X]", pCardData->hSCardCtx);
   if (pCardData->pvVendorSpecific != NULL)
   {
	   pCardData->pfnCspFree(pCardData->pvVendorSpecific);
	   pCardData->pvVendorSpecific = NULL;
   }
	
cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/
