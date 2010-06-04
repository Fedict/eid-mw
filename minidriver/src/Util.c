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

extern head_type        gContextCardList;

/****************************************************************************************************/

#define WHERE "DeleteContextFromList"
DWORD DeleteContextFromList(PCARD_DATA pCardData) 
{
   DWORD                dwReturn    = 0;
   int                  Found       = 0;
   int                  i           = 0;
   int                  j           = 0;

   PCONTEXT_LIST_TYPE   pCtxItem    = NULL;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;

   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   /* Card context */
   if ( pCardData->hSCardCtx == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hSCardCtx = 0]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }
   /* Card handle */
   if ( pCardData->hScard == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hScard]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }

   /* Look for Context handle */
   Found = 0;
   for ( i = 1 ; i <= ITEM_CNT(&gContextCardList) ; i++)
   {
      Goto_item_in_list(&gContextCardList, i);
      pCtxItem = (PCONTEXT_LIST_TYPE) CURR_PTR(&gContextCardList);
      if ( pCtxItem->hSCardCtx == pCardData->hSCardCtx )
      {
         Found++;
         break;
      }
   }

   if ( ! Found )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Context Handle not found in List...");
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   LogTrace(LOGTYPE_INFO, WHERE, "Found Context [0x%08X], [%d] Card Handles...", pCardData->hSCardCtx, ITEM_CNT(&pCtxItem->CardList));

   /* Look for Card handle */
   for ( i = 1 ; i <= ITEM_CNT(&pCtxItem->CardList) ; i++)
   {
      Goto_item_in_list(&pCtxItem->CardList, i);
      pCardItem = (PCARD_LIST_TYPE) CURR_PTR(&pCtxItem->CardList);

      LogTrace(LOGTYPE_INFO, WHERE, "Card Handle: [0x%08X], Deleting [%d] objects...", pCardItem->hScard, ITEM_CNT(&pCardItem->ObjectList));
      for ( j = 1 ; j <= ITEM_CNT(&pCardItem->ObjectList) ; j++ )
      {
         Goto_item_in_list(&pCardItem->ObjectList, j);
         pObjectItem = (POBJECT_LIST_TYPE) CURR_PTR(&pCardItem->ObjectList);

         /* Free memory allocated in Objects */
         if ( pObjectItem->pObjectData != NULL )
         {
            LogTrace(LOGTYPE_INFO, WHERE, "Free [%d] bytes at [%x]...", pObjectItem->ObjectDataSize, pObjectItem->pObjectData);

            pCardData->pfnCspFree(pObjectItem->pObjectData);
            pObjectItem->pObjectData    = NULL;
            pObjectItem->ObjectDataSize = 0;
         }
      }
      LogTrace(LOGTYPE_INFO, WHERE, "Destroy Object List...");
      Destroy_list(&pCardItem->ObjectList);

      for ( j = 0 ; j < MAX_CONTAINERS ; j++ )
      {
         if ( pCardItem->ContainerInfo[j].ContainerInfo.pbSigPublicKey != NULL )
         {
            pCardData->pfnCspFree(pCardItem->ContainerInfo[j].ContainerInfo.pbSigPublicKey);
            pCardItem->ContainerInfo[j].ContainerInfo.pbSigPublicKey = NULL;
            pCardItem->ContainerInfo[j].ContainerInfo.cbSigPublicKey = 0;
         }
         if ( pCardItem->ContainerInfo[j].ContainerInfo.pbKeyExPublicKey != NULL )
         {
            pCardData->pfnCspFree(pCardItem->ContainerInfo[j].ContainerInfo.pbKeyExPublicKey);
            pCardItem->ContainerInfo[j].ContainerInfo.pbKeyExPublicKey = NULL;
            pCardItem->ContainerInfo[j].ContainerInfo.cbKeyExPublicKey = 0;
         }
      }
   }

   LogTrace(LOGTYPE_INFO, WHERE, "Destroy Card List...");
   Destroy_list(&pCtxItem->CardList);

   /* Remove context from list */
   LogTrace(LOGTYPE_INFO, WHERE, "Remove context from list...");
   Remove_item_from_list(&gContextCardList);
cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "GetCardListItem"
PCARD_LIST_TYPE GetCardListItem(PCARD_DATA pCardData)
{
   DWORD                dwReturn  = 0;

   int                  i         = 0;
   int                  Found     = 0;

   PCONTEXT_LIST_TYPE   pCtxItem    = NULL;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;

   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      return (NULL);
   }
   /* Card context */
   if ( pCardData->hSCardCtx == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hSCardCtx = 0]");
      return (NULL);
   }
   /* Card handle */
   if ( pCardData->hScard == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hScard]");
      return (NULL);
   }

   /* Look for Context handle */
   Found = 0;
   for ( i = 1 ; i <= ITEM_CNT(&gContextCardList) ; i++)
   {
      Goto_item_in_list(&gContextCardList, i);
      pCtxItem = (PCONTEXT_LIST_TYPE) CURR_PTR(&gContextCardList);
      if ( pCtxItem->hSCardCtx == pCardData->hSCardCtx )
      {
         Found++;
         break;
      }
   }

   if ( ! Found )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Context Handle not found in List...");
      return (NULL);
   }

   /* Look for Card handle */
   Found = 0;
   for ( i = 1 ; i <= ITEM_CNT(&pCtxItem->CardList) ; i++)
   {
      Goto_item_in_list(&pCtxItem->CardList, i);
      pCardItem = (PCARD_LIST_TYPE) CURR_PTR(&pCtxItem->CardList);
      if ( pCardItem->hScard == pCardData->hScard )
      {
         Found++;
         break;
      }
   }

   if ( ! Found )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Card Handle not found in List...");
      return (NULL);
   }

   return(pCardItem);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "AddContextInList"
