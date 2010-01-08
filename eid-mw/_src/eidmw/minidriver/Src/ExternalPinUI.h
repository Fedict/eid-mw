#ifndef __EXTERNALPINUI_H__
#define __EXTERNALPINUI_H__
typedef enum _CARD_STATES
{
    CS_PINENTRY                         = 0,
    CS_PINCANCEL                        = 1,
    CS_PINTIMEOUT                       = 2,        
    CS_PINFAILED                        = 3,
	CS_PINVERIFIED                      = 4,
	CS_PINENTERED                       = 5
} CARD_STATE;

typedef enum _UI_STATE
{
    US_PINENTRY                         = 0,
    US_PINCANCEL                        = 1,
} UI_STATE;


//struct to communicate between logic an UI
typedef struct _EXTERNAL_PIN_INFORMATION
{
	SCARDHANDLE hCardHandle;
	FEATURES	features;
	CARD_STATE  cardState;
	UI_STATE	uiState;
	int			iPinCharacters;
} EXTERNAL_PIN_INFORMATION, *PEXTERNAL_PIN_INFORMATION;

typedef enum _LOCALIZATION_STRINGS
{
	CANCEL_MAININSTRUCTIONS,
	CANCEL_CONTENT,
	WINDOW_TITLE,
	ENTER_PIN_MAININSTRUCTIONS,
	ENTER_PIN_CONTENT,
	PIN_TOO_SHORT_MAININSTRUCTIONS,
	PIN_TOO_SHORT_CONTENT,
	PIN_TIMED_OUT_MAININSTRUCTIONS,
	PIN_TIMED_OUT_CONTENT,
	PIN_CANCELLED_MAININSTRUCTIONS,
	PIN_CANCELLED_CONTENT,
	PIN_DO_NOT_MATCH_MAININSTRUCTIONS,
	PIN_DO_NOT_MATCH_CONTENT,
	PIN_TOO_LONG_MAININSTRUCTIONS,
	PIN_TOO_LONG_CONTENT,
	PIN_SIZE_MAININSTRUCTIONS,
	PIN_SIZE_CONTENT,
	PIN_UNKNOWN_MAININSTRUCTIONS,
	PIN_UNKNOWN_CONTENT,
	PIN_INVALID_MAININSTRUCTIONS,
	PIN_INVALID_CONTENT,
	PIN_BLOCKED_MAININSTRUCTIONS,
	PIN_BLOCKED_CONTENT,
	MAX_LOCALIZATION_STRINGS
	
} LOCALIZATION_STRINGS;

typedef enum _LANGUAGES
{
	en,
	nl,
	fr,
	MAX_LANGUAGES
} LANGUAGES;

HRESULT CALLBACK TaskDialogCallbackProcPinEntry
	(      
    HWND hwnd,
    UINT uNotification,
    WPARAM wParam,
    LPARAM lParam,
    LONG_PTR dwRefData
	)
;

DWORD WINAPI DialogThreadPinEntry
	(
	LPVOID lpParam
	)
;
LANGUAGES getLanguage();

