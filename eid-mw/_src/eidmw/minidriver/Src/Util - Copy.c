/****************************************************************************************************/

#include "GlobMdrv.h"

#include "Log.h"
#include "util.h"

/****************************************************************************************************/

extern head_type        gContextCardList;

/****************************************************************************************************/

#define WHERE "DeleteContextFromList"
DWORD DeleteContextFromList(PCARD_DATA pCardData)
{
   DWORD                dwReturn    = 0;
   int                  Found       = 0;
   int                  i           = 0;

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
         /* Free memory allocated in Objects */
   pObjectItem->ObjectDataSize     = sizeof(GUID);
   pObjectItem->pObjectData        = pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize);

      }
   }

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
   BYTE                 *str;
   int                  iOutLg = 0;

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
      CLEANUP(SCARD_E_UNEXPECTED);
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

   /***************************/
   /* Authentication Key Info */
   /***************************/

   /* Create GUID for Authentication key */
   memset(&cmr, '\0', sizeof(cmr));
   memset (&guid, '\0', sizeof(guid));
   CoCreateGuid(&guid);
   UuidToString((UUID*)&guid, &str);
   iOutLg = MultiByteToWideChar(CP_ACP, 0, str, strlen(str), cmr[0].wszGuid, sizeof(cmr[0].wszGuid));
   RpcStringFree(&str);

   wcscpy(cmr[0].wszGuid,L"BEID_AUTHENTICATION_KEY");
   cmr[0].bFlags                     = CONTAINER_MAP_VALID_CONTAINER|CONTAINER_MAP_DEFAULT_CONTAINER;
   cmr[0].bReserved                  = 0;
   cmr[0].wSigKeySizeBits            = 1024;
   cmr[0].wKeyExchangeKeySizeBits    = 0;

   /****************************/
   /* Non-Repudiation Key Info */
   /****************************/

   /* Create GUID for NON-Repudiation key */
   memset (&guid, '\0', sizeof(guid));
   CoCreateGuid(&guid);
   UuidToString((UUID*)&guid, &str);
   iOutLg = MultiByteToWideChar(CP_ACP, 0, str, strlen(str), cmr[1].wszGuid, sizeof(cmr[1].wszGuid));
   RpcStringFree(&str);

   wcscpy(cmr[0].wszGuid,L"BEID_NONREP_KEY");
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

   /* Pin Cach Policy */
   pinCachePolicy.dwVersion            = PIN_CACHE_POLICY_CURRENT_VERSION;
   pinCachePolicy.PinCachePolicyType   = PinCacheNormal;
   pinCachePolicy.dwPinCachePolicyInfo = 0;
   /* Pin Info */
   pCardItem->PinInfo[ROLE_DIGSIG].dwVersion           = PIN_INFO_CURRENT_VERSION;
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

