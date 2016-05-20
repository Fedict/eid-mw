/**
 * \defgroup C_API the (base) C API
 *
 * @{
 */

/** \file oslayer.h
 *  \brief contains the functions, enums and structures that need to be known by the ui
 */

#ifndef EID_VWR_OSLAYER_H
#define EID_VWR_OSLAYER_H

#include <conversions.h>
#include <stdlib.h>
#include <stdarg.h>
#include <eid-viewer/macros.h>

typedef struct _slotdesc slotdesc;

/** Describes a slot (reader) */
struct _slotdesc {
	unsigned long slot; ///< The number of the slot, to be used with eid_vwr_be_select_slot()
	char* description[65]; ///< A human-readable name for the reader
};

/** Types of data sources we can have */
enum eid_vwr_source {
	EID_VWR_SRC_NONE, ///< No source. UI should be cleared.
	EID_VWR_SRC_FILE, ///< File source. "Print", "Validate" and "Close" operations should be allowed, PIN-related operations are impossible. Save is pointless (but may be possible).
	EID_VWR_SRC_CARD, ///< Card source. Close should not be allowed, everything else should be.
	EID_VWR_SRC_UNKNOWN, ///< Unknown. Used as initializer.
};

/** \brief Log levels.
 *
 * These are the log levels which the backend may use.
 */
enum eid_vwr_loglevel {
	EID_VWR_LOG_DETAIL = 0, /**< most detail */
	EID_VWR_LOG_NORMAL = 1, /**< intermediate detail */
	EID_VWR_LOG_COARSE = 2, /**< least detail */
	EID_VWR_LOG_ERROR = 3, /**< error message; a dialog box with the message (in addition to adding to the log) may be appropriate */
};

/** PIN operations. TEST: perform a login (and return whether the login was successful). CHG: change PIN */
enum eid_vwr_pinops {
	EID_VWR_PINOP_TEST, ///< perform a login and return whether the login was successful
	EID_VWR_PINOP_CHG, ///< change the PIN code of the card
};

/** Result of an operation. */
enum eid_vwr_result {
    EID_VWR_RES_FAILED, ///< operation failed
    EID_VWR_RES_SUCCESS, ///< the operation returned successfully
    EID_VWR_RES_UNKNOWN ///< the operation did not finish
};

/** \brief Possible states.
 *
 * \image html be-statemach-docs.png
 */
enum eid_vwr_states {
	STATE_LIBOPEN, ///< The library has been opened. Used as an initializer.
	STATE_CALLBACKS, ///< The callbacks have been defined.
	STATE_READY, ///< Ready to receive a token (eID card)
	STATE_TOKEN, ///< A token (eID card) has been found
	STATE_TOKEN_WAIT, ///< The card has been read, we're now waiting for events to do something else.
	STATE_TOKEN_ID, ///< Reading identity data
	STATE_TOKEN_CERTS, ///< Reading certificates
	STATE_TOKEN_PINOP, ///< Performing a PIN operation
	STATE_TOKEN_SERIALIZE, ///< Saving data to a file
	STATE_TOKEN_ERROR, ///< An error occurred while dealing with the card
	STATE_FILE, ///< We've read data from a file
	STATE_CARD_INVALID, ///< The data was determined to be invalid. That is, the card could be read, but signature validation failed.
	STATE_NO_TOKEN, ///< We don't have a card, and we also don't have a file.
	STATE_NO_READER, ///< We don't have a reader (yet?)

	STATE_COUNT,
};

/** Possible languages. */
enum eid_vwr_langs {
	EID_VWR_LANG_NONE, ///< No language has yet been selected. Used as initializer.
	EID_VWR_LANG_DE, ///< German
	EID_VWR_LANG_EN, ///< English
	EID_VWR_LANG_FR, ///< French
	EID_VWR_LANG_NL, ///< Dutch
};


/** Callbacks which the backend can perform towards the UI */
struct eid_vwr_ui_callbacks {
	void(*newsrc)(enum eid_vwr_source); ///< data source has changed.
	void(*newstringdata)(const EID_CHAR* label, const EID_CHAR* data); ///< new string data to be displayed in UI.
	void(*newbindata)(const EID_CHAR* label, const unsigned char* data, int datalen); ///< new binary data to be displayed in UI.
	void(*log)(enum eid_vwr_loglevel, const EID_CHAR* line); ///< log a string at the given level.
	void(*logv)(enum eid_vwr_loglevel, const EID_CHAR* line, va_list ap); ///< log a string using varargs. Note: a UI needs to implement only one of log() or logv(); the backend will use whichever is implemented.
	void(*newstate)(enum eid_vwr_states); ///< issued at state machine transition
	void(*pinop_result)(enum eid_vwr_pinops, enum eid_vwr_result); ///< issued when a PIN operation finished.
	void(*readers_changed)(unsigned long nreaders, slotdesc* slots); ///< issued when number of readers changes
};

/** Struct used by preview handler */
struct eid_vwr_preview {
	void* imagedata; ///< JPEG-encoded preview
	size_t imagelen; ///< length of imagedata
	int have_data; ///< nonzero if there is actually any data
};

/** Perform a PIN operation */
DllExport void eid_vwr_pinop(enum eid_vwr_pinops);
/** Initialize the callbacks */
DllExport int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb);

/** Creates the eid_vwr_ui_callbacks struct. */
DllExport struct eid_vwr_ui_callbacks* eid_vwr_cbstruct();
/** Create a preview for the given file */
DllExport struct eid_vwr_preview* eid_vwr_get_preview(const EID_CHAR* filename);

/** Check if there is an event to be handled. Will handle one event and then return. */
DllExport void eid_vwr_poll();
/** Loop over the above. Does not return. */
DllExport void eid_vwr_be_mainloop();
/** Save the currently-open data. Returns before the file has been saved; saving
 * data is done in a background thread. To detect when the save operation has
 * finished, listen to the state machine. */
DllExport void eid_vwr_be_serialize(const EID_CHAR* target_file);
/** Open a file. Returns before the file has been opened; reading the file and
 * parsing it is done in a background thread. To detect when the open operation
 * has finished, listen to the state machine. */
DllExport void eid_vwr_be_deserialize(const EID_CHAR* source_file);
/** Get the currently-open data in XML form, usable by the drag-and-drop
 * handler. Note: do *not* free this data. */
DllExport const char* eid_vwr_be_get_xmlform();
/** Select the current reader. If "automatic" is nonzero, let the backend decide
 * which reader and ignore the value of manualslot; otherwise, force
 * "manualslot" to be the active reader. */
DllExport void eid_vwr_be_select_slot(int automatic, unsigned long manualslot);
/** Set the card to "invalid". This should really not be done from the UI
 * but from the backend, but for now, keep things as they are. */
DllExport void eid_vwr_be_set_invalid();
/** Close the current file */
DllExport void eid_vwr_close_file();
/** Inform the backend of what the user interface is, so that converted
 * strings are in the correct language */
DllExport void eid_vwr_convert_set_lang(enum eid_vwr_langs which);

/**@}*/

#endif
