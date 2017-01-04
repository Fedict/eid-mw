
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
	 

/********************************************************************************
*
*	dlgs.cpp
*
********************************************************************************/ 
#include <stdlib.h>
#include <signal.h>
#include "errno.h"
	
#include "mw_util.h"
#include "dialogs.h"
#include "langutil.h"
	
#include "sharedmem.h"
#include <map>
	
#include "log.h"
#include "util.h"
#include "mwexception.h"
#include "eiderrors.h"
#include "config.h"
	
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFUserNotification.h>
	using namespace eIDMW;
 std::map < unsigned long, DlgRunningProc * >dlgPinPadInfoCollector;
unsigned long dlgPinPadInfoCollectorIndex = 0;

 std::string csServerName = "beiddialogsQTsrv";
 bool bRandInitialized = false;
 static bool g_bSystemCallsFail = false;

 

	/************************
	*       DIALOGS
	************************/ 
	
//TODO: Add Keypad possibility in DlgAskPin(s)                                      
	DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
					    DlgPinUsage usage,
					    const wchar_t * wsPinName,
					    DlgPinInfo pinInfo,
					    wchar_t * wsPin,
					    unsigned long ulPinBufferLen) 
{
	DlgRet lRet = DLG_CANCEL;
	 DlgAskPINArguments * oData;
	SharedMem oShMemory;
	std::string csReadableFilePath;
	 try
	{
		csReadableFilePath = CreateRandomFile();
		 
			// creating the shared memory segment
			// attach oData 
			oShMemory.Attach(sizeof(DlgAskPINArguments),
					 csReadableFilePath.c_str(),
					 (void **) &oData);
		 
			// collect the arguments into the struct placed 
			// on the shared memory segment
			oData->operation = operation;
		oData->usage = usage;
		wcscpy_s(oData->pinName,
			  sizeof(oData->pinName) / sizeof(wchar_t),
			  wsPinName);
		oData->pinInfo = pinInfo;
		wcscpy_s(oData->pin, sizeof(oData->pin) / sizeof(wchar_t),
			  wsPin);
		 CallQTServer(DLG_ASK_PIN, csReadableFilePath.c_str());
		lRet = oData->returnValue;
		 if (lRet == DLG_OK)
		{
			wcscpy_s(wsPin, ulPinBufferLen, oData->pin);
		}
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
	}
	catch( ...)
	{
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
		 return DLG_ERR;
	 }
	return lRet;
}

   DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
					    DlgPinUsage usage,
					    const wchar_t * wsPinName,
					    DlgPinInfo pin1Info,
					    wchar_t * wsPin1,
					    unsigned long ulPin1BufferLen,
					    DlgPinInfo pin2Info,
					    wchar_t * wsPin2,
					    unsigned long ulPin2BufferLen) 
{
	 DlgRet lRet = DLG_CANCEL;
	 DlgAskPINsArguments * oData;
	SharedMem oShMemory;
	std::string csReadableFilePath;
	 try
	{
		csReadableFilePath = CreateRandomFile();
		 
			// creating the shared memory segment
			// attach oData 
			oShMemory.Attach(sizeof(DlgAskPINsArguments),
					 csReadableFilePath.c_str(),
					 (void **) &oData);
		 
			// collect the arguments into the struct placed 
			// on the shared memory segment
			oData->operation = operation;
		oData->usage = usage;
		wcscpy_s(oData->pinName,
			  sizeof(oData->pinName) / sizeof(wchar_t),
			  wsPinName);
		oData->pin1Info = pin1Info;
		oData->pin2Info = pin2Info;
		wcscpy_s(oData->pin1, sizeof(oData->pin1) / sizeof(wchar_t),
			  wsPin1);
		wcscpy_s(oData->pin2, sizeof(oData->pin2) / sizeof(wchar_t),
			  wsPin2);
		 CallQTServer(DLG_ASK_PINS, csReadableFilePath.c_str());
		lRet = oData->returnValue;
		 if (lRet == DLG_OK)
		{
			wcscpy_s(wsPin1, ulPin1BufferLen, oData->pin1);
			wcscpy_s(wsPin2, ulPin2BufferLen, oData->pin2);
		}
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
	}
	catch( ...)
	{
		
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
		 return DLG_ERR;
	}
	return lRet;
}

   DLGS_EXPORT DlgRet eIDMW::DlgBadPin(DlgPinUsage usage,
					   const wchar_t * wsPinName,
					   unsigned long ulRemainingTries) 
{
	DlgRet lRet = DLG_CANCEL;
	 DlgBadPinArguments * oData;
	SharedMem oShMemory;
	std::string csReadableFilePath;
	 try
	{
		 csReadableFilePath = CreateRandomFile();
		 
			// creating the shared memory segment
			// attach oData 
			oShMemory.Attach(sizeof(DlgBadPinArguments),
					  csReadableFilePath.c_str(),
					  (void **) &oData);
		 
			// collect the arguments into the struct placed 
			// on the shared memory segment
			oData->usage = usage;
		wcscpy_s(oData->pinName,
			  sizeof(oData->pinName) / sizeof(wchar_t),
			  wsPinName);
		oData->ulRemainingTries = ulRemainingTries;
		 CallQTServer(DLG_BAD_PIN, csReadableFilePath.c_str());
		lRet = oData->returnValue;
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
	}
	catch( ...)
	{
		
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
		 return DLG_ERR;
	}
	return lRet;
 }

 DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
						  const wchar_t * wsReader,
						  DlgPinUsage usage,
						  const wchar_t * wsPinName,
						  const wchar_t * wsMessage,
						  void
						  **puserNotificationRef) 
{
	DlgRet lRet = DLG_CANCEL;
	CFArrayRef titlesArray;
	 SharedMem oShMemory;
	std::string csReadableFilePath;
	const wchar_t *Title;

	CFUserNotificationRef userNotificationRef;
	SInt32 error = 0;
	char datachar[256];
	char titlechar[256];

	CFOptionFlags optionFlags;
	 const void *keys[] =
		{ kCFUserNotificationAlertHeaderKey,
	kCFUserNotificationAlertMessageKey
	};
	 if (usage == DLG_PIN_SIGN)
		
	{
		optionFlags = kCFUserNotificationCautionAlertLevel;
		Title = GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);	//SigningWith);
	}
	
	else
		
	{
		optionFlags = kCFUserNotificationPlainAlertLevel;
		Title = GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
	}
	 wcstombs(datachar, wsReader, sizeof(datachar));
	datachar[255] = '\0';
	 CFStringRef datacharRefBytes =
		CFStringCreateWithBytes(kCFAllocatorDefault,
					(const UInt8 *) datachar,
					strlen(datachar),
					kCFStringEncodingUTF8, false );
	 wcstombs(titlechar, Title, sizeof(titlechar));
	titlechar[255] = '\0';
	CFStringRef titlecharRefBytes =
		CFStringCreateWithBytes(kCFAllocatorDefault,
					(const UInt8 *) titlechar,
					strlen(titlechar),
					kCFStringEncodingUTF8, false );
	 const void *textFieldTitles[] = { Title };
	 titlesArray = CFArrayCreate(NULL, textFieldTitles, 1, NULL);
	 const void *values[] = { titlecharRefBytes, datacharRefBytes
	};
	 CFDictionaryRef parameters =
		CFDictionaryCreate(0, keys, values,
				   sizeof(keys) / sizeof(*keys), NULL,
				   NULL);
	 userNotificationRef = CFUserNotificationCreate(kCFAllocatorDefault,	//CFAllocatorRef allocator,
							  30,	//CFTimeInterval timeout,
							  optionFlags,	//CFOptionFlags flags,
							  &error,	//SInt32 *error,
							  parameters);	//CFDictionaryRef dictionary
	*puserNotificationRef = userNotificationRef;
	 CFRelease(titlesArray);
	CFRelease(parameters);
	 return lRet;
}

   DLGS_EXPORT void eIDMW::