#define WHERE "BeidSignData"
DWORD BeidSignData(PCARD_DATA  pCardData, unsigned int HashAlgo, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature)
{
   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[1024];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   static const unsigned char MD2_AID[] = {
      0x30, 0x20, 
         0x30, 0x0c, 
            0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x02, 
            0x05, 0x00, 
         0x04, 0x10
   };
   static const unsigned char MD4_AID[] = {
      0x30, 0x20, 
         0x30, 0x0c, 
            0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x04, 
            0x05, 0x00, 
         0x04, 0x10
   };
   static const unsigned char MD5_AID[] = {
	   0x30, 0x20,
		   0x30, 0x0c,
			   0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
			   0x05, 0x00,
		   0x04, 0x10
   };
   static const unsigned char SHA1_AID[] = {
	   0x30, 0x21,
		   0x30, 0x09,
			   0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
		   0x05, 0x00,
		   0x04, 0x14
   };
   static const unsigned char SHA256_AID[] = {
	   0x30, 0x31,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
		   0x05, 0x00,
		   0x04, 0x20
   };
   static const unsigned char SHA384_AID[] = {
	   0x30, 0x41,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02,
		   0x05,0x00,
		   0x04, 0x30
   };
   static const unsigned char SHA512_AID[] = {
	   0x30, 0x51,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03,
		   0x05, 0x00,
		   0x04, 0x40
   };
   static const unsigned char RIPEMD160_AID[] = {
	   0x30, 0x21,
		   0x30, 0x09,
			   0x06, 0x05, 0x2B, 0x24,	0x03, 0x02, 0x01,
		   0x05, 0x00,
		   0x04, 0x14
   };

   unsigned int            i          = 0;
   unsigned int            cbHdrHash  = 0;
   const unsigned char     *pbHdrHash = NULL;

#if 0
   /* Set Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0x22;   /* MSE: SET COMMAND */
   Cmd [2] = 0x41;
   Cmd [3] = 0xB6;
   Cmd [4] = 0x05;
   Cmd [5] = 0x04;   /* Length of following data      */
   Cmd [6] = 0x80;   /* ALGO Rreference               */
   Cmd [7] = 0x01;   /* RSA PKCS#1                    */
   Cmd [8] = 0x84;   /* TAG for private key reference */
   if ( dwSignType == SIGN_TYPE_DIGSIG )
   {
      Cmd [9] = 0x82;
   }
   else if ( dwSignType == SIGN_TYPE_NONREP )
   {
      Cmd [9] = 0x83;
   }
   else
   {
      /* Invalid signing type */
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid signing type...[%d]", dwSignType);
      CLEANUP(dwReturn);
   }
   uiCmdLg = 10;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SET) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SET Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }
#endif

   /* Sign Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0x2A;   /* PSO: Compute Digital Signature COMMAND */
   Cmd [2] = 0x9E;
   Cmd [3] = 0x9A;

   /* Length of data to be signed   */
   switch (HashAlgo)
   {
   case CALG_MD2:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD2");
      cbHdrHash = sizeof(MD2_AID);
      pbHdrHash = MD2_AID;
      break;
   case CALG_MD4:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD4");
      cbHdrHash = sizeof(MD4_AID);
      pbHdrHash = MD4_AID;
      break;
   case CALG_MD5:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD5");
      cbHdrHash = sizeof(MD5_AID);
      pbHdrHash = MD5_AID;
      break;
   case CALG_SHA1:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA1");
      cbHdrHash = sizeof(SHA1_AID);
      pbHdrHash = SHA1_AID;
      break;
   case CALG_SHA_256:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_256");
      cbHdrHash = sizeof(SHA256_AID);
      pbHdrHash = SHA256_AID;
      break;
   case CALG_SHA_384:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_384");
      cbHdrHash = sizeof(SHA384_AID);
      pbHdrHash = SHA384_AID;
      break;
   case CALG_SHA_512:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_512");
      cbHdrHash = sizeof(SHA512_AID);
      pbHdrHash = SHA512_AID;
      break;
   default:
      break;
   }
   Cmd [4] = (BYTE)(cbToBeSigned + cbHdrHash);
   memcpy(Cmd + 5, pbHdrHash, cbHdrHash);
   memcpy(Cmd + 5 + cbHdrHash, pbToBeSigned, cbToBeSigned);
   uiCmdLg = 5 + cbHdrHash + cbToBeSigned;

#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\signdata.bin", cbHdrHash + cbToBeSigned, (char *)&Cmd[5]);
#endif

   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SIGN) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];

   if ( (SW1 != 0x61) || (SW2 != 0x80))
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Sign Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

   /* Retrieve signature Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0xC0;   /* PSO: GET RESPONSE COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x80;   /* Length of response */
   uiCmdLg = 5;

   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (Get Response) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Get Response Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   if ( (recvlen - 2) != 0x80 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X][0x%02X]", recvlen - 2, 0x80);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   *pcbSignature = 0x80;

   /* Allocate memory for the target buffer */
   *ppbSignature = pCardData->pfnCspAlloc(*pcbSignature);
   if ( *ppbSignature == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbSignature]");
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   /* Copy the signature */
   for ( i = 0 ; i < *pcbSignature ; i++ )
   {
      (*ppbSignature)[i] = recvbuf[*pcbSignature - i - 1];
   }

