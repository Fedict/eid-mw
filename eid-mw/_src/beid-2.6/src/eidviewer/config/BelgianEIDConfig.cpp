// BelgianEIDConfig.cpp: implementation of the CBelgianEIDConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "BelgianEIDConfig.h"

#include "wx/file.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBelgianEIDConfig::CBelgianEIDConfig()
{
   m_pbCardID = NULL;
   m_dwCardIDLen = 0;
   m_tpSlotInfo.clear ();     // Make sure the vector is empty
}

CBelgianEIDConfig::~CBelgianEIDConfig()
{
    if (NULL != m_pbCardID)
    {
        delete [] m_pbCardID;
        m_pbCardID = NULL;
        m_dwCardIDLen = 0;
    }
    CSlotInfo *pSlotInfo = NULL;

    for (unsigned int i = 0; i  < m_tpSlotInfo.size (); ++i)
    {
        pSlotInfo = m_tpSlotInfo[i];
        if(pSlotInfo != NULL)
        {
            delete (pSlotInfo);
            pSlotInfo = NULL;
        }
    }
    m_tpSlotInfo.clear(); 
}

bool CBelgianEIDConfig::Read (const wxString & strFileName)
{
    bool bRet = true;
    DWORD dwFileType = 0;
    DWORD dwDataLg = 0;

    if(!wxFile::Exists(strFileName))
        return false;        

   // Open the configuration file
    wxFile oConfigFile(strFileName);

    if(oConfigFile.IsOpened())
    {
        // Read the first 4 bytes, they contain the type of configuration file 
        oConfigFile.Read (&dwFileType, sizeof (DWORD));
        // Read the next 4 bytes, they contain the length the data to come
        oConfigFile.Read (&dwDataLg, sizeof (DWORD));

        // Allocate memory to store the configuration data
        BYTE *pByte = new BYTE [dwDataLg];
        memset(pByte, 0, dwDataLg);

        // Read the configuration data
        oConfigFile.Read (pByte, dwDataLg);

        // Close the configuration data file, we don't need it anymore
        oConfigFile.Close ();

        DWORD dwIndex=0;
        DWORD dwContentFlag=0;

        // Get the Content Flag. This indicates what is stored in the configuration file
        memcpy (&dwContentFlag, &pByte[dwIndex], sizeof (DWORD));
        dwIndex += sizeof (DWORD);

        if ((dwContentFlag & CARD_ID_FLAG) != 0)
        {
            // Read the length of the Card ID string
            DWORD dwIDLength = 0;
            memcpy (&dwIDLength, &pByte[dwIndex], sizeof (DWORD));
            dwIndex += sizeof (DWORD);
   
            // Read the Card ID itself
            SetCardID (&pByte[dwIndex], dwIDLength);
            dwIndex += dwIDLength;
        }

        if ((dwContentFlag & SLOT_INFO_FLAG) != 0)
        {
          DWORD dwNrOfSlots = 0;
          CSlotInfo *pSlotInfo = NULL;

          // Read the number of slots on the card
          memcpy (&dwNrOfSlots, &pByte[dwIndex], sizeof (DWORD));
          dwIndex += sizeof (DWORD);

          // For each slot present on the card, read the information
          for (DWORD i=0; i < dwNrOfSlots; i++)
          {
             pSlotInfo = new CSlotInfo;
             DWORD dwSlotNameLg = 0;
             // Read the length of the Slot name (Label) incl. terminating 0
             memcpy (&dwSlotNameLg, &pByte[dwIndex], sizeof (DWORD));
             dwIndex += sizeof (DWORD);
             // If a slot name is present then store it
             if (dwSlotNameLg > 0)
             {
                pSlotInfo->SetSlotName (&pByte[dwIndex], dwSlotNameLg);
                dwIndex += dwSlotNameLg;
             }

             DWORD dwNrOfKeys=0;
             // Read the number of keys in the slot
             memcpy (&dwNrOfKeys, &pByte[dwIndex], sizeof (DWORD));
             dwIndex += sizeof (DWORD);
             // For each key in the slot, read the information
             for (DWORD j = 0; j < dwNrOfKeys; j++)
             {
                CKeyInfo *pKeyInfo = NULL;
                // Create a new key info object
                pKeyInfo = new CKeyInfo;

                DWORD dwKeyIDLg = 0;
                // Read the Key ID Length (incl. terminating 0)
                memcpy (&dwKeyIDLg, &pByte[dwIndex], sizeof (DWORD));
                dwIndex += sizeof (DWORD);

                // Set the Key ID (Label)   
                pKeyInfo->SetKeyContainerName (&pByte[dwIndex], dwKeyIDLg);
                dwIndex += dwKeyIDLg;

                DWORD dwKeyLength = 0;
                // Read the Key Length
                memcpy (&dwKeyLength, &pByte[dwIndex], sizeof (DWORD));
                dwIndex += sizeof (DWORD);
                // Set the key length
                pKeyInfo->SetKeyLength (dwKeyLength);

                DWORD dwKeyUsage = 0;
                // Read the Key Usage
                memcpy (&dwKeyUsage, &pByte[dwIndex], sizeof (DWORD));
                dwIndex += sizeof (DWORD);
                // Set the key usage
                pKeyInfo->SetKeyUsage (dwKeyUsage);

                DWORD dwKeyID = 0;
                // Read the Key Usage
                memcpy (&dwKeyID, &pByte[dwIndex], sizeof (DWORD));
                dwIndex += sizeof (DWORD);
                // Set the key usage
                pKeyInfo->SetKeyID (dwKeyID);

                // Add the key to the current slot info object
                pSlotInfo->AddKeyInfo (pKeyInfo);
             }
             // Add the slotinfo to this configuration object
             AddSlotInfo (pSlotInfo);
          }
        }
        if(pByte != NULL)
        {
            delete (pByte);
        }
     }
     return bRet;
}

