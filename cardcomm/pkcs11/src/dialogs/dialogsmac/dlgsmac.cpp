/* ****************************************************************************
 
 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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

#include <stdlib.h>
#include <signal.h>
#include "errno.h"

#include "../dialogs.h"
#include "../langutil.h"

#include <map>

#include "util.h"
#include "mwexception.h"
#include "eiderrors.h"
#include "configuration.h"

#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFUserNotification.h>
#include <CoreFoundation/CFBundle.h>

using namespace eIDMW;

#define THROW_ERROR_IF_NULL(a) if(a==NULL){throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);}
#define THROW_ERROR_IF_NON_ZERO(a) if(a!=0){throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);}

/************************
 *       DIALOGS
 ************************/
#define BEID_MAX_MESSAGE_ARRAY_LEN 8

typedef std::map<unsigned long, CFUserNotificationRef> TD_MCPINPAD_MAP;

TD_MCPINPAD_MAP mc_pinpad_map;
unsigned long mc_pinpad_map_index = 0;

CFMutableStringRef CreateStringFromWChar(const wchar_t * wcsMessage)
{
    if(wcsMessage == NULL)
        return NULL;
    
    CFStringRef utf8String = CFStringCreateWithBytes (
                                                     kCFAllocatorDefault,
                                                     (const UInt8 *)wcsMessage,
                                                     wcslen(wcsMessage)*sizeof(wchar_t)/sizeof(UInt8),
                                                     kCFStringEncodingUTF32LE,
                                                     false
                                                     );
    THROW_ERROR_IF_NULL(utf8String);

    CFMutableStringRef toBeStrippedString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, utf8String);
    CFStringFindAndReplace(toBeStrippedString, CFSTR("&"), CFSTR(""), CFRangeMake(0, CFStringGetLength(toBeStrippedString)), NULL);
    CFRelease(utf8String);

    return toBeStrippedString;
}

void AppendToStringFromWChar(CFMutableStringRef mutableString, const wchar_t * wcsMessage)
{
    if(wcsMessage != NULL)
    {
        CFMutableStringRef str = CreateStringFromWChar(wcsMessage);
        CFStringAppend(mutableString, str);
        CFRelease(str);
    }
}

void AppendButtonToArrays (unsigned char ulButtons, unsigned char ulButtonToAdd,
                           CFMutableArrayRef mutArrayKeys,CFMutableArrayRef mutArrayValues,
                           CFStringRef buttonTitleKey)
{
    if(ulButtonToAdd & ulButtons)
    {
        CFArrayAppendValue(mutArrayKeys, buttonTitleKey);
        //DLG_BUTTON_ALWAYS | DLG_BUTTON_YES | DLG_BUTTON_NO
        switch (ulButtonToAdd) {
            case DLG_BUTTON_YES:
                CFArrayAppendValue(mutArrayValues, CreateStringFromWChar(GETSTRING_DLG(Yes)) );
                break;
            case DLG_BUTTON_NO:
                CFArrayAppendValue(mutArrayValues, CreateStringFromWChar(GETSTRING_DLG(No)) );
                break;
            default:
                CFArrayAppendValue(mutArrayValues, CreateStringFromWChar(GETSTRING_DLG(Ok)) );
                break;
        }
    }
}

//keep in mind that you need to release the return value of this function
CFDictionaryRef BeidCreateDictFromArrays(const void**keys, const void** values, CFMutableArrayRef mutArrayKeys, CFMutableArrayRef mutArrayValues)
{
    CFRange range;
    range.length = CFArrayGetCount(mutArrayValues);
    range.location = 0;
    CFArrayGetValues (mutArrayValues, range,(const void**)values);
    
    range.length = CFArrayGetCount(mutArrayKeys);
    range.location = 0;
    CFArrayGetValues (mutArrayKeys, range,(const void**)keys);
    
    CFDictionaryRef dictRef = CFDictionaryCreate(0, keys, values, CFArrayGetCount(mutArrayKeys), NULL, NULL);
    
    THROW_ERROR_IF_NULL(dictRef);
    
    return dictRef;
}

DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
                                    DlgPinUsage usage, const wchar_t *wsPinName,
                                    DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen)
{
    DlgRet lRet = DLG_CANCEL;
    
	std::string csReadableFilePath;
    const wchar_t * wcsTitle = NULL;
	CFUserNotificationRef userNotificationRef = NULL;
	SInt32 error = 0;
    
    CFOptionFlags optionFlags = CFUserNotificationSecureTextField(0);
    CFOptionFlags responseFlags = 0;
	
    void* keys[BEID_MAX_MESSAGE_ARRAY_LEN];   //to store the keys
    void* values[BEID_MAX_MESSAGE_ARRAY_LEN]; //to store the values
    CFMutableArrayRef mutArrayKeys = NULL; //to create the array of keys
    CFMutableArrayRef mutArrayValues = NULL; //to create the array of values
    
    CFStringRef IconURLString = NULL;
    CFURLRef urlRef = NULL;
    CFStringRef headerString = NULL;
    CFMutableStringRef titleString = NULL;
    CFStringRef messageString = NULL;
    CFDictionaryRef parameters = NULL;
    CFStringRef defaultButtonString = NULL;
    CFStringRef alternateButtonString = NULL;
    
    try {
        
        //create header text
        headerString = CreateStringFromWChar(GETSTRING_DLG(EnterYourPin));
        //create default button text
        defaultButtonString = CreateStringFromWChar(GETSTRING_DLG(Ok));
        //create alternate (cancel) button text
        alternateButtonString = CreateStringFromWChar(GETSTRING_DLG(Cancel));
        //create message text
        messageString = CreateStringFromWChar(GETSTRING_DLG(Pin));
        
        mutArrayKeys = CFArrayCreateMutable (kCFAllocatorDefault,
                                             BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                             NULL//const CFArrayCallBacks *callBacks
                                             );
        mutArrayValues = CFArrayCreateMutable (kCFAllocatorDefault,
                                               BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                               NULL//const CFArrayCallBacks *callBacks
                                               );
        
        //CFStringRef bundleID = CFSTR("beid.pkcs11");
        //CFBundleRef bundle = CFBundleGetBundleWithIdentifier(bundleID);
        
        if( usage == DLG_PIN_SIGN )
        {
            optionFlags |= kCFUserNotificationCautionAlertLevel;
            wcsTitle=GETSTRING_DLG(SigningWith);
            IconURLString = CreateStringFromWChar(L"/usr/local/lib/beid-pkcs11.bundle/Contents/Resources/ICO_CARD_DIGSIG_128x128.png");
            //urlRef = CFBundleCopyResourceURL(bundle, CFSTR("ICO_CARD_DIGSIG_128x128"), CFSTR("png"), NULL);
        }
        else
        {
            optionFlags |= kCFUserNotificationPlainAlertLevel;
            wcsTitle=GETSTRING_DLG(Asking);
            IconURLString = CreateStringFromWChar(L"/usr/local/lib/beid-pkcs11.bundle/Contents/Resources/ICO_CARD_PIN_128x128.png");
            //urlRef = CFBundleCopyResourceURL(bundle, CFSTR("ICO_CARD_PIN_128x128"), CFSTR("png"), NULL);
        }
        urlRef = CFURLCreateWithString ( kCFAllocatorDefault, IconURLString, NULL );
        
        //need room for space and pin name
        if(wcslen(wcsTitle) >= 250)
            return DLG_ERR;

        titleString = CFStringCreateMutable(kCFAllocatorDefault, 0);
        AppendToStringFromWChar(titleString, wcsTitle);
        AppendToStringFromWChar(titleString, L" ");
        AppendToStringFromWChar(titleString, wsPinName);

        //always display header
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertHeaderKey);
        CFArrayAppendValue(mutArrayValues, headerString);
        //always display tittle
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertMessageKey);
        CFArrayAppendValue(mutArrayValues, titleString);
        //always display textbox tittle
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationTextFieldTitlesKey);
        CFArrayAppendValue(mutArrayValues, messageString);
        //always display default button text
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationDefaultButtonTitleKey);
        CFArrayAppendValue(mutArrayValues, defaultButtonString);
        //always display default button text
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlternateButtonTitleKey);
        CFArrayAppendValue(mutArrayValues, alternateButtonString);
        //add url if it exists
        if(urlRef != NULL)
        {
            //add the image as icon
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationIconURLKey);
            CFArrayAppendValue(mutArrayValues, urlRef);
        }
        
        
        parameters = BeidCreateDictFromArrays((const void**)keys, (const void**)values, mutArrayKeys, mutArrayValues);
        
        userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
                                                        30, //CFTimeInterval timeout,
                                                        optionFlags,//CFOptionFlags flags,
                                                        &error,//SInt32 *error,
                                                        parameters);//CFDictionaryRef dictionary
        
        THROW_ERROR_IF_NULL(userNotificationRef);
        THROW_ERROR_IF_NON_ZERO(error);
        
        error = CFUserNotificationReceiveResponse (
                                                   userNotificationRef,//CFUserNotificationRef userNotification,
                                                   0,//CFTimeInterval timeout,
                                                   &responseFlags//CFOptionFlags responseFlags
                                                   );
        CFStringRef PinValue = NULL;
        CFIndex length;
        
        switch (responseFlags & 0x03)
        {
            case kCFUserNotificationDefaultResponse:
                lRet = DLG_OK;
                //get the PIN
                PinValue = CFUserNotificationGetResponseValue ( userNotificationRef, kCFUserNotificationTextFieldValuesKey, 0 );
                
                if ((length = CFStringGetLength(PinValue)) >= ulPinBufferLen) {
                    //PIN entered is too long, we'll return an error (need 1 char for string termination)
                    lRet = DLG_ERR;
                } else {
                    char *chars = (char*)malloc(length * 2);
                    CFStringGetCString(PinValue, chars, length * 2, kCFStringEncodingUTF8);
                    mbstowcs(wsPin, chars, ulPinBufferLen);
                    free(chars);
                }
                break;
            case kCFUserNotificationAlternateResponse:
                lRet = DLG_CANCEL;
                break;
            case kCFUserNotificationOtherResponse:
                lRet = DLG_RETRY;
                break;
            case kCFUserNotificationCancelResponse:
                lRet = DLG_CANCEL;
                break;
            default:
                lRet = DLG_CANCEL;
        }
        CFRelease(userNotificationRef);
        userNotificationRef = NULL;
    } catch (...) {
        
        lRet = DLG_ERR;
    }
    
    //cleanup
    if (defaultButtonString != NULL)
        CFRelease(defaultButtonString);
    if (alternateButtonString != NULL)
        CFRelease(alternateButtonString);
    if (userNotificationRef != NULL) {
        CFUserNotificationCancel(userNotificationRef);
        CFRelease(userNotificationRef);
    }
    if (urlRef != NULL)
        CFRelease(urlRef);
    if (IconURLString != NULL)
        CFRelease(IconURLString);
    if (userNotificationRef != NULL)
        CFRelease(userNotificationRef);
    if (parameters != NULL)
        CFRelease(parameters);
    if (messageString != NULL)
        CFRelease(messageString);
    if (titleString != NULL)
        CFRelease(titleString);
    if (headerString != NULL)
        CFRelease(headerString);
    
    return lRet;
    
}

DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
                                     DlgPinUsage usage, const wchar_t *wsPinName,
                                     DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen,
                                     DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen)
{
    
    DlgRet lRet = DLG_CANCEL;
    
    std::string csReadableFilePath;
    //const wchar_t * wcsTitle = NULL;
    CFUserNotificationRef userNotificationRef = NULL;
    SInt32 error = 0;
    
    CFOptionFlags optionFlags = CFUserNotificationSecureTextField(0);
    optionFlags |= CFUserNotificationSecureTextField(1);
    optionFlags |= CFUserNotificationSecureTextField(2);
    CFOptionFlags responseFlags = 0;
    
    void* keys[BEID_MAX_MESSAGE_ARRAY_LEN];   //to store the keys
    void* values[BEID_MAX_MESSAGE_ARRAY_LEN]; //to store the values
    CFMutableArrayRef mutArrayKeys = NULL; //to create the array of keys
    CFMutableArrayRef mutArrayValues = NULL; //to create the array of values
    
    CFStringRef IconURLString = NULL;
    CFURLRef urlRef = NULL;
    CFStringRef headerString = NULL;
    CFMutableStringRef titleString = NULL;
    CFStringRef currentPinMessageString = NULL;
    CFStringRef newPinMessageString = NULL;
    CFStringRef confNewPinMessageString = NULL;
    CFDictionaryRef parameters = NULL;
    CFStringRef defaultButtonString = NULL;
    CFStringRef alternateButtonString = NULL;
 
    try {
        
        //create title text
        titleString = CreateStringFromWChar(GETSTRING_DLG(EnterYourPin));
        //create header text
        headerString = CreateStringFromWChar(GETSTRING_DLG(RenewingPinCode));
        //create default button text
        defaultButtonString = CreateStringFromWChar(GETSTRING_DLG(Ok));
        //create alternate (cancel) button text
        alternateButtonString = CreateStringFromWChar(GETSTRING_DLG(Cancel));
        //create message text for current PIN
        currentPinMessageString = CreateStringFromWChar(GETSTRING_DLG(CurrentPin));
        //create message text for new PIN
        newPinMessageString = CreateStringFromWChar(GETSTRING_DLG(NewPin));
        //create message text for new PIN confirmation
        confNewPinMessageString = CreateStringFromWChar(GETSTRING_DLG(ConfirmNewPin));
        
        mutArrayKeys = CFArrayCreateMutable (kCFAllocatorDefault,
                                             BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                             NULL//const CFArrayCallBacks *callBacks
                                             );
        mutArrayValues = CFArrayCreateMutable (kCFAllocatorDefault,
                                               BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                               NULL//const CFArrayCallBacks *callBacks
                                               );
        
        CFTypeRef PinMessages[3] = { currentPinMessageString, newPinMessageString, confNewPinMessageString};
        CFArrayRef arrayPinMessages = CFArrayCreate( NULL, PinMessages, 3, &kCFTypeArrayCallBacks );
        
        optionFlags |= kCFUserNotificationCautionAlertLevel;
        IconURLString = CreateStringFromWChar(L"/usr/local/lib/beid-pkcs11.bundle/Contents/Resources/ICO_CARD_DIGSIG_128x128.png");

        urlRef = CFURLCreateWithString ( kCFAllocatorDefault, IconURLString, NULL );
        
        //always display header
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertHeaderKey);
        CFArrayAppendValue(mutArrayValues, headerString);
        //always display tittle
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertMessageKey);
        CFArrayAppendValue(mutArrayValues, titleString);
        //always display textbox tittles
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationTextFieldTitlesKey);
        CFArrayAppendValue(mutArrayValues, arrayPinMessages);
        //always display default button text
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationDefaultButtonTitleKey);
        CFArrayAppendValue(mutArrayValues, defaultButtonString);
        //always display default button text
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlternateButtonTitleKey);
        CFArrayAppendValue(mutArrayValues, alternateButtonString);
        //add url if it exists
        if(urlRef != NULL)
        {
            //add the image as icon
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationIconURLKey);
            CFArrayAppendValue(mutArrayValues, urlRef);
        }
        
        
        parameters = BeidCreateDictFromArrays((const void**)keys, (const void**)values, mutArrayKeys, mutArrayValues);
        
        userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
                                                        30, //CFTimeInterval timeout,
                                                        optionFlags,//CFOptionFlags flags,
                                                        &error,//SInt32 *error,
                                                        parameters);//CFDictionaryRef dictionary
        
        THROW_ERROR_IF_NULL(userNotificationRef);
        THROW_ERROR_IF_NON_ZERO(error);
        
        error = CFUserNotificationReceiveResponse (
                                                   userNotificationRef,//CFUserNotificationRef userNotification,
                                                   0,//CFTimeInterval timeout,
                                                   &responseFlags//CFOptionFlags responseFlags
                                                   );
        CFStringRef currentPinValue = NULL;
        CFStringRef newPinValue = NULL;
        CFStringRef confNewPinValue = NULL;
        CFIndex length;
        CFComparisonResult compResult = kCFCompareEqualTo;
        
        switch (responseFlags & 0x03)
        {
            case kCFUserNotificationDefaultResponse:
                lRet = DLG_OK;
                //get the PINs
                currentPinValue = CFUserNotificationGetResponseValue ( userNotificationRef, kCFUserNotificationTextFieldValuesKey, 0 );
                newPinValue = CFUserNotificationGetResponseValue ( userNotificationRef, kCFUserNotificationTextFieldValuesKey, 1 );
                confNewPinValue = CFUserNotificationGetResponseValue ( userNotificationRef, kCFUserNotificationTextFieldValuesKey, 2 );
                
                compResult = CFStringCompare(newPinValue, confNewPinValue, 0);
                
                if (compResult != kCFCompareEqualTo)
                {
                    lRet = DLG_BAD_PARAM;
                }
                
                //convert current PIN to char array
                if ((length = CFStringGetLength(currentPinValue)) >= ulPin1BufferLen) {
                    //PIN entered is too long, we'll return an error (need 1 char for string termination)
                    lRet = DLG_ERR;
                } else {
                    char *chars = (char*)malloc(length * 2);
                    CFStringGetCString(currentPinValue, chars, length * 2, kCFStringEncodingUTF8);
                    mbstowcs(wsPin1, chars, ulPin1BufferLen);
                    free(chars);
                }
                
                //convert new PIN to char array
                if ((length = CFStringGetLength(newPinValue)) >= ulPin2BufferLen) {
                    //PIN entered is too long, we'll return an error (need 1 char for string termination)
                    lRet = DLG_ERR;
                } else {
                    char *chars = (char*)malloc(length * 2);
                    CFStringGetCString(newPinValue, chars, length * 2, kCFStringEncodingUTF8);
                    mbstowcs(wsPin2, chars, ulPin2BufferLen);
                    free(chars);
                }
                
                break;
            case kCFUserNotificationAlternateResponse:
                lRet = DLG_CANCEL;
                break;
            case kCFUserNotificationOtherResponse:
                lRet = DLG_RETRY;
                break;
            case kCFUserNotificationCancelResponse:
                lRet = DLG_CANCEL;
                break;
            default:
                lRet = DLG_CANCEL;
        }
        CFRelease(userNotificationRef);
        userNotificationRef = NULL;
    } catch (...) {
        
        lRet = DLG_ERR;
    }
    
    //cleanup
    if (defaultButtonString != NULL)
        CFRelease(defaultButtonString);
    if (alternateButtonString != NULL)
        CFRelease(alternateButtonString);
    if (userNotificationRef != NULL) {
        CFUserNotificationCancel(userNotificationRef);
        CFRelease(userNotificationRef);
    }
    if (urlRef != NULL)
        CFRelease(urlRef);
    if (IconURLString != NULL)
        CFRelease(IconURLString);
    if (userNotificationRef != NULL)
        CFRelease(userNotificationRef);
    if (parameters != NULL)
        CFRelease(parameters);
    if (currentPinMessageString != NULL)
        CFRelease(currentPinMessageString);
    if (newPinMessageString != NULL)
        CFRelease(newPinMessageString);
    if (confNewPinMessageString != NULL)
        CFRelease(confNewPinMessageString);
    if (titleString != NULL)
        CFRelease(titleString);
    if (headerString != NULL)
        CFRelease(headerString);
    if (currentPinMessageString != NULL)
        CFRelease(currentPinMessageString);
    if (newPinMessageString != NULL)
        CFRelease(newPinMessageString);
    if (confNewPinMessageString != NULL)
        CFRelease(confNewPinMessageString);
    
    return lRet;
}


