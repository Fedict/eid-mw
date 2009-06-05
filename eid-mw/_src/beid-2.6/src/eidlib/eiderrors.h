//////////////////////////////////////////////////////////////////////
// OpenSC Error Mapping
//////////////////////////////////////////////////////////////////////

#ifndef _EID_ERRORS_H
#define _EID_ERRORS_H

#include <string.h>

#include <opensc/errors.h>
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifdef _WIN32
#include <windows.h>
#include <SCardErr.h>
#else
#include <pcsclite.h>
#define SCARD_E_NO_READERS_AVAILABLE    0x8010002E
#define SCARD_E_FILE_NOT_FOUND              0x80100024
#define SCARD_W_CHV_BLOCKED                 0x8010006C
#define SCARD_E_INVALID_CHV                     0x8010002A
#define SCARD_E_NO_ACCESS                       0x80100027
#endif

typedef struct
{
    int iOpenSC;
    int iMapped;
    int iType;
    unsigned char ucSW[2];
} tOpenSCError;


const static tOpenSCError OpenSCErrors[] = {
/* Errors related to reader operation */
    {SC_ERROR_READER, SCARD_E_READER_UNAVAILABLE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_NO_READERS_FOUND, SCARD_E_NO_READERS_AVAILABLE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_SLOT_NOT_FOUND, SCARD_E_NO_SMARTCARD, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_SLOT_ALREADY_CONNECTED, SCARD_E_READER_UNAVAILABLE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CARD_NOT_PRESENT, SCARD_E_NO_SMARTCARD, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CARD_REMOVED, SCARD_W_REMOVED_CARD, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CARD_RESET, SCARD_W_RESET_CARD, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_TRANSMIT_FAILED, SCARD_F_COMM_ERROR, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_KEYPAD_TIMEOUT, 0, BEID_E_KEYPAD_TIMEOUT, {0x00, 0x00}},
    {SC_ERROR_KEYPAD_CANCELLED, 0, BEID_E_KEYPAD_CANCELLED, {0x00, 0x00}},
    {SC_ERROR_KEYPAD_PIN_MISMATCH, 0, BEID_E_KEYPAD_PIN_MISMATCH, {0x00, 0x00}},
    {SC_ERROR_KEYPAD_MSG_TOO_LONG, 0, BEID_E_KEYPAD_MSG_TOO_LONG, {0x00, 0x00}},
    {SC_ERROR_EVENT_TIMEOUT, SCARD_E_TIMEOUT, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CARD_UNRESPONSIVE, SCARD_W_UNRESPONSIVE_CARD, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_READER_DETACHED, SCARD_E_READER_UNAVAILABLE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_READER_REATTACHED, SCARD_E_READER_UNAVAILABLE, BEID_E_PCSC, {0x00, 0x00}},

/* Resulting from a card command or related to the card*/
    {SC_ERROR_CARD_CMD_FAILED, 0, BEID_E_CARD, {0x62, 0x82}},
    {SC_ERROR_FILE_NOT_FOUND, SCARD_E_FILE_NOT_FOUND, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_RECORD_NOT_FOUND, SCARD_E_FILE_NOT_FOUND, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CLASS_NOT_SUPPORTED, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_INS_NOT_SUPPORTED, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_INCORRECT_PARAMETERS, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_WRONG_LENGTH, SCARD_E_INVALID_VALUE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_MEMORY_FAILURE, SCARD_E_NO_MEMORY, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_NO_CARD_SUPPORT, SCARD_E_UNSUPPORTED_FEATURE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_NOT_ALLOWED, SCARD_E_UNSUPPORTED_FEATURE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_INVALID_CARD, SCARD_E_CARD_UNSUPPORTED, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_SECURITY_STATUS_NOT_SATISFIED, 0, BEID_E_CARD, {0x69, 0x82}},
    {SC_ERROR_AUTH_METHOD_BLOCKED, SCARD_W_CHV_BLOCKED, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_UNKNOWN_DATA_RECEIVED, SCARD_E_UNSUPPORTED_FEATURE, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_PIN_CODE_INCORRECT, SCARD_E_INVALID_CHV, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_FILE_ALREADY_EXISTS, SCARD_E_NO_ACCESS, BEID_E_PCSC, {0x00, 0x00}},

/* Returned by OpenSC library when called with invalid arguments */
    {SC_ERROR_INVALID_ARGUMENTS, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CMD_TOO_SHORT, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_CMD_TOO_LONG, SCARD_E_INVALID_PARAMETER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_BUFFER_TOO_SMALL, SCARD_E_INSUFFICIENT_BUFFER, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_INVALID_PIN_LENGTH, 0, BEID_E_INVALID_PIN_LENGTH, {0x00, 0x00}},

/* Resulting from OpenSC internal operation */
    {SC_ERROR_INTERNAL, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_INVALID_ASN1_OBJECT, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_ASN1_OBJECT_NOT_FOUND, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_ASN1_END_OF_CONTENTS, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_OUT_OF_MEMORY, SCARD_E_NO_MEMORY, BEID_E_PCSC, {0x00, 0x00}},
    {SC_ERROR_TOO_MANY_OBJECTS, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_OBJECT_NOT_VALID, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_OBJECT_NOT_FOUND, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_NOT_SUPPORTED, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_PASSPHRASE_REQUIRED, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_EXTRACTABLE_KEY, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_DECRYPT_FAILED, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_WRONG_PADDING, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_WRONG_CARD, SCARD_E_CARD_UNSUPPORTED, BEID_E_PCSC, {0x00, 0x00}},

/* Relating to PKCS #15 init stuff */
    {SC_ERROR_PKCS15INIT, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_SYNTAX_ERROR, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_INCONSISTENT_PROFILE, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_INCOMPATIBLE_KEY, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_NO_DEFAULT_KEY, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_ID_NOT_UNIQUE, 0, BEID_E_INTERNAL, {0x00, 0x00}},
    {SC_ERROR_CANNOT_LOAD_KEY, 0, BEID_E_INTERNAL, {0x00, 0x00}},

/* Errors that do not fit the categories above */
    {SC_ERROR_UNKNOWN, 0, BEID_E_UNKNOWN, {0x00, 0x00}},
    {SC_ERROR_PKCS15_APP_NOT_FOUND, 0, BEID_E_UNKNOWN, {0x00, 0x00}}
};


static BOOL ConvertOpenSCError(long ulError, BEID_Status *ptStatus)
{
    if(SC_NO_ERROR == ulError)
    {
        ptStatus->general = BEID_OK;
        ptStatus->pcsc = BEID_OK;
        ptStatus->system = BEID_OK;
        memset(ptStatus->cardSW, 0, 2); 
        return TRUE;
    }

	int iErrCount = sizeof(OpenSCErrors)/sizeof(OpenSCErrors[0]);
	for (int i = 0; i < iErrCount; i++)
    {
		if (OpenSCErrors[i].iOpenSC == ulError) 
        {
            ptStatus->general = OpenSCErrors[i].iType;
            ptStatus->pcsc = OpenSCErrors[i].iMapped;
            memcpy(ptStatus->cardSW, OpenSCErrors[i].ucSW, 2); 
			return TRUE;
		}
    }
    /* Error Not Found, check if other OpenSC error */
    if(-1099 > ulError && ulError > -1999)
    {
        ptStatus->general = BEID_E_UNKNOWN;
        return TRUE;
    }
    return FALSE;
}

#endif /* _EID_ERRORS_H */