DlgClosePinpadInfo(void *theUserNotificationRef) 
{
	SInt32 error = 0;
	 CFUserNotificationRef userNotificationRef =
		(CFUserNotificationRef) theUserNotificationRef;
	error = CFUserNotificationCancel(userNotificationRef);
	 CFRelease(userNotificationRef);
 }  DLGS_EXPORT void eIDMW::DlgCloseAllPinpadInfo() 
{
	 
		// check if we have this handle
		for (std::map < unsigned long,
		     DlgRunningProc * >::iterator pIt =
		     dlgPinPadInfoCollector.begin();
		     pIt != dlgPinPadInfoCollector.end(); ++pIt)
	{
		 
			// check if the process is still running
			// and send SIGTERM if so
			if (!kill(pIt->second->tRunningProcess, 0))
		{
			 MWLOG(LEV_INFO, MOD_DLG,
				 L"  eIDMW::DlgCloseAllPinpadInfo :  sending kill signal to process %d\n",
				 pIt->second->tRunningProcess);
			 if (kill(pIt->second->tRunningProcess, SIGINT))
			{
				MWLOG(LEV_ERROR, MOD_DLG,
				       L"  eIDMW::DlgCloseAllPinpadInfo sent signal SIGINT to proc %d : %s ",
				       pIt->second->tRunningProcess,
				       strerror(errno));
				throw CMWEXCEPTION(EIDMW_ERR_SIGNAL);
			}
		} else
		{
			MWLOG(LEV_ERROR, MOD_DLG,
			       L"  eIDMW::DlgCloseAllPinpadInfo sent signal 0 to proc %d : %s ",
			       pIt->second->tRunningProcess,
			       strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_SIGNAL);
		}
		 
			// delete the random file
			DeleteFile(pIt->second->csRandomFilename.c_str());
		 delete pIt->second;
		pIt->second = NULL;
		 
			// memory is cleaned up in the child process
	}
	
		// delete the map 
		dlgPinPadInfoCollector.clear();
}

   DLGS_EXPORT DlgRet eIDMW::DlgDisplayModal(DlgIcon icon,
						 DlgMessageID messageID,
						 const wchar_t * csMesg,
						 unsigned char ulButtons,
						 unsigned char ulEnterButton,
						 unsigned char
						 ulCancelButton) 
{
	 DlgRet lRet = DLG_CANCEL;
	 DlgDisplayModalArguments * oData;
	SharedMem oShMemory;
	std::string csReadableFilePath;
	 try
	{
		csReadableFilePath = CreateRandomFile();
		 oShMemory.Attach(sizeof(DlgDisplayModalArguments),
				    csReadableFilePath.c_str(),
				    (void **) &oData);
		 oData->icon = icon;
		if (wcslen(csMesg) == 0)
			
		{
			std::wstring translatedMessage(CLang::
							GetMessageFromID
							(messageID));
			wcscpy_s(oData->mesg,
				  sizeof(oData->mesg) / sizeof(wchar_t),
				  translatedMessage.c_str());
		}
		
		else
			
		{
			wcscpy_s(oData->mesg,
				  sizeof(oData->mesg) / sizeof(wchar_t),
				  csMesg);
		}
		oData->buttons = ulButtons;
		oData->EnterButton = ulCancelButton;	//ulEnterButton;
		oData->CancelButton = ulCancelButton;
		 CallQTServer(DLG_DISPLAY_MODAL,
				csReadableFilePath.c_str());
		lRet = oData->returnValue;
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
	 }
	catch( ...)
	{
		
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
		 return DLG_ERR;
	 }
	return lRet;
}

   DLGS_EXPORT DlgRet eIDMW::DlgAskAccess(const wchar_t * wsAppPath,
					      const wchar_t * wsReaderName,
					      DlgPFOperation ulOperation,
					      int *piForAllOperations) 
{
	 DlgRet lRet = DLG_CANCEL;
	 DlgAskAccessArguments * oData = NULL;
	SharedMem oShMemory;
	std::string csReadableFilePath;
	 try
	{
		csReadableFilePath = CreateRandomFile();
		 
			// attach to the segment and get a pointer
			oShMemory.Attach(sizeof(DlgAskAccessArguments),
					 csReadableFilePath.c_str(),
					 (void **) &oData);
		 wcscpy_s(oData->appPath,
			    sizeof(oData->appPath) / sizeof(wchar_t),
			    wsAppPath);
		wcscpy_s(oData->readerName,
			  sizeof(oData->readerName) / sizeof(wchar_t),
			  wsReaderName);
		oData->operation = ulOperation;
		oData->forAllOperations = *piForAllOperations;
		 CallQTServer(DLG_ASK_ACCESS, csReadableFilePath.c_str());
		lRet = oData->returnValue;
		 if (lRet == DLG_OK)
			*piForAllOperations = oData->forAllOperations;
		 
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
	}
	catch( ...)
	{
		
			// detach from the segment
			oShMemory.Detach(oData);
		 
			// delete the random file
			DeleteFile(csReadableFilePath.c_str());
		return DLG_ERR;
	}
	return lRet;
 }

 