DlgRet eIDMW::DlgBadPin(
                                    DlgPinUsage usage, const wchar_t *wsPinName,
                                    unsigned long ulRemainingTries)
{
    DlgRet lRet = DLG_CANCEL;
    
    CFMutableStringRef headerString = NULL;
    CFMutableStringRef titleString = NULL;
    CFStringRef defButtonString = NULL;
    CFStringRef altButtonString = NULL;
    
	std::string csReadableFilePath;
	CFUserNotificationRef userNotificationRef = NULL;
	SInt32 error = 0;
	CFOptionFlags optionFlags;
    CFOptionFlags responseFlags;
    
    optionFlags = kCFUserNotificationCautionAlertLevel;
    CFMutableArrayRef mutArrayKeys = NULL;
    CFMutableArrayRef mutArrayValues = NULL;
    CFDictionaryRef parameters = NULL;
    
    void* keys[BEID_MAX_MESSAGE_ARRAY_LEN];
    void* values[BEID_MAX_MESSAGE_ARRAY_LEN];
    
    try {
        
        mutArrayKeys = CFArrayCreateMutable (kCFAllocatorDefault,
                                             BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                             NULL//const CFArrayCallBacks *callBacks
                                             );
        mutArrayValues = CFArrayCreateMutable (kCFAllocatorDefault,
                                               BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                               NULL//const CFArrayCallBacks *callBacks
                                               );
        
        //create header string
        headerString = CFStringCreateMutable(kCFAllocatorDefault, 0);
        AppendToStringFromWChar(headerString, GETSTRING_DLG(Notification));
        CFStringAppend(headerString, CFSTR(": "));
        AppendToStringFromWChar(headerString, GETSTRING_DLG(Bad));
        CFStringAppend(headerString, CFSTR(" "));
        AppendToStringFromWChar(headerString, wsPinName);
        
        //create title string
        titleString = CFStringCreateMutable(kCFAllocatorDefault, 0);
        AppendToStringFromWChar(titleString, GETSTRING_DLG(Bad));
        CFStringAppend(titleString, CFSTR(" "));
        AppendToStringFromWChar(titleString, wsPinName);
        CFStringAppend(titleString, CFSTR(": "));
        
        CFDictionaryRef formatOptions = NULL;
        CFStringAppendFormat(titleString, formatOptions, CFSTR("%lu"), ulRemainingTries);
        CFStringAppend(titleString, CFSTR(" "));
        AppendToStringFromWChar(titleString, GETSTRING_DLG(RemainingAttempts));
        CFStringAppend(titleString, CFSTR("\n\n"));
        
        if( ulRemainingTries == 0 )
        {
            AppendToStringFromWChar(titleString, wsPinName);
            CFStringAppend(titleString, CFSTR(" "));
            AppendToStringFromWChar(titleString, GETSTRING_DLG(PinBlocked));
            defButtonString = CreateStringFromWChar(GETSTRING_DLG(Ok));
            
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationDefaultButtonTitleKey);
            CFArrayAppendValue(mutArrayValues, defButtonString);
        }
        else
        {
            AppendToStringFromWChar(titleString, GETSTRING_DLG(TryAgainOrCancel));
            defButtonString = CreateStringFromWChar(GETSTRING_DLG(Cancel));
            altButtonString = CreateStringFromWChar(GETSTRING_DLG(Retry));
            
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationDefaultButtonTitleKey);
            CFArrayAppendValue(mutArrayValues, defButtonString);
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlternateButtonTitleKey);
            CFArrayAppendValue(mutArrayValues, altButtonString);
        }
        
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertHeaderKey);
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertMessageKey);
        CFArrayAppendValue(mutArrayValues, headerString);
        CFArrayAppendValue(mutArrayValues, titleString);
        
        parameters = BeidCreateDictFromArrays((const void**)keys, (const void**)values, mutArrayKeys, mutArrayValues);
        
        userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
                                                        30, //CFTimeInterval timeout,
                                                        optionFlags,//CFOptionFlags flags,
                                                        &error,//SInt32 *error,
                                                        parameters);//CFDictionaryRef dictionary
        THROW_ERROR_IF_NULL(userNotificationRef);
        
        error = CFUserNotificationReceiveResponse (
                                                   userNotificationRef,//CFUserNotificationRef userNotification,
                                                   0,//CFTimeInterval timeout,
                                                   &responseFlags//CFOptionFlags responseFlags
                                                   );
        THROW_ERROR_IF_NON_ZERO(error);
        
        switch (responseFlags & 0x03)
        {
            case kCFUserNotificationDefaultResponse:
                lRet = DLG_OK;
                break;
            case kCFUserNotificationAlternateResponse:
                lRet = DLG_CANCEL;
                break;
            case kCFUserNotificationOtherResponse:
                lRet = DLG_RETRY;
                break;
            case kCFUserNotificationCancelResponse:
                lRet = DLG_CANCEL;
                break;
            default:
                lRet = DLG_CANCEL;
        }
        
    } catch (...) {
        lRet = DLG_ERR;
    }
    if(mutArrayKeys != NULL)
        CFRelease(mutArrayKeys);
    if(mutArrayValues != NULL)
        CFRelease(mutArrayValues);
    if(userNotificationRef != NULL)
        CFRelease(userNotificationRef);
    if(parameters != NULL)
        CFRelease(parameters);
    if(headerString != NULL)
        CFRelease(headerString);
    if(titleString != NULL)
        CFRelease(titleString);
    if(defButtonString != NULL)
        CFRelease(defButtonString);
    if(altButtonString != NULL)
        CFRelease(altButtonString);
    return lRet;
    
}


