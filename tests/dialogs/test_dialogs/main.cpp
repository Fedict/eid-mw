//
//  main.c
//  test_dialogs
//
//  Created by Frederik on 29/07/14.
//  Copyright (c) 2014 FedICT. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>

#include "mwexception.h"
#include "eiderrors.h"
#include <dialogs.h>
#include "language.h"
#include "langutil.h"

using namespace eIDMW;

void DisplayButtonPressed(DlgRet dlgret);
void DisplayPINPressed(DlgRet dlgret, wchar_t * wsPin, unsigned long ulPinBufferLen);

void DisplayAllModalCombinations();
void DisplayAllAskPinCombinations();
void DisplayChangePinCombinations();
void DisplayAllPinpadInfoCombinations();
void DisplayAllBadPinCombinations();

//void DisplayAllChangePinCombinations();

int main(int argc, char **argv, char **envp, char **apple)
{
	tLanguage lang[] = { LANG_EN, LANG_NL, LANG_FR, LANG_DE };
	unsigned long counter = 0;
	size_t langLen = sizeof(lang) / sizeof(tLanguage);

	for (; counter < langLen; counter++)
	{
		CLang::SetLang(lang[counter]);

		DisplayAllModalCombinations();
		DisplayAllAskPinCombinations();
        DisplayChangePinCombinations();
		DisplayAllPinpadInfoCombinations();
		DisplayAllBadPinCombinations();
	}

	CFShow(CFSTR("The end has been reached\n"));
	return 0;
}

struct ModalDialogCombo
{
	DlgIcon icon;
	DlgMessageID messageID;
	unsigned char ulButtons;
	unsigned char ulEnterButton;
	unsigned char ulCancelButton;
};

void DisplayAllModalCombinations()
{
	//  DLG_ICON_NONE,DLG_ICON_INFO,DLG_ICON_WARN,DLG_ICON_ERROR,DLG_ICON_QUESTION,
	//DLG_MESSAGE_NONE, DLG_MESSAGE_TESTCARD, DLG_MESSAGE_ENTER_CORRECT_CARD, DLG_MESSAGE_USER_WARNING, DLG_MESSAGE_SDK35_WARNING,

	ModalDialogCombo allModalDialogCombos[] = {
		{DLG_ICON_WARN, DLG_MESSAGE_SDK35_WARNING, DLG_BUTTON_OK, DLG_BUTTON_OK, DLG_BUTTON_NO},	//OLD SDK
		{DLG_ICON_WARN, DLG_MESSAGE_USER_WARNING, DLG_BUTTON_ALWAYS | DLG_BUTTON_YES | DLG_BUTTON_NO, DLG_BUTTON_NO, DLG_BUTTON_YES},	//PRIVACY POPUP
		{DLG_ICON_ERROR, DLG_MESSAGE_ENTER_CORRECT_CARD,
		 DLG_BUTTON_OK, DLG_BUTTON_OK, DLG_BUTTON_NO}
	};			//WRONG CARD

	DlgRet dlgret;

	unsigned long ulNrOfCombos =
		sizeof(allModalDialogCombos) / sizeof(ModalDialogCombo);

	unsigned char counter;

	for (counter = 0; counter < ulNrOfCombos; counter++)
	{
		dlgret = DlgDisplayModal(allModalDialogCombos[counter].icon,
					 allModalDialogCombos[counter].
					 messageID, L"",
					 allModalDialogCombos[counter].
					 ulButtons,
					 allModalDialogCombos[counter].
					 ulEnterButton,
					 allModalDialogCombos[counter].
					 ulCancelButton);
		DisplayButtonPressed(dlgret);
	}
}

struct VerifyPinDialogCombo
{
	DlgPinUsage usage;
	const wchar_t *wsPinName;
	const wchar_t *wsReader;
	const wchar_t *wsMessage;
	unsigned long ulRemainingTries;
	DlgPinOperation operation;	//        DLG_PIN_OP_VERIFY,DLG_PIN_OP_CHANGE,DLG_PIN_OP_UNBLOCK_NO_CHANGE,DLG_PIN_OP_UNBLOCK_CHANGE,
};

void DisplayAllAskPinCombinations()
{
	DlgPinInfo pinInfo;

	pinInfo.ulFlags = 0;
	pinInfo.ulMinLen = 4;
	pinInfo.ulMaxLen = 12;
	wchar_t wsPin[12];
	unsigned long ulPinBufferLen = 12;

	VerifyPinDialogCombo allAskPinCombos[] = {
		{DLG_PIN_SIGN, L"PIN", NULL, NULL, 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", NULL, NULL, 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_SIGN, L"PIN", NULL, NULL, 0, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", NULL, NULL, 0, DLG_PIN_OP_VERIFY}
	};

	DlgRet dlgret;

	unsigned long ulNrOfCombos =
		sizeof(allAskPinCombos) / sizeof(VerifyPinDialogCombo);

	unsigned char counter;

	for (counter = 0; counter < ulNrOfCombos; counter++)
	{
		dlgret = DlgAskPin(allAskPinCombos[counter].operation,
				   allAskPinCombos[counter].usage,
				   allAskPinCombos[counter].wsPinName,
				   pinInfo, wsPin, ulPinBufferLen);
		DisplayButtonPressed(dlgret);
		DisplayPINPressed(dlgret, wsPin, ulPinBufferLen);
	}
}

