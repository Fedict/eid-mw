// ScConfig.cpp: implementation of the CScConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/file.h"

#include "ScConfig.h"
#include "BelgianEIDConfig.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScConfig::CScConfig()
{

}

CScConfig::~CScConfig()
{

}

CScConfig *CScConfig::GetConfigObject (const wxString & strFileName)
{
    CScConfig *pConfig = NULL;
    DWORD dwFileType = 0;

    try
    {
        // Open the configuration file
        if(!wxFile::Exists(strFileName))
            return pConfig;

        wxFile oConfigFile(strFileName);

        if(oConfigFile.IsOpened())
        {
            // Read the first 4 bytes, they contain the type of configuration file 
            oConfigFile.Read (&dwFileType, sizeof (DWORD));
            // Close the configuration data file, we don't need it anymore
            oConfigFile.Close ();
        }
        if(dwFileType == BELGIAN_EID_CARD)
        {
            // Create an object of the correct type
            pConfig = new CBelgianEIDConfig;
            // Instruct the object to reads its configuration data
            ((CBelgianEIDConfig *)pConfig)->Read (strFileName);
        }
    }
    catch (...)
    {
        if (NULL != pConfig)
        {
            delete (pConfig);
            pConfig = NULL;
        }
    }

    return pConfig;
}

CScConfig *CScConfig::CreateConfigObject (DWORD dwID)
{
   CScConfig *pConfig = NULL;
   if(dwID == BELGIAN_EID_CARD)
   {
       // Create an object of the correct type
       pConfig = new CBelgianEIDConfig;
   }
   
   return pConfig;
}