//DlgDisplayModal is only used as dialog askaccess at the moment,
DlgRet eIDMW::DlgDisplayModal(DlgIcon icon,
                                          DlgMessageID messageID, const wchar_t *csMesg,
                                          unsigned char ulButtons, unsigned char ulEnterButton,
                                          unsigned char ulCancelButton)
{
    DlgRet lRet = DLG_CANCEL;
    
	std::string csReadableFilePath;
	CFUserNotificationRef userNotificationRef = NULL;
	SInt32 error = 0;
	CFOptionFlags optionFlags;
    CFOptionFlags responseFlags;
    CFStringRef tittleStrRef = NULL;
    
    void* keys[BEID_MAX_MESSAGE_ARRAY_LEN];   //to store the keys
    void* values[BEID_MAX_MESSAGE_ARRAY_LEN]; //to store the values
    CFMutableArrayRef mutArrayKeys = NULL; //to create the array of keys
    CFMutableArrayRef mutArrayValues = NULL; //to create the array of values
    
    CFRange range;
    CFDictionaryRef parameters = NULL;
    CFStringRef datacharRefBytes = NULL;
    
    try {
        
        mutArrayKeys = CFArrayCreateMutable (kCFAllocatorDefault,
                                             BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                             NULL//const CFArrayCallBacks *callBacks
                                             );
        
        mutArrayValues = CFArrayCreateMutable (kCFAllocatorDefault,
                                               BEID_MAX_MESSAGE_ARRAY_LEN,//CFIndex capacity,
                                               NULL//const CFArrayCallBacks *callBacks
                                               );
        
        switch (icon) {
            case DLG_ICON_ERROR:
                optionFlags = kCFUserNotificationStopAlertLevel;
                break;
            case DLG_ICON_WARN:
                optionFlags = kCFUserNotificationCautionAlertLevel;
                break;
            case DLG_ICON_INFO:
            case DLG_ICON_QUESTION:
                optionFlags = kCFUserNotificationNoteAlertLevel;
                break;
            case DLG_ICON_NONE:
            default:
                optionFlags = kCFUserNotificationPlainAlertLevel;
                break;
        }
        
        if( csMesg == NULL)
        {
            tittleStrRef = CreateStringFromWChar(L"");
        }
        else
        {
            tittleStrRef = CreateStringFromWChar(csMesg);
        }
        std::wstring translatedMessage(CLang::GetMessageFromID(messageID));
        datacharRefBytes = CFStringCreateWithBytes (
                                                    kCFAllocatorDefault,
                                                    (const UInt8 *)translatedMessage.c_str(),
                                                    translatedMessage.size()*sizeof(wchar_t)/sizeof(UInt8),
                                                    kCFStringEncodingUTF32LE,
                                                    false
                                                    );
        
        //always display tittle
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertHeaderKey);
        CFArrayAppendValue(mutArrayValues, tittleStrRef);
        //always display message
        CFArrayAppendValue(mutArrayKeys, kCFUserNotificationAlertMessageKey);
        CFArrayAppendValue(mutArrayValues, datacharRefBytes?:CreateStringFromWChar(L""));
        
        //check which buttons are requested to be shown
        //ulEnterButton is the one on the right, only show it when part of the button list
        AppendButtonToArrays (ulButtons, ulEnterButton, mutArrayKeys, mutArrayValues,
                              kCFUserNotificationDefaultButtonTitleKey);
        
        //ulCancelButton is the one on the left
        AppendButtonToArrays (ulButtons, ulCancelButton, mutArrayKeys, mutArrayValues,
                              kCFUserNotificationAlternateButtonTitleKey);
        
        if(ulButtons & DLG_BUTTON_ALWAYS)
        {
            CFArrayAppendValue(mutArrayKeys, kCFUserNotificationOtherButtonTitleKey);
            CFArrayAppendValue(mutArrayValues, CreateStringFromWChar(GETSTRING_DLG(Always)) );
        }
        
        range.length = CFArrayGetCount(mutArrayValues);
        range.location = 0;
        CFArrayGetValues (mutArrayValues, range,(const void**)values);
        
        range.length = CFArrayGetCount(mutArrayKeys);
        range.location = 0;
        CFArrayGetValues (mutArrayKeys, range,(const void**)keys);
        
        parameters = CFDictionaryCreate(0, (const void**)keys, (const void**)values,
                                        CFArrayGetCount(mutArrayKeys), NULL,
                                        NULL);
        
        userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
                                                        30, //CFTimeInterval timeout,
                                                        optionFlags,//CFOptionFlags flags,
                                                        &error,//SInt32 *error,
                                                        parameters);//CFDictionaryRef dictionary
        THROW_ERROR_IF_NULL(userNotificationRef);
        
        error = CFUserNotificationReceiveResponse (userNotificationRef,//CFUserNotificationRef userNotification,
                                                   0,//CFTimeInterval timeout,
                                                   &responseFlags//CFOptionFlags responseFlags
                                                   );
        THROW_ERROR_IF_NON_ZERO(error);
        
        switch (responseFlags & 0x03)
        {
            case kCFUserNotificationDefaultResponse:
                lRet = DLG_OK;
                break;
            case kCFUserNotificationAlternateResponse:
                lRet = DLG_CANCEL;
                break;
                
            case kCFUserNotificationOtherResponse:
                if (ulButtons & DLG_BUTTON_ALWAYS)
                    lRet = DLG_ALWAYS;
                else if(ulButtons & DLG_BUTTON_YES)
                    lRet = DLG_YES;
                else if(ulButtons & DLG_BUTTON_NO)
                    lRet = DLG_NO;
                else if(ulButtons & DLG_BUTTON_OK)
                    lRet = DLG_OK;
                else if(ulButtons & DLG_BUTTON_CANCEL)
                    lRet = DLG_CANCEL;
                break;
                
            case kCFUserNotificationCancelResponse:
                lRet = DLG_CANCEL;
                break;
                
            default:
                lRet = DLG_CANCEL;
        }
        
    }
    catch (...) {
        lRet = DLG_ERR;
    }
    
    if(tittleStrRef != NULL)
        CFRelease(tittleStrRef);
    if(datacharRefBytes != NULL)
        CFRelease(datacharRefBytes);
    if(mutArrayKeys != NULL)
        CFRelease(mutArrayKeys);
    if(mutArrayValues != NULL)
        CFRelease(mutArrayValues);
    if(parameters != NULL)
        CFRelease(parameters);
    if(userNotificationRef != NULL)
        CFRelease(userNotificationRef);
    
    return lRet;
}


DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
											   const wchar_t *wsReader, DlgPinUsage usage, const wchar_t *wsPinName,
											   const wchar_t *wsMessage,
											   unsigned long *pulHandle)
{
    DlgRet lRet = DLG_CANCEL;
	CFArrayRef           titlesArray = NULL;
    
	std::string csReadableFilePath;
    const wchar_t * Title;
	CFUserNotificationRef userNotificationRef = NULL;
	SInt32 error = 0;
    CFOptionFlags optionFlags;
    CFStringRef datacharRefBytes = NULL;
    CFStringRef titlecharRefBytes = NULL;
    CFDictionaryRef parameters = NULL;
	
	const void* keys[] = {kCFUserNotificationAlertHeaderKey,
		kCFUserNotificationAlertMessageKey};
	
    try {
        
        if( usage == DLG_PIN_SIGN )
        {
            optionFlags = kCFUserNotificationCautionAlertLevel;
            Title=GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);//SigningWith);
        }
        else
        {
            optionFlags = kCFUserNotificationPlainAlertLevel;
            Title=GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
        }
        
        datacharRefBytes = CFStringCreateWithBytes (
                                                    kCFAllocatorDefault,
                                                    (const UInt8 *)wsReader,
                                                    wcslen(wsReader)*sizeof(wchar_t)/sizeof(UInt8),
                                                    kCFStringEncodingUTF32LE,
                                                    false
                                                    );
        
        titlecharRefBytes = CFStringCreateWithBytes (
                                                    kCFAllocatorDefault,
                                                    (const UInt8 *)Title,
                                                    wcslen(Title)*sizeof(wchar_t)/sizeof(UInt8),
                                                    kCFStringEncodingUTF32LE,
                                                    false
                                                    );

        const void* textFieldTitles[] = {Title};
        
        titlesArray = CFArrayCreate(NULL, textFieldTitles, 1, NULL);
        
        const void* values[] = {titlecharRefBytes,
            datacharRefBytes};
        
        parameters = CFDictionaryCreate(0, keys, values,
                                        sizeof(keys)/sizeof(*keys), NULL,
                                        NULL);
        
        userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
                                                        30, //CFTimeInterval timeout,
                                                        optionFlags,//CFOptionFlags flags,
                                                        &error,//SInt32 *error,
                                                        parameters);//CFDictionaryRef dictionary
        THROW_ERROR_IF_NULL(userNotificationRef);
        
        mc_pinpad_map[mc_pinpad_map_index++] = userNotificationRef;
        if(pulHandle) {
            *pulHandle = mc_pinpad_map_index;
        }
    }
    catch (...) {
        lRet = DLG_ERR;
    }
    if (titlecharRefBytes != NULL)
        CFRelease(titlecharRefBytes);
    if (datacharRefBytes != NULL)
        CFRelease(datacharRefBytes);
    if (titlesArray != NULL)
        CFRelease(titlesArray);
    if (parameters != NULL)
        CFRelease(parameters);
    
    return lRet;
}


void eIDMW::DlgClosePinpadInfo( unsigned long theUserNotificationRef )
{
    SInt32 error = 0;
    
    CFUserNotificationRef userNotificationRef = mc_pinpad_map[theUserNotificationRef];
    error = CFUserNotificationCancel (userNotificationRef);
    
    if(userNotificationRef != NULL)
        CFRelease(userNotificationRef);
}


/*
 //TODO: Add Keypad possibility in DlgAskPin(s)
 DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
 DlgPinUsage usage, const wchar_t *wsPinName,
 DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen)
 {
 DlgRet lRet = DLG_CANCEL;
 CFArrayRef           titlesArray;
 //CFArrayRef           valuesArray;
 //DlgAskAccessArguments *oData = NULL;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 const wchar_t * Title;
 //https://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFUserNotificationRef/Reference/reference.html#//apple_ref/c/data/kCFUserNotificationAlertHeaderKey
 CFUserNotificationRef userNotificationRef;
 SInt32 error = 0;
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
 kCFUserNotificationAlertMessageKey,
 kCFUserNotificationTextFieldTitlesKey,
 //kCFUserNotificationTextFieldValuesKey,
 kCFUserNotificationProgressIndicatorValueKey,
 kCFUserNotificationDefaultButtonTitleKey,
 kCFUserNotificationAlternateButtonTitleKey};
 
 if( usage == DLG_PIN_SIGN )
 {
 Title=GETSTRING_DLG(SigningWith);
 //Title+= ": ";
 }
 else
 {
 Title=GETSTRING_DLG(Asking);
 //Title+= " ";
 }
 
 
 const void* textFieldTitles[] = {Title};
 
 titlesArray = CFArrayCreate(NULL, textFieldTitles, 1, NULL);
 //valuesArray = CFArrayCreate(NULL, textFieldValues, 3, NULL);
 
 //if (array) {
 //	CFArrayAppendValue(array, CFSTR("0"));
 //	CFArrayAppendValue(array, CFSTR("1"));
 //	CFArrayAppendValue(array, CFSTR("2"));
 //  }
 
 const void* values[] = {CFSTR("NotificationAlertHeader"),
 CFSTR("NotificationAlertMessage"),
 CFSTR("NotificationTextFieldTitles"),
 //CFSTR("NotificationTextFieldValuesKey"),
 CFSTR("NotificationProgressIndicatorValue"),
 CFSTR("NotificationDefaultButtonTitle"),
 CFSTR("NotificationAlternateButtonTitle")};
 
 
 
 CFOptionFlags optionFlags = kCFUserNotificationCautionAlertLevel | CFUserNotificationSecureTextField (0);
 
 CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
 
 sizeof(keys)/sizeof(*keys), NULL,
 
 NULL);
 
 //CFDictionaryAddValue(dict, kCFUserNotificationAlertHeaderKey, CFSTR("my title"));
 
 //CFDictionaryAddValue(dict, kCFUserNotificationAlternateButtonTitleKey, CFSTR("Cancel"));
 
 
 userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
 10000, //CFTimeInterval timeout,
 optionFlags,//CFOptionFlags flags,
 &error,//SInt32 *error,
 parameters);//CFDictionaryRef dictionary
 
 
 error = CFUserNotificationReceiveResponse (	userNotificationRef,
 10000,//CFTimeInterval timeout,
 &optionFlags);//CFOptionFlags *responseFlags
 //CFUserNotificationCancel
 CFRelease(titlesArray);
 CFRelease(parameters);
 CFRelease(userNotificationRef);
 
 
 return lRet;
 }
 */

/*
 DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
 DlgPinUsage usage, const wchar_t *wsPinName,
 DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen,
 DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen)
 {
 
 DlgRet lRet = DLG_CANCEL;
 
 DlgAskPINsArguments* oData;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 
 try {
 csReadableFilePath = CreateRandomFile();
 
 // creating the shared memory segment
 // attach oData
 oShMemory.Attach(sizeof(DlgAskPINsArguments),csReadableFilePath.c_str(),(void**)&oData);
 
 // collect the arguments into the struct placed
 // on the shared memory segment
 oData->operation = operation;
 oData->usage = usage;
 wcscpy_s(oData->pinName,sizeof(oData->pinName)/sizeof(wchar_t),wsPinName);
 oData->pin1Info = pin1Info;
 oData->pin2Info = pin2Info;
 wcscpy_s(oData->pin1,sizeof(oData->pin1)/sizeof(wchar_t),wsPin1);
 wcscpy_s(oData->pin2,sizeof(oData->pin2)/sizeof(wchar_t),wsPin2);
 
 CallQTServer(DLG_ASK_PINS,csReadableFilePath.c_str());
 lRet = oData->returnValue;
 
 if(lRet == DLG_OK) {
 wcscpy_s(wsPin1,ulPin1BufferLen,oData->pin1);
 wcscpy_s(wsPin2,ulPin2BufferLen,oData->pin2);
 }
 
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 } catch (...) {
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 
 return DLG_ERR;
 }
 return lRet;
 }*/







