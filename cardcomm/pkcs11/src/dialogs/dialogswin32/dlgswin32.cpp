
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2013 FedICT.
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
	 
	
#include "stdafx.h"
#include "../dialogs.h"
#include "dlgwndaskpin.h"
#include "dlgwndaskpins.h"
#include "dlgwndbadpin.h"
#include "dlgwndpinpadinfo.h"
#include "dlgwndmodal.h"
#include "dlgwndaskaccess.h"
#include "resource.h"
#include "../langutil.h"
#include "configuration.h"
#include "common/log.h"
#include "mw_util.h"
	using namespace eIDMW;
 HMODULE g_hDLLInstance = (HMODULE) NULL;
 int g_UseKeyPad = -1;
 typedef std::map < unsigned long, dlgWndPinpadInfo * >TD_WNDPINPAD_MAP;
typedef std::pair < unsigned long, dlgWndPinpadInfo * >TD_WNDPINPAD_PAIR;

 dlgWndPinpadInfo * dlgModal = NULL;
TD_WNDPINPAD_MAP dlgPinPadInfoCollector;
unsigned long dlgPinPadInfoCollectorIndex = 0;

  
#ifdef _MANAGED
#pragma managed(push, off)
#endif	/*  */
	
#ifndef EIDMW_STATIC_LIB
	BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
			      LPVOID lpReserved ) 
{
	switch (ul_reason_for_call)
		
	{
		case DLL_PROCESS_ATTACH:
			g_hDLLInstance = hModule;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


#endif	/*  */
	
#ifdef _MANAGED
#pragma managed(pop)
#endif	/*  */
	bool DlgGetKeyPad() 
{
	if (g_UseKeyPad == -1)
		
	{
		g_UseKeyPad =
			(CConfig::
			 GetLong(CConfig::
				 EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD) ? 1 :
			 0);
	}
	return (g_UseKeyPad == 0 ? false : true);
}

 

	/************************
	*       DIALOGS
	************************/ 
//TODO: Add Keypad possibility in DlgAskPin(s)     
	DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
					    DlgPinUsage usage,
					    const wchar_t * csPinName,
					    DlgPinInfo pinInfo,
					    wchar_t * csPin,
					    unsigned long ulPinBufferLen) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPin() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	g_UseKeyPad = -1;	// Reset the keypad
	dlgWndAskPIN * dlg = NULL;
	try 
	{
		std::wstring PINName;
		if (usage == DLG_PIN_UNKNOWN)
			PINName = csPinName;
		
		else
			PINName = GETSTRING_DLG(Pin);
		 std::wstring sMessage;
		switch (operation)
			
		{
			case DLG_PIN_OP_VERIFY:
				switch (usage)
					
				{
					case DLG_PIN_AUTH:
						sMessage =
							GETSTRING_DLG
							(PleaseEnterYourPin);
						sMessage += L", ";
						sMessage +=
							GETSTRING_DLG
							(InOrderToAuthenticateYourself);
						sMessage += L"\n";
						break;
					case DLG_PIN_SIGN:
						sMessage =
							GETSTRING_DLG
							(Caution);
						sMessage += L" ";
						sMessage +=
							GETSTRING_DLG
							(YouAreAboutToMakeALegallyBindingElectronic);
						sMessage += L"\n";
						sMessage +=
							GETSTRING_DLG
							(PleaseEnterYourPin);
						sMessage += L", ";
						sMessage +=
							GETSTRING_DLG
							(ToContinueOrClickTheCancelButton);
						sMessage += L"\n\n";
						sMessage +=
							GETSTRING_DLG
							(Warning);
						sMessage += L" ";
						sMessage +=
							GETSTRING_DLG
							(IfYouOnlyWantToLogOnToA);
						sMessage += L"\n";
						break;
					default:
						sMessage =
							GETSTRING_DLG
							(PleaseEnterYourPin);
						sMessage += L"\n";
						break;
				}
				 break;
			case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
				sMessage = GETSTRING_DLG(PleaseEnterYourPuk);
				sMessage += L", ";
				sMessage = GETSTRING_DLG(ToUnblock);
				sMessage += L" ";
				sMessage = GETSTRING_DLG(Your);
				sMessage += L" \"";
				if (wcslen(csPinName) == 0)
					sMessage += csPinName;
				
				else
					sMessage += GETSTRING_DLG(Pin);
				sMessage += L"\"\n";
				 break;
			default:
				MWLOG(LEV_DEBUG, MOD_DLG,
				       L"  --> DlgAskPin() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
				break;
		}
		  dlg =
			new dlgWndAskPIN(pinInfo, usage, sMessage, PINName,
					 DlgGetKeyPad());
		if (dlg->exec())
			
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			wcscpy_s(csPin, ulPinBufferLen, dlg->PinResult);
			 delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG,
			       L"  --> DlgAskPin() returns DLG_OK");
			return DLG_OK;
		}
		delete dlg;
	}
	catch( ...) 
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgAskPin() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
					DlgPinUsage usage,
					const wchar_t * csPinName,
					DlgPinInfo pin1Info,
					wchar_t * csPin1,
					unsigned long ulPin1BufferLen,
					DlgPinInfo pin2Info,
					wchar_t * csPin2,
					unsigned long ulPin2BufferLen) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPins() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	g_UseKeyPad = -1;	// Reset the keypad
	dlgWndAskPINs * dlg = NULL;
	try 
	{
		std::wstring PINName;
		std::wstring Header;
		Header = GETSTRING_DLG(EnterYour);
		Header += L" ";
		switch (operation)
			
		{
			case DLG_PIN_OP_CHANGE:
				if (usage == DLG_PIN_UNKNOWN)
					PINName = csPinName;
				
				else
					PINName = GETSTRING_DLG(Pin);
				break;
			case DLG_PIN_OP_UNBLOCK_CHANGE:
				if (usage == DLG_PIN_UNKNOWN)
					PINName = csPinName;
				
				else
					PINName = GETSTRING_DLG(Puk);
				break;
			default:
				MWLOG(LEV_DEBUG, MOD_DLG,
				       L"  --> DlgAskPins() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
				break;
		}
		Header += PINName;
		Header += L" ";
		Header += GETSTRING_DLG(Code);
		 dlg = new dlgWndAskPINs(pin1Info, pin2Info, Header,
					   PINName, DlgGetKeyPad());
		if (dlg->exec())
			
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			wcscpy_s(csPin1, ulPin1BufferLen, dlg->Pin1Result);
			wcscpy_s(csPin2, ulPin2BufferLen, dlg->Pin2Result);
			 delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG,
			       L"  --> DlgAskPins() returns DLG_OK");
			return DLG_OK;
		}
		delete dlg;
	}
	catch( ...) 
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgAskPins() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 DLGS_EXPORT DlgRet eIDMW::DlgBadPin(DlgPinUsage usage,
				       const wchar_t * csPinName,
				       unsigned long ulRemainingTries) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgBadPin() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	dlgWndBadPIN * dlg = NULL;
	try 
	{
		std::wstring PINName;
		switch (usage)
			
		{
			case DLG_PIN_UNKNOWN:
				if (wcslen(csPinName) == 0)
					
				{
					MWLOG(LEV_DEBUG, MOD_DLG,
					       L"  --> DlgBadPin() returns DLG_BAD_PARAM");
					return DLG_BAD_PARAM;
				}
				PINName = csPinName;
				break;
			default:
				if (wcslen(csPinName) == 0)
					PINName = GETSTRING_DLG(Pin);
				
				else
					PINName = csPinName;
				break;
		}
		 dlg = new dlgWndBadPIN(PINName, ulRemainingTries);
		if (dlg->exec())
			
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			
				//csPin = dlg->PinResult;
				delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG,
			       L"  --> DlgBadPin() returns %ld", dlgResult);
			return dlgResult;
		}
		delete dlg;
	}
	catch( ...) 
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgBadPin() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
						  const wchar_t * csReader,
						  DlgPinUsage usage,
						  const wchar_t * csPinName,
						  const wchar_t * csMessage,
						  unsigned long *pulHandle) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgDisplayPinpadInfo() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	try 
	{
		std::wstring PINName;
		switch (usage)
			
		{
			case DLG_PIN_UNKNOWN:
				if (wcslen(csPinName) == 0)
					
				{
					MWLOG(LEV_DEBUG, MOD_DLG,
					       L"  --> DlgDisplayPinpadInfo() returns DLG_BAD_PARAM");
					return DLG_BAD_PARAM;
				}
				PINName = csPinName;
				break;
			default:
				if (wcslen(csPinName) == 0)
					PINName = GETSTRING_DLG(Pin);
				
				else
					PINName = csPinName;
				break;
		}
		 std::wstring sMessage;
		if (wcslen(csMessage) != 0)
			
		{
			sMessage = csMessage;
		}
		
		else
			
		{
			switch (operation)
				
			{
				case DLG_PIN_OP_VERIFY:
					switch (usage)
						
					{
						case DLG_PIN_AUTH:
							sMessage =
								GETSTRING_DLG
								(PleaseEnterYourPinOnThePinpadReader);
							if (wcslen(csReader)
							     != 0)
								
							{
								sMessage +=
									L" \"";
								sMessage +=
									csReader;
								sMessage +=
									L"\"";
							}
							sMessage += L", ";
							sMessage +=
								GETSTRING_DLG
								(InOrderToAuthenticateYourself);
							sMessage += L"\n";
							break;
						case DLG_PIN_SIGN:
							sMessage =
								GETSTRING_DLG
								(Caution);
							sMessage += L" ";
							sMessage +=
								GETSTRING_DLG
								(YouAreAboutToMakeALegallyBindingElectronic);
							sMessage += L"\n";
							sMessage +=
								GETSTRING_DLG
								(PleaseEnterYourPinOnThePinpadReader);
							if (wcslen(csReader)
							     != 0)
								
							{
								sMessage +=
									L" \"";
								sMessage +=
									csReader;
								sMessage +=
									L"\"";
							}
							sMessage += L", ";
							sMessage +=
								GETSTRING_DLG
								(ToContinueOrClickTheCancelButton);
							sMessage += L"\n\n";
							sMessage +=
								GETSTRING_DLG
								(Warning);
							sMessage += L" ";
							sMessage +=
								GETSTRING_DLG
								(IfYouOnlyWantToLogOnToA);
							sMessage += L"\n";
							break;
						default:
							sMessage =
								GETSTRING_DLG
								(PleaseEnterYourPinOnThePinpadReader);
							if (wcslen(csReader)
							     != 0)
								
							{
								sMessage +=
									L" \"";
								sMessage +=
									csReader;
								sMessage +=
									L"\"";
							}
							sMessage += L"\n";
							break;
					}
					break;
				case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
					sMessage =
						GETSTRING_DLG
						(PleaseEnterYourPukOnThePinpadReader);
					if (wcslen(csReader) != 0)
						
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					sMessage += L", ";
					sMessage = GETSTRING_DLG(ToUnblock);
					sMessage += L" ";
					sMessage += GETSTRING_DLG(Your);
					sMessage += L" \"";
					if (wcslen(csPinName) != 0)
						sMessage += csPinName;
					
					else
						sMessage +=
							GETSTRING_DLG(Pin);
					sMessage += L"\"\n";
					 break;
				case DLG_PIN_OP_CHANGE:
					sMessage =
						GETSTRING_DLG(ChangeYourPin);
					sMessage += L" \"";
					if (wcslen(csPinName) != 0)
						sMessage += csPinName;
					
					else
						sMessage +=
							GETSTRING_DLG(Pin);
					sMessage += L"\" ";
					sMessage +=
						GETSTRING_DLG(OnTheReader);
					if (wcslen(csReader) != 0)
						
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					sMessage += L"\n";
					sMessage +=
						GETSTRING_DLG
						(EnterOldNewVerify);
					sMessage += L"\n";
					break;
				case DLG_PIN_OP_UNBLOCK_CHANGE:
					sMessage =
						GETSTRING_DLG(ChangeYourPuk);
					sMessage += L" \"";
					if (wcslen(csPinName) != 0)
						sMessage += csPinName;
					
					else
						sMessage +=
							GETSTRING_DLG(Pin);
					sMessage += L"\" ";
					sMessage +=
						GETSTRING_DLG(OnTheReader);
					if (wcslen(csReader) != 0)
						
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					sMessage += L"\n";
					break;
				default:
					MWLOG(LEV_DEBUG, MOD_DLG,
					       L"  --> DlgDisplayPinpadInfo() returns DLG_BAD_PARAM");
					return DLG_BAD_PARAM;
					break;
			}
		}
		 
			//QString buf = "dlg num: " + QString().setNum( dlgPinPadInfoCollectorIndex );
			dlgPinPadInfoCollectorIndex++;
		dlgModal =
			new dlgWndPinpadInfo(dlgPinPadInfoCollectorIndex,
					     usage, operation, csReader,
					     PINName, sMessage);
		 dlgModal->show();
		dlgModal->ProcecEvent(WM_PAINT, NULL, NULL);
		 dlgPinPadInfoCollector.
			insert(TD_WNDPINPAD_PAIR
			       (dlgPinPadInfoCollectorIndex, dlgModal));
		if (pulHandle)
			*pulHandle = dlgPinPadInfoCollectorIndex;
		 
			//delete dlgModal;
			MWLOG(LEV_DEBUG, MOD_DLG,
			      L"  --> DlgDisplayPinpadInfo() returns DLG_OK");
		return DLG_OK;
	}
	catch( ...) 
	{
		if (dlgModal)
			delete dlgModal;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgDisplayPinpadInfo() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG,
	       L"  --> DlgDisplayPinpadInfo() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 DLGS_EXPORT void eIDMW::DlgClosePinpadInfo(unsigned long ulHandle) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgClosePinpadInfo() called");
	TD_WNDPINPAD_MAP::iterator it_WndPinpad_Map =
		dlgPinPadInfoCollector.find(ulHandle);
	if (it_WndPinpad_Map != dlgPinPadInfoCollector.end())
		dlgModal = (*it_WndPinpad_Map).second;
	
	else
		dlgModal = NULL;
	 if (dlgModal)
		
	{
		delete dlgModal;
		dlgModal = NULL;
	}
	dlgPinPadInfoCollector.erase(ulHandle);
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgClosePinpadInfo() returns");
}

 DLGS_EXPORT DlgRet eIDMW::DlgDisplayModal(DlgIcon icon,
					     DlgMessageID messageID,
					     const wchar_t * csMesg,
					     unsigned char ulButtons,
					     unsigned char ulEnterButton,
					     unsigned char ulCancelButton) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgDisplayModal() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	dlgWndModal * dlg = NULL;
	try 
	{
		std::wstring csTitle = L"";;
		std::wstring csMessage;
		if (wcslen(csMesg) == 0)
			csMessage = CLang::GetMessageFromID(messageID);
		
		else
			csMessage = csMesg;
		 if (messageID == DLG_MESSAGE_USER_WARNING)
			
		{
			csTitle += GETSTRING_DLG(ReadIdentityData);
		}
		
		else
			
		{
			csTitle += GETSTRING_DLG(Message);
		}
		 dlg = new dlgWndModal(icon, csMessage, csTitle, ulButtons,
					 ulEnterButton, ulCancelButton);
		dlg->exec();
		eIDMW::DlgRet dlgResult = dlg->dlgResult;
		
			//csPin = dlg->PinResult;
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgDisplayModal() returns %ld", dlgResult);
		return dlgResult;
	}
	catch( ...) 
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgDisplayModal() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG,
	       L"  --> DlgDisplayModal() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 DLGS_EXPORT DlgRet eIDMW::DlgAskAccess(const wchar_t * csAppPath,
					  const wchar_t * csReaderName,
					  DlgPFOperation ulOperation,
					  int *piForAllOperations) 
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskAccess() called");
	 CLang::ResetInit();	// Reset language to take into account last change
	dlgWndAskAccess * dlg = NULL;
	try 
	{
		dlg = new dlgWndAskAccess(csAppPath, csReaderName,
					   ulOperation);
		dlg->exec();
		eIDMW::DlgRet dlgResult = dlg->dlgResult;
		
			//csPin = dlg->PinResult;
			if (dlg->ForAllIsChecked())
			*piForAllOperations = 1;
		
		else
			*piForAllOperations = 0;
		 delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgAskAccess() returns %ld", dlgResult);
		return dlgResult;
	}
	catch( ...) 
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG,
		       L"  --> DlgAskAccess() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG,
	       L"  --> DlgAskAccess() returns DLG_CANCEL");
	return DLG_CANCEL;
}

 