DWORD AddContextInList(PCARD_DATA pCardData)
{
   DWORD                dwReturn  = 0;

   int                  i         = 0;
   int                  Found     = 0;

   PCONTEXT_LIST_TYPE   pCtxItem    = NULL;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;

   PIN_CACHE_POLICY     pinCachePolicy;

   CONTAINER_MAP_RECORD cmr[2];
   GUID                 guid  ;
   unsigned char        a_ucSerNum  [64];
   unsigned char        a_ucContName[64];
   int                  iOutLg = 0;
   FEATURES				CCIDfeatures;

   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   /* Card context */
   if ( pCardData->hSCardCtx == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hSCardCtx = 0]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }
   /* Card handle */
   if ( pCardData->hScard == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hScard]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }

   /* Look for Context handle */
   Found = 0;
   for ( i = 1 ; i <= ITEM_CNT(&gContextCardList) ; i++)
   {
      Goto_item_in_list(&gContextCardList, i);
      pCtxItem = (PCONTEXT_LIST_TYPE) CURR_PTR(&gContextCardList);
      if ( pCtxItem->hSCardCtx == pCardData->hSCardCtx )
      {
         Found++;
         break;
      }
   }
   if ( Found == 0 )
   {
      /* Add new context item to the list */
      pCtxItem = (PCONTEXT_LIST_TYPE) Allocate_memory_for_list(sizeof(CONTEXT_LIST_TYPE));
      if ( pCtxItem == NULL )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CONTEXT_LIST_TYPE");
         CLEANUP(SCARD_E_UNEXPECTED);
      }

      pCtxItem->hSCardCtx = pCardData->hSCardCtx;
      Init_list(&pCtxItem->CardList);
      Append_item_in_list (&gContextCardList, (head_type *)pCtxItem);
   }

   /* Add new card to the new context */
   Found = 0;
   for ( i = 1 ; i <= ITEM_CNT(&pCtxItem->CardList) ; i++)
   {
      Goto_item_in_list(&pCtxItem->CardList, i);
      pCardItem = (PCARD_LIST_TYPE) CURR_PTR(&pCtxItem->CardList);
      if ( pCardItem->hScard == pCardData->hScard )
      {
         Found++;
         break;
      }
   }
   if ( Found != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "AddContextInList: Entry exists...");
      //CLEANUP(SCARD_E_UNEXPECTED);
      CLEANUP(0);
   }
   else
   {
      /* Add new card item to the list */
      pCardItem = (PCARD_LIST_TYPE) Allocate_memory_for_list(sizeof(CARD_LIST_TYPE));
      if ( pCardItem == NULL )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
         CLEANUP(SCARD_E_UNEXPECTED);
      }

      pCardItem->hScard = pCardData->hScard;
      Init_list(&pCardItem->ObjectList);
      Append_item_in_list (&pCtxItem->CardList, (head_type *)pCardItem);
   }

   /* Create the Card Objects */
   for ( i = 0 ; i < MAX_CONTAINERS ; i++ )
   {
      pCardItem->ContainerInfo[i].ContainerInfo.pbSigPublicKey   = NULL;
      pCardItem->ContainerInfo[i].ContainerInfo.cbSigPublicKey   = 0;
      pCardItem->ContainerInfo[i].ContainerInfo.pbKeyExPublicKey = NULL;
      pCardItem->ContainerInfo[i].ContainerInfo.cbKeyExPublicKey = 0;
   }

   /*
    * Card Identifier
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Card Identifier...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   pObjectItem->szDirectoryName[0] = '\0';
   strcpy (pObjectItem->szFileName, szCARD_IDENTIFIER_FILE); /* cardid */
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadAdminWriteAc;
   /* The size of the GUID-structure is 16 bytes */
   pObjectItem->ObjectDataSize     = sizeof(GUID);
   pObjectItem->pObjectData        = pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize);
   if ( pObjectItem->pObjectData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [pObjectItem->pObjectData][GUID]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   CoCreateGuid(&guid);
   memcpy (pObjectItem->pObjectData, &guid, pObjectItem->ObjectDataSize);
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

   /*
    * Application Directory
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Application Directory...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   pObjectItem->szDirectoryName[0] = '\0';
   strcpy (pObjectItem->szFileName, "cardapps");
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadAdminWriteAc;
   pObjectItem->ObjectDataSize     = 8;
   pObjectItem->pObjectData        = pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize); /* Name can be 8 bytes: no requirement of zero-termination */
   if ( pObjectItem->pObjectData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [pObjectItem->pObjectData][cardapps]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
   strncpy (pObjectItem->pObjectData, szBASE_CSP_DIR, pObjectItem->ObjectDataSize);
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

   /*
    * Cache File           
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Cache File...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   pObjectItem->szDirectoryName[0] = '\0';
   strcpy (pObjectItem->szFileName, szCACHE_FILE); /* cardcf */
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadUserWriteAc;
   pObjectItem->ObjectDataSize     = 6;
   pObjectItem->pObjectData        = pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize);
   if ( pObjectItem->pObjectData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [pObjectItem->pObjectData][cardcf]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   memset(pObjectItem->pObjectData, '\0', pObjectItem->ObjectDataSize);
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

   /*
    * Container Map File
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Map file...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   strcpy (pObjectItem->szDirectoryName, szBASE_CSP_DIR);       /* mscp     */
   strcpy (pObjectItem->szFileName     , szCONTAINER_MAP_FILE); /* cmapfile */
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadAdminWriteAc;
   pObjectItem->ObjectDataSize     = sizeof(cmr);
   pObjectItem->pObjectData        = pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize);
   if ( pObjectItem->pObjectData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [pObjectItem->pObjectData][mscp]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   /***************************/
   /* Authentication Key Info */
   /***************************/

   /* Use Card Serial Number as part of the container names */
   dwReturn = BeidGetCardSN(pCardData, sizeof(a_ucSerNum), a_ucSerNum) ;
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidGetCardSN returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   /* Cleanup CMR first */
   memset(&cmr, '\0', sizeof(cmr));

   /* Container name for Authentication key */
   sprintf (a_ucContName, "DS_%s", a_ucSerNum);
   memset(cmr[0].wszGuid, '\0', sizeof(cmr[0].wszGuid));
   iOutLg = MultiByteToWideChar(CP_UTF8, 0, a_ucContName, strlen(a_ucContName), cmr[0].wszGuid, sizeof(cmr[0].wszGuid));

   cmr[0].bFlags                     = CONTAINER_MAP_VALID_CONTAINER|CONTAINER_MAP_DEFAULT_CONTAINER;
   cmr[0].bReserved                  = 0;
   cmr[0].wSigKeySizeBits            = 1024;
   cmr[0].wKeyExchangeKeySizeBits    = 0;

   /****************************/
   /* Non-Repudiation Key Info */
   /****************************/
   /* Container name for Non-repudiation key */
   sprintf (a_ucContName, "NR_%s", a_ucSerNum);
   memset(cmr[1].wszGuid, '\0', sizeof(cmr[1].wszGuid));
   iOutLg = MultiByteToWideChar(CP_UTF8, 0, a_ucContName, strlen(a_ucContName), cmr[1].wszGuid, sizeof(cmr[1].wszGuid));

   cmr[1].bFlags                     = CONTAINER_MAP_VALID_CONTAINER;
   cmr[1].bReserved                  = 0;
   cmr[1].wSigKeySizeBits            = 1024;
   cmr[1].wKeyExchangeKeySizeBits    = 0;
   memcpy(pObjectItem->pObjectData, &cmr, pObjectItem->ObjectDataSize);
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

   /*
    * Authentication Certificate
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Authentication Certif...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   strcpy (pObjectItem->szDirectoryName, szBASE_CSP_DIR);
   strcpy (pObjectItem->szFileName, "ksc00");
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadUserWriteAc;
   dwReturn = BeidReadCert(pCardData, CERT_AUTH, &pObjectItem->ObjectDataSize, (PBYTE *)&pObjectItem->pObjectData);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadCert[CERT_AUTH] returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

#ifdef _DEBUG
   LogDump (pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
   LogDumpBin ("C:\\SmartCardMinidriverTest\\auth.crt", pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
#endif

   /* 
    * Container Info
    */
   /* Pin ID */
   pCardItem->ContainerInfo[0].dwPinId = ROLE_DIGSIG;
   
   /* Container Info */
   pCardItem->ContainerInfo[0].ContainerInfo.dwVersion  = CONTAINER_INFO_CURRENT_VERSION;
   pCardItem->ContainerInfo[0].ContainerInfo.dwReserved = 0;
   dwReturn = BeidGetPubKey(pCardData,
                            pObjectItem->ObjectDataSize,
                            pObjectItem->pObjectData,
                            &(pCardItem->ContainerInfo[0].ContainerInfo.cbSigPublicKey),
                            &(pCardItem->ContainerInfo[0].ContainerInfo.pbSigPublicKey));
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidGetPubKey returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   pCardItem->ContainerInfo[0].ContainerInfo.cbKeyExPublicKey = 0;
   pCardItem->ContainerInfo[0].ContainerInfo.pbKeyExPublicKey = NULL;
#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\authpk.bin", pCardItem->ContainerInfo[0].ContainerInfo.cbSigPublicKey
                                                       , (char *)pCardItem->ContainerInfo[0].ContainerInfo.pbSigPublicKey);
#endif

   /*
    * Non-repudiation Certificate
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating Non-Repudiation Certif...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   strcpy (pObjectItem->szDirectoryName, szBASE_CSP_DIR);
   strcpy (pObjectItem->szFileName, "ksc01");
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadUserWriteAc;
   dwReturn = BeidReadCert(pCardData, CERT_NONREP, &pObjectItem->ObjectDataSize, (PBYTE *)&pObjectItem->pObjectData);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadCert[CERT_NONREP] returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

#ifdef _DEBUG
   LogDump (pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
   LogDumpBin ("C:\\SmartCardMinidriverTest\\nonrep.crt", pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
#endif

   /* 
    * Container Info
    */
   /* Pin Id */
   pCardItem->ContainerInfo[1].dwPinId = ROLE_NONREP;

   /* Container Info */
   pCardItem->ContainerInfo[1].ContainerInfo.dwVersion        = CONTAINER_INFO_CURRENT_VERSION;
   pCardItem->ContainerInfo[1].ContainerInfo.dwReserved       = 0;
   dwReturn = BeidGetPubKey(pCardData,
                            pObjectItem->ObjectDataSize,
                            pObjectItem->pObjectData,
                            &(pCardItem->ContainerInfo[1].ContainerInfo.cbSigPublicKey),
                            &(pCardItem->ContainerInfo[1].ContainerInfo.pbSigPublicKey));
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidGetPubKey returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   pCardItem->ContainerInfo[1].ContainerInfo.cbKeyExPublicKey = 0;
   pCardItem->ContainerInfo[1].ContainerInfo.pbKeyExPublicKey = NULL;
#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\nonreppk.bin", pCardItem->ContainerInfo[1].ContainerInfo.cbSigPublicKey
                                                         , (char *)pCardItem->ContainerInfo[1].ContainerInfo.pbSigPublicKey);