// translations for UI
static wchar_t * t[MAX_LOCALIZATION_STRINGS][MAX_LANGUAGES] =
{ 
	// CANCEL_MAININSTRUCTIONS
	{
		// en
		L"Press cancel on the PIN pad",
		// nl
		L"Druk op Cancel op het PIN pad",
		// fr
		L""
	},
	// CANCEL_CONTENT
	{
		// en
		L"To cancel the operation, press the Cancel button on the PIN pad.",
		// nl
		L"Om te annuleren, druk op Cancel op het PIN pad.",
		// fr
		L""
	},
	// 	WINDOW_TITLE 
	{
		// en
		L"External PIN pad entry",
		// nl
		L"Extern PIN pad",
		// fr
		L""
	},
	// ENTER_PIN_MAININSTRUCTIONS
	{
		// en
		L"Enter your PIN code on the PIN pad",
		// nl
		L"Tik je PIN code in op het PIN pad",
		// fr
		L""
	},
	// ENTER_PIN_CONTENT
	{
		// en
		L"Confirm your PIN with the OK button on the external PIN pad.",
		// nl
		L"Bevestig je PIN met de OK knop op het externe PIN pad.",
		// fr
		L""
	},
	// PIN_TOO_SHORT_MAININSTRUCTIONS
	{
		// en
		L"PIN too short",
		// nl
		L"PIN is te kort",
		// fr
		L""
	},
	// PIN_TOO_SHORT_CONTENT
	{
		// en
		L"The PIN you entered is too short. Minimum PIN length is 4.",
		// nl
		L"De PIN is te kort. De minimum lengte is 4.",
		// fr
		L""
	},
	// PIN_TIMED_OUT_MAININSTRUCTIONS
	{
		// en
		L"PIN entry timed out",
		// nl
		L"Wachttijd is verlopen",
		// fr
		L""
	},
	// PIN_TIMED_OUT_CONTENT
	{
		// en
		L"On the PIN pad, push OK after you entered the PIN.",
		// nl
		L"Bevestig je PIN met OK op het PIN pad",
		// fr
		L""
	},	
	// PIN_CANCELLED_MAININSTRUCTIONS
	{
		// en
		L"PIN entry cancelled",
		// nl
		L"PIN operatie geannuleerd",
		// fr
		L""
	},
	// PIN_CANCELLED_CONTENT
	{
		// en
		L"You pushed the Cancel button.",
		// nl
		L"U drukte op de Cancel knop",
		// fr
		L""
	},
	// PIN_DO_NOT_MATCH_MAININSTRUCTIONS
	{
		// en
		L"New PINs do not match",
		// nl
		L"De nieuwe PINs komen niet overeen",
		// fr
		L""
	},
	// PIN_DO_NOT_MATCH_CONTENT
	{
		// en
		L"Modify PIN failed because two new PIN entries do not match",
		// nl
		L"PIN wijzigen is mislukt omdat de nieuwe PINs niet overeenkomen",
		// fr
		L""
	},
	// PIN_TOO_LONG_MAININSTRUCTIONS
	{
		// en
		L"PIN too long",
		// nl
		L"PIN is te lang",
		// fr
		L""
	},
	// PIN_TOO_LONG_CONTENT
	{
		// en
		L"The PIN you entered was too long. Maximum PIN length is 12.",
		// nl
		L"De PIN is te lang. De maximum lengte is 12.",
		// fr
		L""
	},
	// PIN_SIZE_MAININSTRUCTIONS
	{
		// en
		L"PIN entry failed",
		// nl
		L"Verkeerde PIN lengte",
		// fr
		L""
	},
	// PIN_SIZE_CONTENT
	{
		// en
		L"The PIN you entered is too short or too long. Minimum length is 4, maximum is 12.",
		// nl
		L"De PIN is te kort of te lang. De PIN moet tussen 4 en 12 cijfers lang zijn.",
		// fr
		L""
	},
	// PIN_UNKNOWN_MAININSTRUCTIONS
	{
		// en
		L"Unknown error",
		// nl
		L"Onbekende fout",
		// fr
		L""
	},
	// PIN_UNKNOWN_CONTENT
	{
		// en
		L"Unknown return value %x %x",
		// nl
		L"Onbekende waarde: %x %x",
		// fr
		L""
	},
	// PIN_INVALID_MAININSTRUCTIONS
	{
		// en
		L"Invalid PIN",
		// nl
		L"Foute PIN",
		// fr
		L""
	},
	// PIN_INVALID_CONTENT
	{
		// en
		L"The PIN you entered is invalid. Retries left: %d",
		// nl
		L"De PIN die je ingaf is fout. Pogingen over: %d",
		// fr
		L""
	},
	// PIN_BLOCKED_MAININSTRUCTIONS
	{
		// en
		L"PIN entry failed",
		// nl
		L"PIN geblokkeerd",
		// fr
		L""
	},
	// PIN_BLOCKED_CONTENT
	{
		// en
		L"PIN blocked!",
		// nl
		L"De PIN is geblokkeerd",
		// fr
		L""
	}
};

#endif