cleanup:
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
      CLEANUP(SCARD_E_UNEXPECTED);
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


#define WHERE "BeidReadFile"
DWORD BeidReadFile(PCARD_DATA  pCardData, DWORD dwOffset, DWORD *cbStream, PBYTE pbStream)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   DWORD             cbRead      = 0;
   DWORD             cbPartRead  = 0;

   /***************/
   /* Read File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xB0; /* READ BINARY COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x00;
   uiCmdLg = 5;

   while ( ( *cbStream - cbRead ) > 0 )
   {
		Cmd[2] = (BYTE)((dwOffset + cbRead) >> 8);   /* set reading startpoint     */
		Cmd[3] = (BYTE)(dwOffset + cbRead);

      cbPartRead = *cbStream - cbRead;
		if(cbPartRead > BEID_READ_BINARY_MAX_LEN)    /*if more than maximum length */
      {
         Cmd[4] = BEID_READ_BINARY_MAX_LEN;        /* is requested, than read    */
      }
		else                                         /* maximum length             */
      {
			Cmd[4] = (BYTE)(cbPartRead);
      }
      dwReturn = SCardTransmit(pCardData->hScard, 
                               &ioSendPci, 
                               Cmd, 
                               uiCmdLg, 
                               &ioRecvPci, 
                               recvbuf, 
                               &recvlen);
      if ( dwReturn != SCARD_S_SUCCESS )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
         CLEANUP(dwReturn);
      }
      SW1 = recvbuf[recvlen - 2];
      SW2 = recvbuf[recvlen - 1];
      if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
         CLEANUP(dwReturn);
      }

      memcpy (pbStream + cbRead, recvbuf, recvlen - 2);
      cbRead += recvlen - 2;
   }

cleanup:
   return (dwReturn);
}
#undef WHERE

#define WHERE "BeidReadCert"
DWORD BeidReadCert(PCARD_DATA  pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   BYTE              bFileID[6] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x00};
   BYTE              cbFileID   = sizeof(bFileID);

   BYTE              bRead [255];
   DWORD             cbRead;

   DWORD             cbCertif;

   /***************/
   /* Select File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xA4; /* SELECT COMMAND */
   Cmd [2] = 0x08;
   Cmd [3] = 0x0C;
   Cmd [4] = cbFileID;
   uiCmdLg = 5;

   switch (dwCertSpec)
   {
   case CERT_AUTH:
      bFileID [5] = 0x38;
      break;
   case CERT_NONREP:
      bFileID [5] = 0x39;
      break;
   case CERT_CA:
      bFileID [5] = 0x3a;
      break;
   case CERT_ROOTCA:
      bFileID [5] = 0x3b;
      break;
   }

   memcpy(&Cmd[5], bFileID, cbFileID);
   uiCmdLg += cbFileID;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

   /* Read First 4 bytes */
   cbRead = 4;
   dwReturn = BeidReadFile(pCardData, 0, &cbRead, bRead);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   cbCertif = (bRead[2] << 8) + bRead[3] + 4;
   cbRead = cbCertif;

   *ppbCertif = pCardData->pfnCspAlloc(cbCertif);

   dwReturn = BeidReadFile(pCardData, 0, &cbCertif, *ppbCertif);
   if ( ( dwReturn != SCARD_S_SUCCESS ) ||
        ( cbCertif != cbRead          ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   /* Certificate Length */
   *pcbCertif = cbCertif;

cleanup:
   return (dwReturn);
}
#undef WHERE

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

