//
//  main.c
//  test_dialogs
//
//  Created by Frederik on 29/07/14.
//  Copyright (c) 2014 FedICT. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>

#include "mwexception.h"
#include "eidErrors.h"
#include <dialogs.h>
#include "language.h"
#include "langutil.h"

void DisplayButtonPressed(eIDMW::DlgRet dlgret);

int main(int argc, char **argv, char **envp, char **apple)
{
    eIDMW::tLanguage lang = eIDMW::LANG_NL;
    
    eIDMW::CLang::SetLang(lang); //  LANG_EN=0,LANG_NL=1,LANG_FR=2,LANG_DE=3
    //DLG_PIN_AUTH
    eIDMW::DlgPinUsage usage = eIDMW::DLG_PIN_SIGN;//eIDMW::DLG_PIN_AUTH;//
    const wchar_t *wsPinName = L"PIN";
    const wchar_t *wsReader = L"SmartCardReaderX";
    const wchar_t *wsMessage = L"The message given";
    unsigned long ulRemainingTries = 3;
    eIDMW::DlgPinOperation operation =  eIDMW::DLG_PIN_OP_VERIFY;//	DLG_PIN_OP_VERIFY,DLG_PIN_OP_CHANGE,DLG_PIN_OP_UNBLOCK_NO_CHANGE,DLG_PIN_OP_UNBLOCK_CHANGE,
    eIDMW::DlgPinInfo pinInfo;
    pinInfo.ulFlags = 0;
    pinInfo.ulMinLen = 4;
    pinInfo.ulMaxLen = 12;
    wchar_t wsPin[12];
    unsigned long ulPinBufferLen = 12;
    //	DLG_ICON_NONE,DLG_ICON_INFO,DLG_ICON_WARN,DLG_ICON_ERROR,DLG_ICON_QUESTION,
    eIDMW::DlgIcon icon = eIDMW::DLG_ICON_WARN;
    eIDMW::DlgMessageID messageID = eIDMW::DLG_MESSAGE_SDK35_WARNING;//DLG_MESSAGE_NONE, DLG_MESSAGE_TESTCARD, DLG_MESSAGE_ENTER_CORRECT_CARD, DLG_MESSAGE_USER_WARNING, DLG_MESSAGE_SDK35_WARNING,
    
    unsigned char ulButtons = eIDMW::DLG_BUTTON_ALWAYS | eIDMW::DLG_BUTTON_YES | eIDMW::DLG_BUTTON_NO;
    
    
    eIDMW::DlgRet dlgret = eIDMW::DlgDisplayPinpadInfo( operation, wsReader, usage, wsPinName, wsMessage, NULL);
    DisplayButtonPressed(dlgret);
    
    //dlgret =  eIDMW::DlgDisplayModal(icon,messageID, L"Message to be displayed", ulButtons, '0','0');
    //DisplayButtonPressed(dlgret);
    
    dlgret =  eIDMW::DlgAskPin(operation,usage, wsPinName,pinInfo, wsPin, ulPinBufferLen);
    DisplayButtonPressed(dlgret);
    
    dlgret = eIDMW::DlgBadPin(usage, wsPinName, ulRemainingTries);
    DisplayButtonPressed(dlgret);
    

    // insert code here...
    CFShow(CFSTR("Hello, World!\n"));
    return 0;
}

void DisplayButtonPressed(eIDMW::DlgRet dlgret)
{
    switch (dlgret) {
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