void DisplayChangePinCombinations()
{
    DlgPinInfo pinInfo;
    
    pinInfo.ulFlags = 0;
    pinInfo.ulMinLen = 4;
    pinInfo.ulMaxLen = 12;
    wchar_t wsPin1[12];
    unsigned long ulPin1BufferLen = 12;
    wchar_t wsPin2[12];
    unsigned long ulPin2BufferLen = 12;
    
    VerifyPinDialogCombo allAskPinCombos[] = {
        {DLG_PIN_SIGN, L"PIN", NULL, NULL, 3, DLG_PIN_OP_CHANGE}
    };
    
    DlgRet dlgret;
    
    unsigned long ulNrOfCombos =
    sizeof(allAskPinCombos) / sizeof(VerifyPinDialogCombo);
    
    unsigned char counter;
    
    for (counter = 0; counter < ulNrOfCombos; counter++)
    {
        dlgret = DlgAskPins(allAskPinCombos[counter].operation,
                           allAskPinCombos[counter].usage,
                           allAskPinCombos[counter].wsPinName,
                           pinInfo, wsPin1, ulPin1BufferLen,
                           pinInfo, wsPin2, ulPin2BufferLen);
        DisplayButtonPressed(dlgret);
        DisplayPINPressed(dlgret, wsPin2, ulPin2BufferLen);
    }
}

void DisplayAllPinpadInfoCombinations()
{
	VerifyPinDialogCombo allPinpadInfoCombos[] = {
		{DLG_PIN_SIGN, L"PIN", L"SmartCardReaderX",
		 L"The message given", 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", L"SmartCardReaderX",
		 L"The message given", 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_SIGN, L"PIN", L"SmartCardReaderX",
		 L"The message given", 0, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", L"SmartCardReaderX",
		 L"The message given", 0, DLG_PIN_OP_VERIFY}
	};

	DlgRet dlgret;

	unsigned long ulNrOfCombos =
		sizeof(allPinpadInfoCombos) / sizeof(VerifyPinDialogCombo);

	unsigned char counter;

	for (counter = 0; counter < ulNrOfCombos; counter++)
	{
		dlgret = DlgDisplayPinpadInfo(allPinpadInfoCombos[counter].
					      operation,
					      allPinpadInfoCombos[counter].
					      wsReader,
					      allPinpadInfoCombos[counter].
					      usage,
					      allPinpadInfoCombos[counter].
					      wsPinName,
					      allPinpadInfoCombos[counter].
					      wsMessage, NULL);
		DisplayButtonPressed(dlgret);
	}
}

void DisplayAllBadPinCombinations()
{
	VerifyPinDialogCombo allBadPinCombos[] = {
		{DLG_PIN_SIGN, L"PIN", NULL, NULL, 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", NULL, NULL, 3, DLG_PIN_OP_VERIFY},
		{DLG_PIN_SIGN, L"PIN", NULL, NULL, 0, DLG_PIN_OP_VERIFY},
		{DLG_PIN_AUTH, L"PIN", NULL, NULL, 0, DLG_PIN_OP_VERIFY}
	};

	DlgRet dlgret;

	unsigned long ulNrOfCombos =
		sizeof(allBadPinCombos) / sizeof(VerifyPinDialogCombo);

	unsigned char counter;

	for (counter = 0; counter < ulNrOfCombos; counter++)
	{
		dlgret = DlgBadPin(allBadPinCombos[counter].usage,
				   allBadPinCombos[counter].wsPinName,
				   allBadPinCombos[counter].ulRemainingTries);
		DisplayButtonPressed(dlgret);
	}
}

void DisplayPINPressed(DlgRet dlgret, wchar_t * wsPin,
		       unsigned long ulPinBufferLen)
{
	CFIndex numChars = ulPinBufferLen;
	CFStringRef strPin =
		CFStringCreateWithCharacters(kCFAllocatorDefault,
					     (const UniChar *) wsPin,
					     numChars);
	switch (dlgret)
	{
		case DLG_OK:
		case DLG_YES:
			CFShow(strPin);
			break;
		default:
			CFShow(CFSTR("check dialog return value\n"));
			break;
	}
}

void DisplayButtonPressed(DlgRet dlgret)
{
	switch (dlgret)
	{
		case eIDMW::DLG_OK:
			CFShow(CFSTR("DLG_OK pressed\n"));
			break;
		case eIDMW::DLG_CANCEL:
			CFShow(CFSTR("DLG_CANCEL pressed\n"));
			break;
		case eIDMW::DLG_RETRY:
			CFShow(CFSTR("DLG_RETRY pressed\n"));
			break;
		case eIDMW::DLG_YES:
			CFShow(CFSTR("DLG_YES pressed\n"));
			break;
		case eIDMW::DLG_NO:
			CFShow(CFSTR("DLG_NO pressed\n"));
			break;
		case eIDMW::DLG_ALWAYS:
			CFShow(CFSTR("DLG_ALWAYS pressed\n"));
			break;
		case eIDMW::DLG_NEVER:
			CFShow(CFSTR("DLG_NEVER pressed\n"));
			break;
		case eIDMW::DLG_BAD_PARAM:
			CFShow(CFSTR("DLG_BAD_PARAM returned\n"));
			break;
		case eIDMW::DLG_ERR:
			CFShow(CFSTR("DLG_ERR returned\n"));
			break;
		default:
			break;
	}
}