/*done
 DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
 const wchar_t *wsReader, DlgPinUsage usage, const wchar_t *wsPinName,
 const wchar_t *wsMessage,
 void **puserNotificationRef)
 {
 DlgRet lRet = DLG_CANCEL;
 CFArrayRef           titlesArray;
 //CFArrayRef           valuesArray;
 //DlgAskAccessArguments *oData = NULL;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 const wchar_t * Title;
 //https://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFUserNotificationRef/Reference/reference.html#//apple_ref/c/data/kCFUserNotificationAlertHeaderKey
 CFUserNotificationRef userNotificationRef;
 SInt32 error = 0;
 char datachar[256];
 char titlechar[256];
 CFOptionFlags optionFlags;
 
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
 kCFUserNotificationAlertMessageKey};//,
 //kCFUserNotificationTextFieldTitlesKey};//,
 //kCFUserNotificationTextFieldValuesKey,
 //kCFUserNotificationProgressIndicatorValueKey,
 //kCFUserNotificationDefaultButtonTitleKey,
 //kCFUserNotificationAlternateButtonTitleKey};
 
 if( usage == DLG_PIN_SIGN )
 {
 optionFlags = kCFUserNotificationCautionAlertLevel;
 Title=GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);//SigningWith);
 //Title+= ": ";
 }
 else
 {
 optionFlags = kCFUserNotificationPlainAlertLevel;
 Title=GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
 
 //Title+= " ";
 }
 
 //CFDataRef
 CFDataRef dataRef = CFDataCreate (
 kCFAllocatorDefault,
 (const UInt8 *)wsReader,
 sizeof(wsReader)
 );
 
 CFStringRef NotificationAlertMessage = CFStringCreateFromExternalRepresentation (
 kCFAllocatorDefault,
 dataRef,
 kCFStringEncodingUTF16
 );
 
 wcstombs(datachar,wsReader,sizeof(datachar));
 datachar[255]='\0';
 CFStringRef datacharRef = CFStringCreateWithCharacters (
 kCFAllocatorDefault,
 (const UniChar *)datachar,
 (CFIndex)strlen(datachar)
 );
 
 CFStringRef datacharRefBytes = CFStringCreateWithBytes (
 kCFAllocatorDefault,
 (const UInt8 *)datachar,
 strlen(datachar),
 kCFStringEncodingUTF8,
 false
 );
 
 wcstombs(titlechar,Title,sizeof(titlechar));
 titlechar[255]='\0';
 CFStringRef titlecharRefBytes = CFStringCreateWithBytes (
 kCFAllocatorDefault,
 (const UInt8 *)titlechar,
 strlen(titlechar),
 kCFStringEncodingUTF8,
 false
 );
 
 //wcscpy_s(oData->message,sizeof(oData->message)/sizeof(wchar_t),wsMessage);
 const void* textFieldTitles[] = {Title};
 
 titlesArray = CFArrayCreate(NULL, textFieldTitles, 1, NULL);
 //valuesArray = CFArrayCreate(NULL, textFieldValues, 3, NULL);
 
 //if (array) {
 //	CFArrayAppendValue(array, CFSTR("0"));
 //	CFArrayAppendValue(array, CFSTR("1"));
 //	CFArrayAppendValue(array, CFSTR("2"));
 //  }
 
 const void* values[] = {titlecharRefBytes,
 datacharRefBytes};//,
 //CFSTR("NotificationTextFieldTitles")};//,
 //CFSTR("NotificationTextFieldValuesKey"),
 //CFSTR("NotificationProgressIndicatorValue"),
 //CFSTR("NotificationDefaultButtonTitle"),
 //CFSTR("NotificationAlternateButtonTitle")};
 */
/*const void* values[] = {
 Title,
 titlesArray};
 //valuesArray,
 //CFSTR("YES"),
 //CFSTR("CANCEL")};*/

/*
 CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
 
 sizeof(keys)/sizeof(*keys), NULL,
 
 NULL);
 */
//CFDictionaryAddValue(dict, kCFUserNotificationAlertHeaderKey, CFSTR("my title"));

//CFDictionaryAddValue(dict, kCFUserNotificationAlternateButtonTitleKey, CFSTR("Cancel"));


/*	userNotificationRef = CFUserNotificationCreate (kCFAllocatorDefault, //CFAllocatorRef allocator,
 30, //CFTimeInterval timeout,
 optionFlags,//CFOptionFlags flags,
 &error,//SInt32 *error,
 parameters);//CFDictionaryRef dictionary
 */
//	*puserNotificationRef = userNotificationRef;
//error = CFUserNotificationReceiveResponse (	userNotificationRef,
//										   10000,//CFTimeInterval timeout,
//										   &optionFlags);//CFOptionFlags *responseFlags
//CFUserNotificationCancel
//	CFRelease(titlesArray);
//	CFRelease(parameters);
//CFRelease(userNotificationRef);
/*
 */
//   return lRet;



/*
 DlgRet lRet = DLG_CANCEL;
 
 DlgDisplayPinpadInfoArguments* oData;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 
 try {
 MWLOG(LEV_DEBUG, MOD_DLG,L"  eIDMW::DlgDisplayPinpadInfo called");
 csReadableFilePath = CreateRandomFile();
 
 oShMemory.Attach(sizeof(DlgDisplayPinpadInfoArguments),csReadableFilePath.c_str(),(void**)&oData);
 
 // collect the arguments into the struct placed
 // on the shared memory segment
 
 oData->operation = operation;
 wcscpy_s(oData->reader,sizeof(oData->reader)/sizeof(wchar_t),wsReader);
 oData->usage = usage;
 wcscpy_s(oData->pinName,sizeof(oData->pinName)/sizeof(wchar_t),wsPinName);
 wcscpy_s(oData->message,sizeof(oData->message)/sizeof(wchar_t),wsMessage);
 oData->infoCollectorIndex = ++dlgPinPadInfoCollectorIndex;
 
 CallQTServer(DLG_DISPLAY_PINPAD_INFO,csReadableFilePath.c_str());
 lRet = oData->returnValue;
 
 if (lRet != DLG_OK) {
 throw CMWEXCEPTION(EIDMW_ERR_SYSTEM);
 }
 
 // for the killing need to store:
 // - the shared memory area to be released (unique with the filename?)
 // - the child process ID
 // - the handle (because the user will use it)
 
 DlgRunningProc *ptRunningProc = new DlgRunningProc();
 ptRunningProc->iSharedMemSegmentID = oShMemory.getID();
 ptRunningProc->csRandomFilename = csReadableFilePath;
 
 ptRunningProc->tRunningProcess = oData->tRunningProcess;
 
 dlgPinPadInfoCollector[dlgPinPadInfoCollectorIndex] = ptRunningProc;
 
 if( pulHandle )
 *pulHandle = dlgPinPadInfoCollectorIndex;
 
 
 } catch(...) {
 
 
 return DLG_ERR;
 }
 //return lRet;
 
 }*/