#endif

   /*
    * MSROOTS
    */
   LogTrace(LOGTYPE_INFO, WHERE, "Creating MSROOTS...");
   pObjectItem = (POBJECT_LIST_TYPE) Allocate_memory_for_list(sizeof(OBJECT_LIST_TYPE));
   if ( pObjectItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Allocate_memory_for_list() Failed : CARD_LIST_TYPE");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   strcpy (pObjectItem->szDirectoryName, szBASE_CSP_DIR);
   strcpy (pObjectItem->szFileName, "msroots");
   pObjectItem->bObjectType        = FILE_OBJECT_FILE_TYPE;
   pObjectItem->bAccessCondition   = EveryoneReadUserWriteAc;

   pObjectItem->ObjectDataSize = 0;
   pObjectItem->pObjectData    = NULL;


   dwReturn = BeidCreateMSRoots(pCardData, &(pObjectItem->ObjectDataSize), (PBYTE *)&(pObjectItem->pObjectData));
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidCreateMSRoots returned [%d]", dwReturn);
      CLEANUP(SCARD_E_UNEXPECTED);
   }


   Append_item_in_list (&pCardItem->ObjectList, (head_type *)pObjectItem);

#ifdef _DEBUG
   LogDump (pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
   LogDumpBin ("C:\\SmartCardMinidriverTest\\msroots.p7c", pObjectItem->ObjectDataSize, (char *)pObjectItem->pObjectData);
#endif

   /**************************************************************************************************************************/
   /*                                          PIN Information                                                               */
   /**************************************************************************************************************************/

   pinCachePolicy.dwVersion            = 0;
   pinCachePolicy.PinCachePolicyType   = 0;
   pinCachePolicy.dwPinCachePolicyInfo = 0;
   for ( i = 0 ; i < MAX_PINS ; i++ )
   {
      pCardItem->PinInfo[i].dwVersion           = 0;
      pCardItem->PinInfo[i].PinType             = 0;
      pCardItem->PinInfo[i].PinPurpose          = 0;
      pCardItem->PinInfo[i].dwChangePermission  = 0;
      pCardItem->PinInfo[i].dwUnblockPermission = 0;
      memcpy (&(pCardItem->PinInfo[i].PinCachePolicy), &pinCachePolicy, sizeof(pinCachePolicy));
      pCardItem->PinInfo[i].dwFlags             = 0;
   }

   /* Pin Cache Policy */
   pinCachePolicy.dwVersion            = PIN_CACHE_POLICY_CURRENT_VERSION;
   pinCachePolicy.PinCachePolicyType   = PinCacheNormal;
   pinCachePolicy.dwPinCachePolicyInfo = 0;
   /* Pin Info */
   pCardItem->PinInfo[ROLE_DIGSIG].dwVersion           = PIN_INFO_CURRENT_VERSION; 
   CCIDgetFeatures(&(CCIDfeatures), pCardData->hScard);

   if (CCIDfeatures.VERIFY_PIN_START != 0)
       pCardItem->PinInfo[ROLE_DIGSIG].PinType             = ExternalPinType;
   else
       pCardItem->PinInfo[ROLE_DIGSIG].PinType             = AlphaNumericPinType;
   
   pCardItem->PinInfo[ROLE_DIGSIG].PinPurpose          = AuthenticationPin;
   pCardItem->PinInfo[ROLE_DIGSIG].dwChangePermission  = CREATE_PIN_SET(ROLE_DIGSIG);
   pCardItem->PinInfo[ROLE_DIGSIG].dwUnblockPermission = 0;
   memcpy (&(pCardItem->PinInfo[ROLE_DIGSIG].PinCachePolicy), &pinCachePolicy, sizeof(pinCachePolicy));
   pCardItem->PinInfo[ROLE_DIGSIG].dwFlags             = 0;

   /* Pin Cach Policy */
   pinCachePolicy.dwVersion            = PIN_CACHE_POLICY_CURRENT_VERSION;
   pinCachePolicy.PinCachePolicyType   = PinCacheNone;
   pinCachePolicy.dwPinCachePolicyInfo = 0;
   pCardItem->PinInfo[ROLE_NONREP].dwVersion           = PIN_INFO_CURRENT_VERSION;
   if (CCIDfeatures.VERIFY_PIN_START != 0)
       pCardItem->PinInfo[ROLE_NONREP].PinType             = ExternalPinType;
   else
	   pCardItem->PinInfo[ROLE_NONREP].PinType             = AlphaNumericPinType;
   pCardItem->PinInfo[ROLE_NONREP].PinPurpose          = NonRepudiationPin;
   pCardItem->PinInfo[ROLE_NONREP].dwChangePermission  = CREATE_PIN_SET(ROLE_NONREP);
   pCardItem->PinInfo[ROLE_NONREP].dwUnblockPermission = 0;
   memcpy (&(pCardItem->PinInfo[ROLE_NONREP].PinCachePolicy), &pinCachePolicy, sizeof(pinCachePolicy));
   pCardItem->PinInfo[ROLE_NONREP].dwFlags             = 0;


   LogTrace(LOGTYPE_INFO, WHERE, "ADDED new card for the context: [0x%08X][0x%08X]", pCardData->hSCardCtx, pCardData->hScard);

cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return (dwReturn);
}
#undef WHERE

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
   *pcbMSRoots = blob.cbData;

cleanup:
   if ( pCertContext != NULL )
   {
      CertFreeCertificateContext(pCertContext);
      pCertContext = NULL;
   }
   if ( pbCertif != NULL )
   {
      pCardData->pfnCspFree(pbCertif);
   }
   if ( hMemoryStore != NULL )
   {
      CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
   }

   return (dwReturn);
}

