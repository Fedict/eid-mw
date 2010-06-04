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
#include "globmdrv.h"

#include "log.h"
#include "util.h"

/****************************************************************************************************/

//
// Function: CardCreateDirectory
//
// Purpose: Register the specified application name on the card, and apply the
//          provided access condition.
//
// Return Value:
//          ERROR_FILE_EXISTS - directory already exists
//

#define WHERE "CardCreateDirectory()"
DWORD WINAPI   CardCreateDirectory
               (
                  __in    PCARD_DATA                        pCardData,
                  __in    LPSTR                             pszDirectoryName,
                  __in    CARD_DIRECTORY_ACCESS_CONDITION   AccessCondition
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeleteDirectory
//
// Purpose: Unregister the specified application from the card.
//
// Return Value:
//          SCARD_E_DIR_NOT_FOUND - directory does not exist
//          ERROR_DIR_NOT_EMPTY - the directory is not empty
//

#define WHERE "CardDeleteDirectory()"
DWORD WINAPI   CardDeleteDirectory
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardReadFile
//
// Purpose: Read the specified file from the card.
//
//          The pbData parameter should be allocated
//          by the card module and freed by the CSP.  The card module
//          must set the cbData parameter to the size of the returned buffer.
//

#define WHERE "CardReadFile()"
DWORD WINAPI   CardReadFile
(
   __in                             PCARD_DATA  pCardData,
   __in                             LPSTR       pszDirectoryName,
   __in                             LPSTR       pszFileName,
   __in                             DWORD       dwFlags,
   __deref_out_bcount(*pcbData)     PBYTE      *ppbData,
   __out                            PDWORD      pcbData
)
{
   DWORD                dwReturn    = 0;
   int                  i           = 0;
   int                  DirFound    = 0;
   int                  FileFound   = 0;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszFileName    == NULL ) ||
        ( pszFileName[0] == '\0' ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ppbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [ppbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pcbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pcbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( pszDirectoryName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [NULL]");
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [%s]", pszDirectoryName);
   }
   if ( pszFileName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }

   /*
    * Read file from Virtual File List 
    */
   pCardItem = GetCardListItem(pCardData);
   if ( pCardItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Card context and handle not Found...");
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   for ( i = 1 ; i <= ITEM_CNT(&pCardItem->ObjectList) ; i++)
   {
      Goto_item_in_list(&pCardItem->ObjectList, i);
      pObjectItem = (POBJECT_LIST_TYPE) CURR_PTR(&pCardItem->ObjectList);

      if ( ( ( pszDirectoryName == NULL ) && ( pObjectItem->szDirectoryName[0]                          == '\0' ) ) ||
           ( ( pszDirectoryName != NULL ) && ( _stricmp(pszDirectoryName, pObjectItem->szDirectoryName) == 0    ) ) )
      {
         DirFound++;

         if ( _stricmp(pObjectItem->szFileName, pszFileName) == 0 )
         {
            /* Found file */
            FileFound++;
            break;
         }
      }
   }

   if ( ! FileFound )
   {
      if ( ! DirFound )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Dir not found");
         CLEANUP(SCARD_E_DIR_NOT_FOUND);
      }
      else
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "File not found");
         CLEANUP(SCARD_E_FILE_NOT_FOUND);
      }
   }
   
   if ( ( _stricmp(pObjectItem->szFileName, "msroots") == 0 ) &&
        ( pObjectItem->ObjectDataSize                  == 0 ) )
   {
      dwReturn = BeidCreateMSRoots(pCardData, &(pObjectItem->ObjectDataSize), (PBYTE *)&(pObjectItem->pObjectData));
      if ( dwReturn != SCARD_S_SUCCESS )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "BeidCreateMSRoots returned [%d]", dwReturn);
         CLEANUP(SCARD_E_UNEXPECTED);
      }
   }

   *pcbData = pObjectItem->ObjectDataSize;
   *ppbData = (LPVOID)pCardData->pfnCspAlloc(pObjectItem->ObjectDataSize);
   if ( *ppbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
   memcpy (*ppbData, pObjectItem->pObjectData, pObjectItem->ObjectDataSize);

   LogTrace(LOGTYPE_INFO, WHERE, "#bytes: [%d]", pObjectItem->ObjectDataSize);

#ifdef _DEBUG
   LogDump (*pcbData, (char *)*ppbData);
#endif

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardCreateFile
//

#define WHERE "CardCreateFile()"
DWORD WINAPI   CardCreateFile
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName,
                  __in    LPSTR       pszFileName,
                  __in    DWORD       cbInitialCreationSize,
                  __in    CARD_FILE_ACCESS_CONDITION AccessCondition
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardGetFileInfo
//

#define WHERE "CardGetFileInfo()"
DWORD WINAPI   CardGetFileInfo
               (
                  __in         PCARD_DATA       pCardData,
                  __in         LPSTR            pszDirectoryName,
                  __in         LPSTR            pszFileName,
                  __in         PCARD_FILE_INFO  pCardFileInfo
               )
{
   DWORD                dwReturn    = 0;
   int                  i           = 0;
   int                  FileFound   = 0;
   int                  DirFound    = 0;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;
   DWORD                dwVersion   = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszFileName    == NULL ) ||
        ( pszFileName[0] == '\0' ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pCardFileInfo == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   dwVersion = (pCardFileInfo->dwVersion == 0) ? 1 : pCardFileInfo->dwVersion;
   if ( dwVersion != CARD_FILE_INFO_CURRENT_VERSION ) 
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo->dwVersion]");
      CLEANUP(ERROR_REVISION_MISMATCH );
   }

   /* Some trace info */
   if ( pszDirectoryName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [NULL]");
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [%s]", pszDirectoryName);
   }
   if ( pszFileName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }

   /*
    * Read file from Virtual File List 
    */
   pCardItem = GetCardListItem(pCardData);
   if ( pCardItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Card context and handle not Found...");
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   for ( i = 1 ; i <= ITEM_CNT(&pCardItem->ObjectList) ; i++)
   {
      Goto_item_in_list(&pCardItem->ObjectList, i);
      pObjectItem = (POBJECT_LIST_TYPE) CURR_PTR(&pCardItem->ObjectList);

      if ( ( ( pszDirectoryName == NULL ) && ( pObjectItem->szDirectoryName[0]                          == '\0' ) ) ||
           ( ( pszDirectoryName != NULL ) && ( _stricmp(pszDirectoryName, pObjectItem->szDirectoryName) == 0    ) ) )
      {
         DirFound++;

         if ( _stricmp(pObjectItem->szFileName, pszFileName) == 0 )
         {
            /* Found file */
            FileFound++;
            break;
         }
      }
   }

   if ( ! FileFound )
   {
      if ( ! DirFound )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Dir not found");
         CLEANUP(SCARD_E_DIR_NOT_FOUND);
      }
      else
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "File not found");
         CLEANUP(SCARD_E_FILE_NOT_FOUND);
      }
   }

   pCardFileInfo->dwVersion       = CARD_FILE_INFO_CURRENT_VERSION;
   pCardFileInfo->cbFileSize      = pObjectItem->ObjectDataSize;
   pCardFileInfo->AccessCondition = pObjectItem->bAccessCondition;

   LogTrace(LOGTYPE_INFO, WHERE, "FileInfo: [%d][%d][%d]"
                        , pCardFileInfo->dwVersion
                        , pCardFileInfo->cbFileSize
                        , pCardFileInfo->AccessCondition);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardWriteFile
//

#define WHERE "CardWriteFile()"
DWORD WINAPI   CardWriteFile
               (
                  __in                     PCARD_DATA  pCardData,
                  __in                     LPSTR       pszDirectoryName,
                  __in                     LPSTR       pszFileName,
                  __in                     DWORD       dwFlags,
                  __in_bcount(cbData)      PBYTE       pbData,
                  __in                     DWORD       cbData
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeleteFile
//
#define WHERE "CardDeleteFile()"
DWORD WINAPI   CardDeleteFile
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName,
                  __in    LPSTR       pszFileName,
                  __in    DWORD       dwFlags
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardEnumFiles
//
// Purpose: Return a multi-string list of the general files
//          present on this card.  The multi-string is allocated
//          by the card module and must be freed by the CSP.
//
//  The caller must provide a logical file directory name in the
//  pmwszFileNames parameter (see Logical Directory Names, above).
//  The logical directory name indicates which group of files will be
//  enumerated.
//
//  The logical directory name is expected to be a static string, so the
//  the card module will not free it.  The card module
//  will allocate a new buffer in *pmwszFileNames to store the multi-string
//  list of enumerated files using pCardData->pfnCspAlloc.
//
//  If the function fails for any reason, *pmwszFileNames is set to NULL.
//

#define WHERE "CardEnumFiles()"
DWORD WINAPI   CardEnumFiles
               (
                  __in                          PCARD_DATA  pCardData,
                  __in                          LPSTR       pszDirectoryName,
                  __out_ecount(*pdwcbFileName)  LPSTR      *pmszFileNames,
                  __out                         LPDWORD     pdwcbFileName,
                  __in                          DWORD       dwFlags
               )
{
   DWORD                dwReturn    = 0;
   PCARD_LIST_TYPE      pCardItem   = NULL;
   POBJECT_LIST_TYPE    pObjectItem = NULL;
   int                  i           = 0;

   int                  iFileNameLg = 0;
   int                  iFileCnt    = 0;
   int                  iIndex      = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pmszFileNames == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pmszFileNames]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pdwcbFileName == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pdwcbFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   pCardItem = GetCardListItem(pCardData);
   if ( pCardItem == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Card context and handle not Found...");
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   for ( i = 1 ; i <= ITEM_CNT(&pCardItem->ObjectList) ; i++)
   {
      Goto_item_in_list(&pCardItem->ObjectList, i);
      pObjectItem = (POBJECT_LIST_TYPE) CURR_PTR(&pCardItem->ObjectList);

      if ( ( ( pszDirectoryName == NULL ) && ( pObjectItem->szDirectoryName[0]                          == '\0' ) ) ||
           ( ( pszDirectoryName != NULL ) && ( _stricmp(pszDirectoryName, pObjectItem->szDirectoryName) == 0    ) ) )
      {
         /* Found directory - Add file to the list */
         iFileNameLg += strlen(pObjectItem->szFileName);
         iFileCnt++;
      }
   }

   if ( ! iFileCnt )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Directory not found");
      CLEANUP(SCARD_E_DIR_NOT_FOUND);
   }

   *pdwcbFileName = iFileNameLg + iFileCnt + 1;
   *pmszFileNames = pCardData->pfnCspAlloc(*pdwcbFileName);
   if ( *pmszFileNames == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*pmszFileNames]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   memset(*pmszFileNames, '\0', *pdwcbFileName);
   iIndex = 0;
   for ( i = 1 ; i <= ITEM_CNT(&pCardItem->ObjectList) ; i++)
   {
      Goto_item_in_list(&pCardItem->ObjectList, i);
      pObjectItem = (POBJECT_LIST_TYPE) CURR_PTR(&pCardItem->ObjectList);

      if ( ( ( pszDirectoryName == NULL ) && ( pObjectItem->szDirectoryName[0]                          == '\0' ) ) ||
           ( ( pszDirectoryName != NULL ) && ( _stricmp(pszDirectoryName, pObjectItem->szDirectoryName) == 0    ) ) )
      {
         /* Found directory - Add file to the list */
         strcpy(*pmszFileNames + iIndex, pObjectItem->szFileName);
         iIndex += strlen(pObjectItem->szFileName) + 1;
      }
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardQueryFreeSpace
//

#define WHERE "CardQueryFreeSpace()"
DWORD WINAPI   CardQueryFreeSpace
               (
                  __in  PCARD_DATA              pCardData,
                  __in  DWORD                   dwFlags,
                  __in  PCARD_FREE_SPACE_INFO   pCardFreeSpaceInfo
               )
{
   DWORD    dwReturn  = 0;
   DWORD    dwVersion = 0;

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
   if ( pCardFreeSpaceInfo == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFreeSpaceInfo]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   dwVersion = (pCardFreeSpaceInfo->dwVersion == 0) ? 1 : pCardFreeSpaceInfo->dwVersion;
   if ( dwVersion != CARD_FREE_SPACE_INFO_CURRENT_VERSION )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo->dwVersion]");
      CLEANUP(ERROR_REVISION_MISMATCH );
   }

   /* We have a read-only card */
   pCardFreeSpaceInfo->dwBytesAvailable          = 0;
   pCardFreeSpaceInfo->dwKeyContainersAvailable  = 0;
   pCardFreeSpaceInfo->dwMaxKeyContainers        = 2; /* Authentication and Non-repudiation key */

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