/***************************
 *       Helper Functions
 ***************************/ 
void eIDMW::InitializeRand()
{
	if (bRandInitialized)
		return;
	srand(time(NULL));
	bRandInitialized = true;
	return;
}

  std::string eIDMW::RandomFileName()
{
	 InitializeRand();
	 
		// start the filename with a dot, so that it is not visible with a normal 'ls'
		std::string randomFileName = "/tmp/.file_";
	char rndmString[13];

	sprintf(rndmString, "%012d", rand());
	 randomFileName += rndmString;
	 return randomFileName;
}

  std::string eIDMW::CreateRandomFile()
{
	 std::string csFilePath = RandomFileName();
	
		// create this file
	char csCommand[100];

	sprintf(csCommand, "touch %s", csFilePath.c_str());
	if (system(csCommand) != 0)
	{
		
			// If this lib is used by acroread, all system() calls
			// seems to return -1 for some reason, even if the
			// call was successfull.
			FILE * test = fopen(csFilePath.c_str(), "r");
		if (test)
		{
			fclose(test);
			g_bSystemCallsFail = true;
			MWLOG(LEV_WARN, MOD_DLG,
			       L"  eIDMW::CreateRandomFile %s : %s (%d)",
			       csFilePath.c_str(), strerror(errno), errno);
		}
		
		else
		{
			MWLOG(LEV_ERROR, MOD_DLG,
			       L"  eIDMW::CreateRandomFile %s : %s (%d)",
			       csFilePath.c_str(), strerror(errno), errno);
			throw CMWEXCEPTION(EIDMW_ERR_SYSTEM);
		}
	}
	return csFilePath;
}

 void eIDMW::DeleteFile(const char *csFilename)
{
	char csCommand[100];

	sprintf(csCommand, " [ -e %s ] && rm %s", csFilename, csFilename);
	if (system(csCommand) != 0)
	{
		MWLOG(g_bSystemCallsFail ? LEV_WARN : LEV_ERROR, MOD_DLG,
		       L"  eIDMW::DeleteFile %s : %s ", csFilename,
		       strerror(errno));
		
			//throw CMWEXCEPTION(EIDMW_ERR_SYSTEM);
	}
}

 void eIDMW::CallQTServer(const DlgFunctionIndex index,
			    const char *csFilename)
{
	char csCommand[100];

	sprintf(csCommand, "%s %i %s ", csServerName.c_str(), index,
		 csFilename);
	int code = system(csCommand);

	if (code != 0)
	{
		MWLOG(g_bSystemCallsFail ? LEV_WARN : LEV_ERROR, MOD_DLG,
		       L"  eIDMW::CallQTServer %i %s : %s ", index,
		       csFilename, strerror(errno));
		if (!g_bSystemCallsFail)
			throw CMWEXCEPTION(EIDMW_ERR_SYSTEM);
	}
	return;
}