/* only for linux, dlgmodal for win and apple */
/*
 DLGS_EXPORT DlgRet eIDMW::DlgAskAccess(
 const wchar_t *wsAppPath, const wchar_t *wsReaderName,
 DlgPFOperation ulOperation, int *piForAllOperations)
 {
 
 DlgRet lRet = DLG_CANCEL;
 
 //DlgAskAccessArguments *oData = NULL;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 
 CFUserNotificationRef userNotificationRef;
 SInt32 error = 0;
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
 
 kCFUserNotificationProgressIndicatorValueKey,
 
 kCFUserNotificationDefaultButtonTitleKey};
 
 const void* values[] = {CFSTR("Progress"),
 
 kCFBooleanTrue,
 
 CFSTR("Cancel")};
 CFOptionFlags optionFlags;
 *?
 CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
 
 sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
 
 &kCFTypeDictionaryValueCallBacks);
 CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
 
 sizeof(keys)/sizeof(*keys), NULL,
 
 NULL);
 
 userNotificationRef = CFUserNotificationCreate (NULL, //CFAllocatorRef allocator,
 10000, //CFTimeInterval timeout,
 kCFUserNotificationCautionAlertLevel,//CFOptionFlags flags,
 &error,//SInt32 *error,
 parameters);//CFDictionaryRef dictionary
 
 
 error = CFUserNotificationReceiveResponse (	userNotificationRef,
 10000,//CFTimeInterval timeout,
 &optionFlags);//CFOptionFlags *responseFlags
 
 
 
 
 
 
 try {
 csReadableFilePath = CreateRandomFile();
 
 // attach to the segment and get a pointer
 oShMemory.Attach( sizeof(DlgAskAccessArguments), csReadableFilePath.c_str(),(void **) &oData);
 
 wcscpy_s(oData->appPath,sizeof(oData->appPath)/sizeof(wchar_t),wsAppPath);
 wcscpy_s(oData->readerName,sizeof(oData->readerName)/sizeof(wchar_t),wsReaderName);
 oData->operation = ulOperation;
 oData->forAllOperations = *piForAllOperations;
 
 CallQTServer(DLG_ASK_ACCESS,csReadableFilePath.c_str());
 lRet = oData->returnValue;
 
 if(lRet == DLG_OK) *piForAllOperations = oData->forAllOperations;
 
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 } catch (...) {
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 return DLG_ERR;
 }
 return lRet;
 
 }
 
 {
 DlgRet lRet = DLG_CANCEL;
 CFArrayRef           titlesArray;
 //CFArrayRef           valuesArray;
 //DlgAskAccessArguments *oData = NULL;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 
 CFUserNotificationRef userNotificationRef;
 SInt32 error = 0;
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
 kCFUserNotificationAlertMessageKey,
 kCFUserNotificationTextFieldTitlesKey,
 //kCFUserNotificationTextFieldValuesKey,
 //kCFUserNotificationAlertMessageKey,
 kCFUserNotificationAlternateButtonTitleKey,
 kCFUserNotificationDefaultButtonTitleKey};
 
 
 const void* textFieldTitles[] = {CFSTR("old PIN"),CFSTR("new PIN"),CFSTR("new PIN")};
 //const void* textFieldValues[] = {CFSTR("0"),CFSTR("1"),CFSTR("2")};
 
 
 DlgRet lRet = DLG_CANCEL;
 
 DlgDisplayModalArguments* oData;
 SharedMem oShMemory;
 std::string csReadableFilePath;
 
 try {
 csReadableFilePath = CreateRandomFile();
 
 oShMemory.Attach( sizeof(DlgDisplayModalArguments), csReadableFilePath.c_str(),(void **) &oData);
 
 oData->icon = icon;
 if(wcslen(csMesg)==0)
 {
 std::wstring translatedMessage(CLang::GetMessageFromID(messageID));
 wcscpy_s(oData->mesg,sizeof(oData->mesg)/sizeof(wchar_t),translatedMessage.c_str());
 }
 else
 {
 wcscpy_s(oData->mesg,sizeof(oData->mesg)/sizeof(wchar_t),csMesg);
 }
 oData->buttons = ulButtons;
 oData->EnterButton = ulEnterButton;
 oData->CancelButton = ulCancelButton;
 
 CallQTServer(DLG_DISPLAY_MODAL,csReadableFilePath.c_str());
 lRet = oData->returnValue;
 
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 
 } catch(...) {
 // detach from the segment
 oShMemory.Detach(oData);
 
 // delete the random file
 DeleteFile(csReadableFilePath.c_str());
 
 return DLG_ERR;
 
 }
 
 
 
 //array = CFArrayCreate(NULL, 0, &kCFTypeArrayCallBacks);
 
 titlesArray = CFArrayCreate(NULL, textFieldTitles, 3, NULL);
 //valuesArray = CFArrayCreate(NULL, textFieldValues, 3, NULL);
 
 //if (array) {
 //	CFArrayAppendValue(array, CFSTR("0"));
 //	CFArrayAppendValue(array, CFSTR("1"));
 //	CFArrayAppendValue(array, CFSTR("2"));
 //  }
 
 const void* values[] = {CFSTR("Warning"),
 CFSTR("The following application is trying to read data on the eID card\nDo you allow that?\n\n PATH of the application: "),
 //CFSTR("please enter PIN"),//kCFBooleanTrue,
 titlesArray,
 //valuesArray,
 CFSTR("YES"),
 CFSTR("NO")};
 
 CFOptionFlags optionFlags = kCFUserNotificationCautionAlertLevel | CFUserNotificationSecureTextField (0);
 
 CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
 
 sizeof(keys)/sizeof(*keys), NULL,
 
 NULL);
 
 userNotificationRef = CFUserNotificationCreate (NULL, //CFAllocatorRef allocator,
 10000, //CFTimeInterval timeout,
 optionFlags,//CFOptionFlags flags,
 &error,//SInt32 *error,
 parameters);//CFDictionaryRef dictionary
 
 
 error = CFUserNotificationReceiveResponse (	userNotificationRef,
 10000,//CFTimeInterval timeout,
 &optionFlags);//CFOptionFlags *responseFlags
 
 CFRelease(titlesArray);
 CFRelease(parameters);
 CFRelease(userNotificationRef);
 
 
 return lRet;
 } */