void CBelgianEIDConfig::AddSlotInfo (CSlotInfo *pSlotInfo)
{
   m_tpSlotInfo.push_back (pSlotInfo);
}

void CBelgianEIDConfig::SetCardID (BYTE *pbByte, DWORD dwIDLength)
{
    if (NULL != m_pbCardID)
    {
        delete [] m_pbCardID;
        m_pbCardID = NULL;
        m_dwCardIDLen = 0;
    }

   m_dwCardIDLen = dwIDLength;
   m_pbCardID = new BYTE [m_dwCardIDLen];
   memset (m_pbCardID, 0, m_dwCardIDLen);
   memcpy (m_pbCardID, pbByte, dwIDLength);
}

bool CBelgianEIDConfig::Write(const wxString & strFileName)
{
    bool bReturnVal = true;

    wxFile oConfigFile (strFileName, wxFile::write);
    if(oConfigFile.IsOpened())
    {
      // Write the File type
      DWORD dwFileType = 0x20;
      oConfigFile.Write (&dwFileType, sizeof (DWORD));
      
      // Write the length of the data to come. For now, write 0. The correct length will
      // be inserted at the end of the process.
      DWORD dwDataLg = 0;
      oConfigFile.Write (&dwDataLg, sizeof (DWORD));

      // Set the value of the content flag
      DWORD dwContentFlag = 0;
      if (NULL != m_pbCardID)
         dwContentFlag += CARD_ID_FLAG;
      if (0 <m_tpSlotInfo.size ())
         dwContentFlag += SLOT_INFO_FLAG;
      
      // Write the content flag
      oConfigFile.Write (&dwContentFlag, sizeof (DWORD));
      dwDataLg += sizeof (DWORD);
      
      if ((dwContentFlag & CARD_ID_FLAG) != 0)
      {
         // Write the length of the card ID incl. terminating 0
         DWORD dwCardIDLg = m_dwCardIDLen;
         oConfigFile.Write (&dwCardIDLg, sizeof (DWORD));
         dwDataLg += sizeof (DWORD);

         // Write the Card ID itself
         oConfigFile.Write (m_pbCardID, dwCardIDLg);
         dwDataLg += dwCardIDLg;
      }

      if ((dwContentFlag & SLOT_INFO_FLAG) != 0)
      {
         // Write the number of slots
        DWORD m_dwNrOfSlots = m_tpSlotInfo.size();
         oConfigFile.Write (&m_dwNrOfSlots, sizeof (DWORD));
         dwDataLg += sizeof (DWORD);
         
         CSlotInfo *pSlotInfo=NULL;
         BYTE *pbName=NULL;
         DWORD dwLen = 0;
         // Write information for each slot
         for (unsigned int i = 0; i < m_tpSlotInfo.size (); i++)
         {
            pSlotInfo = m_tpSlotInfo[i];
            // Get the Slot Name (label)
            pbName = pSlotInfo->GetSlotName (&dwLen);
            
            if (NULL != pbName)  // Is there a name specified for the slot?
            {
               DWORD dwNameLg = dwLen;
               // Yes, write the length of the name incl. terminating 0
               oConfigFile.Write (&dwNameLg, sizeof (DWORD));
               dwDataLg += sizeof (DWORD);

               // Write the slot name itself
               oConfigFile.Write (pbName, dwNameLg);
               dwDataLg += dwNameLg;

               // Delete the Slot name buffer, we do not need it any longer. Set the pointer
               // to NULL for the next loop. Memory was allocated by GetSlotName
               delete (pbName);
               pbName = NULL;
               dwLen = 0;
            }
            else  // No name specified for the slot, write 0 length
            {  
               DWORD dwNameLg = 0;
               oConfigFile.Write (&dwNameLg, sizeof (DWORD));
               dwDataLg += sizeof (DWORD);
            }

            // Write the number of keys in the slot
            DWORD dwNrOfKeys = pSlotInfo->GetNrOfKeys ();
            oConfigFile.Write (&dwNrOfKeys, sizeof (DWORD));
            dwDataLg += sizeof (DWORD);

            CKeyInfo *pKeyInfo=NULL;
            for (DWORD j=0; j < dwNrOfKeys; j++)
            {
               // Get this key info
               pKeyInfo = pSlotInfo->GetKeyInfo (j);

               // Get the container name length (label)
               pbName = pKeyInfo->GetKeyContainerName (&dwLen);
               if (NULL != pbName)  // Was a Key ID specified?
               {
                  // Yes, write the length of the Container name 
                  DWORD dwKeyIDLg = dwLen;
                  oConfigFile.Write (&dwKeyIDLg, sizeof (DWORD));
                  dwDataLg += sizeof (DWORD);

                  // Write the container name itself
                  oConfigFile.Write (pbName, dwKeyIDLg);
                  dwDataLg += dwKeyIDLg;

                  // Delete the Slot name buffer, we do not need it any longer. Set the pointer
                  // to NULL for the next loop. Memory was allocated by GetKeyContainerName.
                  delete (pbName);
                  pbName = NULL;
                  dwLen = 0;
               }
               else
               {
                  // No, write 0 as length of the Key ID
                  DWORD dwKeyIDLg = 0;
                  oConfigFile.Write (&dwKeyIDLg, sizeof (DWORD));
                  dwDataLg += sizeof (DWORD);
               }

               // Write the length of this key
               DWORD dwKeyLength = pKeyInfo->GetKeyLength ();
               oConfigFile.Write (&dwKeyLength, sizeof (DWORD));
               dwDataLg += sizeof (DWORD);

               // Write the usage of this key
               DWORD dwKeyUsage = pKeyInfo->GetKeyUsage ();
               oConfigFile.Write (&dwKeyUsage, sizeof (DWORD));
               dwDataLg += sizeof (DWORD);

               //Write the Key ID
               DWORD dwKeyID = pKeyInfo->GetKeyID ();
               oConfigFile.Write (&dwKeyID, sizeof (DWORD));
               dwDataLg += sizeof (DWORD);

            }
         }
      }
      // Write the length of the data to come. First go back to the beginning of the file, 
      // then write the length
      oConfigFile.Seek (4L, wxFromStart);
      oConfigFile.Write (&dwDataLg, sizeof (DWORD));

      // Close the file
      oConfigFile.Close ();
   }

   return (bReturnVal);
}

BYTE *CBelgianEIDConfig::GetCardID (DWORD *pdwLen)
{
    BYTE *pRet = NULL;
   *pdwLen = 0;
    if (NULL != m_pbCardID)
    {  
       pRet = new BYTE[m_dwCardIDLen];
      memset (pRet, 0, m_dwCardIDLen);
      memcpy (pRet, m_pbCardID, m_dwCardIDLen);
     *pdwLen = m_dwCardIDLen;
   }
    return pRet;
}

void CBelgianEIDConfig::ClearSlots()
{ 
    m_tpSlotInfo.clear(); 
}




