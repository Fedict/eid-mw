//contains the functions, enums and structures that need to be knonw by the ui

#ifndef EID_VWR_GTK_OSLAYER_H
#define EID_VWR_GTK_OSLAYER_H

#include <conversions.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef WIN32
#define DllExport   __declspec( dllexport ) 
#else
#define DllExport
#endif

/* Types of data sources we can have */
enum eid_vwr_source {
	EID_VWR_SRC_NONE, // No source. UI should be cleared.
	EID_VWR_SRC_FILE, // File source. "Print", "Validate" and "Close" operations should be allowed, PIN-related operations are impossible. Save is pointless.
	EID_VWR_SRC_CARD, // Card source. Close should not be allowed, everything else should be.
	EID_VWR_SRC_UNKNOWN, // Unknown. Used as initializer. */
};

/* Log levels.
   Note: LOG_ERROR produces a message box with "OK" button, so should only be
   used for critical errors */
enum eid_vwr_loglevel {
	EID_VWR_LOG_DETAIL = 0, /* most detail */
	EID_VWR_LOG_NORMAL = 1,
	EID_VWR_LOG_COARSE = 2, /* least detail */
	EID_VWR_LOG_ERROR = 3, /* error message */
};

/* PIN operations. TEST: perform a login (and return whether the login was successful). CHG: change PIN */
enum eid_vwr_pinops {
	EID_VWR_PINOP_TEST,
	EID_VWR_PINOP_CHG,
};

/* Result of an operation. */
enum eid_vwr_result {
    EID_VWR_FAILED,
    EID_VWR_SUCCESS,
};

/* Possible states. See be-statemach.uml in uml/ directory. */
enum eid_vwr_states {
	STATE_LIBOPEN,
	STATE_CALLBACKS,
	STATE_READY,
	STATE_TOKEN,
	STATE_TOKEN_WAIT,
	STATE_TOKEN_ID,
	STATE_TOKEN_CERTS,
	STATE_TOKEN_PINOP,
	STATE_TOKEN_SERIALIZE,
	STATE_TOKEN_ERROR,
	STATE_FILE,
	STATE_CARD_INVALID,
	STATE_NO_TOKEN,
	STATE_NO_READER,

	STATE_COUNT,
};

/* Callbacks which the backend can perform towards the UI */
struct eid_vwr_ui_callbacks {
	void(*newsrc)(enum eid_vwr_source); // data source has changed.
	void(*newstringdata)(const char* label, const char* data); // new string data to be displayed in UI.
	void(*newbindata)(const char* label, const unsigned char* data, int datalen); // new binary data to be displayed in UI.
	void(*log)(enum eid_vwr_loglevel, const char* line); // log a string at the given level.
	void(*logv)(enum eid_vwr_loglevel, const char* line, va_list ap); // log a string using varargs. Note: a UI needs to implement only one of log() or logv(); the backend will use whichever is implemented.
	void(*newstate)(enum eid_vwr_states); // issued at state machine transition
	void(*pinop_result)(enum eid_vwr_pinops, enum eid_vwr_result); // issued when a PIN operation finished.
};

/* Struct used by preview handler */
struct eid_vwr_preview {
	void* imagedata; // JPEG-encoded preview
	size_t imagelen; // length of imagedata
	int have_data; // nonzero if there is actually any data
};

/* Perform a PIN operation */
DllExport int eid_vwr_pinop(enum eid_vwr_pinops);
/* Initialize the callbacks */
DllExport int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks* cb);

/* Creates the eid_vwr_ui_callbacks struct. */
struct eid_vwr_ui_callbacks* eid_vwr_cbstruct();
/* Create a preview for the given file */
struct eid_vwr_preview* eid_vwr_get_preview(const char* filename);

/* Check if there is an event to be handled. Will handle one event and then return. */
void eid_vwr_poll();
/* Loop over the above. Does not return. */
DllExport void eid_vwr_be_mainloop();
/* Save the currently-open data. Returns before the file has been saved; saving
 * data is done in a background thread. */
DllExport void eid_vwr_be_serialize(const char* target_file);
/* Open a file. Returns before the file has been opened; reading the file and
 * parsing it is done in a background thread. */
DllExport void eid_vwr_be_deserialize(const char* source_file);
/* Get the currently-open data in XML form, usable by the drag-and-drop
 * handler. Note: do *not* free this data. */
const char* eid_vwr_be_get_xmlform();

#endif
