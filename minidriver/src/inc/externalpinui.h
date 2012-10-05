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
    US_PINCANCEL                        = 1
} UI_STATE;


//struct to communicate between logic an UI
typedef struct _EXTERNAL_PIN_INFORMATION
{
	SCARDHANDLE		hCardHandle;
	FEATURES		features;
	CARD_STATE		cardState;
	UI_STATE		uiState;
	int				iPinCharacters;
	HWND			hwndParentWindow;
	wchar_t			lpstrPinContextString[256];
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
	de,
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
USHORT getLangID();

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
		L"Appuyez sur Annuler sur le clavier PIN",
		// de
		L"Drücken Sie auf dem PIN-Pad auf Cancel (Abbruch)"
	},
	// CANCEL_CONTENT
	{
		// en
		L"To cancel the operation, press the Cancel button on the PIN pad.",
		// nl
		L"Om de operatie te annuleren, druk op Cancel op het PIN pad.",
		// fr
		L"Pour annuler l�op�ration, appuyez sur Annuler sur le clavier PIN.",
		// de
		L"Um den Vorgang abzubrechen, drücken Sie auf dem PIN-Pad die Taste Cancel."
	},
	// 	WINDOW_TITLE
	{
		// en
		L"External PIN pad entry",
		// nl
		L"Extern PIN pad",
		// fr
		L"Clavier PIN externe",
		// de
		L"Eingabe über externes PIN-Pad"
	},
	// ENTER_PIN_MAININSTRUCTIONS
	{
		// en
		L"Enter your PIN code on the PIN pad",
		// nl
		L"Tik uw PIN code in op het PIN pad",
		// fr
		L"Ins�rez votre code PIN sur le clavier PIN",
		// de
		L"Geben Sie Ihren PIN-Code auf dem PIN-Pad ein"
	},
	// ENTER_PIN_CONTENT
	{
		// en
		L"Confirm your PIN with the OK button on the external PIN pad.",
		// nl
		L"Bevestig uw PIN met de OK knop op het externe PIN pad.",
		// fr
		L"Confirmez votre code PIN avec le bouton OK du clavier PIN externe.",
		// de
		L"Bestätigen Sie Ihre PIN mit der Taste OK auf dem externen PIN-Pad."
	},
	// PIN_TOO_SHORT_MAININSTRUCTIONS
	{
		// en
		L"PIN too short",
		// nl
		L"PIN is te kort",
		// fr
		L"Code PIN trop court",
		// de
		L"PIN zu kurz"
	},
	// PIN_TOO_SHORT_CONTENT
	{
		// en
		L"The PIN you entered is too short. Please enter at least 4 numbers.",
		// nl
		L"De PIN is te kort. Gelieve minimum 4 cijfers in te tikken.",
		// fr
		L"Le code PIN est trop court. Veuillez ins�rer au minimum 4 chiffres.",
		// de
		L"Die von Ihnen eingegebene PIN ist zu kurz. Bitte mindestens vier Ziffern eingeben."
	},
	// PIN_TIMED_OUT_MAININSTRUCTIONS
	{
		// en
		L"PIN entry timed out",
		// nl
		L"Wachttijd is verlopen",
		// fr
		L"Le temps d�introduction du code PIN est d�pass�",
		// de
		L"Zeit für PIN-Eingabe abgelaufen"
	},
	// PIN_TIMED_OUT_CONTENT
	{
		// en
		L"On the PIN pad, push OK to confirm the PIN.",
		// nl
		L"Bevestig uw PIN met OK op het PIN pad.",
		// fr
		L"Confirmez votre PIN en tapant OK sur le clavier PIN.",
		// de
		L"Drücken Sie auf dem PIN-Pad auf OK, um die PIN zu bestätigen."
	},	
	// PIN_CANCELLED_MAININSTRUCTIONS
	{
		// en
		L"PIN entry cancelled",
		// nl
		L"PIN operatie geannuleerd",
		// fr
		L"Op�ration PIN annul�e",
		// de
		L"PIN-Eingabe abgebrochen"
	},
	// PIN_CANCELLED_CONTENT
	{
		// en
		L"You pushed the Cancel button.",
		// nl
		L"U drukte op de Cancel knop",
		// fr
		L"Vous avez appuy� sur la touche Annuler",
		// de
		L"Sie haben die Taste Cancel gedrückt"
	},
	// PIN_DO_NOT_MATCH_MAININSTRUCTIONS
	{
		// en
		L"New PINs do not match",
		// nl
		L"De nieuwe PINs komen niet overeen",
		// fr
		L"Les nouveaux codes PIN ne conviennent pas",
		// de
		L"Neue PINs stimmen nicht überein"
	},
	// PIN_DO_NOT_MATCH_CONTENT
	{
		// en
		L"Modify PIN failed because two new PIN entries do not match",
		// nl
		L"PIN wijzigen is mislukt omdat de nieuwe PINs niet overeenkomen",
		// fr
		L"La modification du code PIN a �chou� car les nouveaux codes PIN ne conviennent pas",
		// de
		L"PIN ändern nicht erfolgreich, da zwei neue PIN-Eingaben nicht übereinstimmen"
	},
	// PIN_TOO_LONG_MAININSTRUCTIONS
	{
		// en
		L"PIN too long",
		// nl
		L"PIN is te lang",
		// fr
		L"Code PIN trop long",
		// de
		L"PIN zu lang"
	},
	// PIN_TOO_LONG_CONTENT
	{
		// en
		L"The PIN you entered was too long. Please enter not more than 12 numbers.",
		// nl
		L"De PIN is te lang. Gelieve maximum 12 cijfers in te tikken.",
		// fr
		L"Le code PIN est trop long. Veuillez ins�rer au maximum 12 chiffres.",
		// de
		L"Die von Ihnen eingegebene PIN ist zu lang. Bitte nicht mehr als 12 Ziffern eingeben."
	},
	// PIN_SIZE_MAININSTRUCTIONS
	{
		// en
		L"PIN entry failed",
		// nl
		L"Verkeerde PIN lengte",
		// fr
		L"Le code PIN a une longueur inappropri�e",
		// de
		L"PIN-Eingabe gescheitert"
	},
	// PIN_SIZE_CONTENT
	{
		// en
		L"The PIN you entered is too short or too long. Please enter at least 4 numbers and not more than 12 numbers.",
		// nl
		L"De PIN is te kort of te lang. De PIN moet tussen 4 en 12 cijfers lang zijn.",
		// fr
		L"Le code PIN est soit trop court soit trop long. Il doit comprendre entre 4 et 12 chiffres.",
		// de
		L"Die von Ihnen eingegebene PIN ist zu kurz oder zu lang. Bitte mindestens 4 und höchstens 12 Ziffern eingeben."
	},
	// PIN_UNKNOWN_MAININSTRUCTIONS
	{
		// en
		L"Unknown error",
		// nl
		L"Onbekende fout",
		// fr
		L"Erreur inconnue",
		// de
		L"Unbekannter Fehler"
	},
	// PIN_UNKNOWN_CONTENT
	{
		// en
		L"Unknown return value %x %x.",
		// nl
		L"Onbekende waarde: %x %x.",
		// fr
		L"Valeur inconnue: %x %x.",
		// de
		L"Unbekannter Rückgabewert: %x %x."
	},
	// PIN_INVALID_MAININSTRUCTIONS
	{
		// en
		L"Invalid PIN",
		// nl
		L"Foute PIN",
		// fr
		L"Code PIN incorrect",
		// de
		L"Ungültige PIN"
	},
	// PIN_INVALID_CONTENT
	{
		// en
		L"The PIN you entered is invalid. Retries left: %d",
		// nl
		L"De PIN die u ingaf is fout. Pogingen over: %d",
		// fr
		L"Le code PIN que vous avez introduit est erron�. Il vous reste %d essais",
		// de
		L"Die von Ihnen eingegebene PIN ist ungültig. Sie haben noch %d weitere Versuche."
	},
	// PIN_BLOCKED_MAININSTRUCTIONS
	{
		// en
		L"PIN entry failed",
		// nl
		L"PIN geblokkeerd",
		// fr
		L"PIN bloqu�",
		// de
		L"PIN-Eingabe gescheitert"
	},
	// PIN_BLOCKED_CONTENT
	{
		// en
		L"PIN blocked!",
		// nl
		L"De PIN is geblokkeerd",
		// fr
		L"Le code PIN a �t� bloqu�",
		// de
		L"PIN gesperrt!"
	}
};

#endif
