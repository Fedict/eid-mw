/* public API => only public header file */

#ifndef SCR_H
#define SCR_H

#ifdef WIN32
#ifdef SCR_EXPORTS
#define SCR_API __declspec(dllexport)
#else
#define SCR_API __declspec(dllimport)
#endif
#else
#define SCR_API
#endif

#ifdef __APPLE__
#include <wintypes.h>
#endif
#include <winscard.h>

typedef enum {
	SCR_USAGE_AUTH = 1L,
	SCR_USAGE_SIGN = 2L,
	SCR_USAGE_DECR = 4L,
	SCR_USAGE_PREF_MODIF = 8L,
	SCR_USAGE_ADMIN	= 16L
} SCR_PinUsageConstants;

typedef enum {
	SCR_SUPPORT_OK = 0L,
	SCR_SUPPORT_INCOMPATIBLE_CALLING_VERSION = 1L,
	SCR_SUPPORT_INCOMPATIBLE_FIRMWARE = 2L,
	SCR_SUPPORT_INCOMPATIBLE_FIRMWARE_VERSION = 3L,
} SCR_SupportConstants;

typedef struct {
	BYTE *data;
	DWORD length;
} SCR_Bytes;

typedef struct {
	SCARDHANDLE hCard;
	char *language;
	SCR_Bytes id;
	void *pinFormat; /* reserved for future use */
} SCR_Card;

typedef struct {
	SCR_Bytes id;
	char *shortString;
	char *longString;
} SCR_Application;

typedef struct {
	SCR_PinUsageConstants code;
	char *shortString;
	char *longString;
} SCR_PinUsage;

#define SCR_I_PIN_CHECK_FAILED	0x60100E02L
#define SCR_E_UNINITIALIZED 0xE0100E01L

SCR_API LONG SCR_Init(LPCTSTR szReader, DWORD version, SCR_SupportConstants *supported);
SCR_API LONG SCR_VerifyPIN(const SCR_Card*, BYTE pinID, const SCR_PinUsage*, const SCR_Application*, BYTE *cardStatus);
SCR_API LONG SCR_ChangePIN(const SCR_Card*, BYTE pinID, const SCR_Application*, BYTE *cardStatus);

/* if really emulating pinpad reader */
/* fixme: use #ifdef */

SCR_API LONG SCR_SendAPDU(const char *buf, SCARDHANDLE hCard);
/* for CSC */
SCR_API LONG SCR_SCardTransmit(
  SCARDHANDLE hCard,
  LPCSCARD_IO_REQUEST pioSendPci,
  LPCBYTE pbSendBuffer,
  DWORD cbSendLength,
  LPSCARD_IO_REQUEST pioRecvPci,
  LPBYTE pbRecvBuffer,
  LPDWORD pcbRecvLength
);

/* beidwinscarp and beisservicepcsc defines */
#define SCR_INIT_ID                 100
#define SCR_VERIFY_ID              101
#define SCR_CHANGE_ID              102
#define SCR_CARD_HANDLE          999

typedef long (*SCR_INIT_PROC)(LPCTSTR, DWORD, SCR_SupportConstants*);
typedef long (*SCR_VERIFYPIN_PROC)(const SCR_Card*, BYTE, const SCR_PinUsage*, const SCR_Application*, BYTE *);
typedef long (*SCR_CHANGEPIN_PROC)(const SCR_Card*, BYTE, const SCR_Application*, BYTE *);

#endif
