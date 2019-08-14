
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

#include <stdlib.h>
#include <stdarg.h>
#include <eid-util/utftranslate.h>
#include <eid-viewer/macros.h>

typedef struct _slotdesc slotdesc;

/** Describes a slot (reader) */
struct _slotdesc
{
	unsigned long slot;  ///< The number of the slot, to be used with eid_vwr_be_select_slot()
	EID_CHAR *description;	///< A human-readable name for the reader
};

/** Types of data sources we can have */
enum eid_vwr_source
{
	EID_VWR_SRC_NONE,    ///< No source. UI should be cleared.
	EID_VWR_SRC_FILE,    ///< File source. "Print", "Validate" and "Close" operations should be allowed, PIN-related operations are impossible. Save is pointless (but may be possible).
	EID_VWR_SRC_CARD,    ///< Card source. Close should not be allowed, everything else should be.
	EID_VWR_SRC_UNKNOWN, ///< Unknown. Used as initializer.
};

/** \brief Log levels.
 *
 * These are the log levels which the backend may use.
 */
enum eid_vwr_loglevel
{

	EID_VWR_LOG_DETAIL = 0,	/**< most detail */

	EID_VWR_LOG_NORMAL = 1,	/**< intermediate detail */

	EID_VWR_LOG_COARSE = 2,	/**< least detail */

	EID_VWR_LOG_ERROR = 3, /**< error message; a dialog box with the message (in addition to adding to the log) may be appropriate */
};

/** PIN operations. TEST: perform a login (and return whether the login was successful). CHG: change PIN */
enum eid_vwr_pinops
{
	EID_VWR_PINOP_TEST,  ///< perform a login and return whether the login was successful
	EID_VWR_PINOP_CHG,   ///< change the PIN code of the card
};

/** Result of an operation. */
enum eid_vwr_result
{
	EID_VWR_RES_FAILED,  ///< operation failed
	EID_VWR_RES_SUCCESS, ///< the operation returned successfully
	EID_VWR_RES_UNKNOWN  ///< the operation did not finish
};

/** \brief Possible states.
 *
 * \image html be-statemach-docs.png
 */
enum eid_vwr_states
{
	STATE_LIBOPEN,	     ///< The library has been opened. Used as an initializer.
	STATE_CALLBACKS,     ///< The callbacks have been defined.
	STATE_READY,	     ///< Ready to receive a token (eID card)
	STATE_TOKEN,	     ///< A token (eID card) has been found
	STATE_TOKEN_WAIT,    ///< The card has been read, we're now waiting for events to do something else.
	STATE_TOKEN_ID,	     ///< Reading identity data
	STATE_TOKEN_CERTS,   ///< Reading certificates
	STATE_TOKEN_PINOP,   ///< Performing a PIN operation
	STATE_TOKEN_SERIALIZE,	///< Saving data to a file
	STATE_TOKEN_ERROR,   ///< An error occurred while dealing with the card
	STATE_FILE,			 ///< We're dealing with files
	STATE_FILE_READING,  ///< We're reading from the file currently
	STATE_FILE_WAIT,     ///< We finished parsing the file.
	STATE_CARD_INVALID,  ///< The data was determined to be invalid. That is, the card could be read, but signature validation failed.
	STATE_NO_TOKEN,	     ///< We don't have a card, and we also don't have a file.
	STATE_NO_READER,     ///< We don't have a reader (yet?)
	STATE_TOKEN_IDLE,	 ///<not performing any action in the taoken_wait state

	STATE_COUNT,
};

/** Possible languages. */
enum eid_vwr_langs
{
	EID_VWR_LANG_NONE,   ///< No language has yet been selected. Used as initializer.
	EID_VWR_LANG_DE,     ///< German
	EID_VWR_LANG_EN,     ///< English
	EID_VWR_LANG_FR,     ///< French
	EID_VWR_LANG_NL,     ///< Dutch
};


/** \brief Callbacks which the backend can perform towards the UI.
  *
  * To ensure forward compatibility, a UI implementation *must* call
  * eid_vwr_cbstruct() to allocate an eid_vwr_ui_callbacks struct.
  * Failure to do so may result in crashes in future versions of the
  * eid-viewer library.
  *
  * An implementation may leave one or more of the below function
  * pointers empty (i.e., at NULL). If this happens, the backend will
  * simply not issue that event.
  */
struct eid_vwr_ui_callbacks
{

	/** \brief The version number of this struct.
	  *
	  * This version number exists for backwards compatibility
	  * reasons. When the eid_vwr_cbstruct() function is called,
	  * this version number will be set to the latest version
	  * supported by the backend (0 for now). Frontend code should
	  * check the version number. If it is higher than the most
	  * recently supported version at the time of writing of the
	  * frontend code, then the frontend code should change the
	  * version number so it reflects the most recent version it
	  * supports.
	  *
	  * If the version is lower, it should not touch the version
	  * number.
	  */
	int version;

	/** \brief Called when the data source has changed.
	  * 
	  * When the viewer library starts looking at a new file or eID
	  * card, this event is issued. When this happens, a user
	  * interface should clear whatever data it has received from
	  * the backend -- it is not valid anymore.
	  *
	  * \param new_source the type of the new source.
	  */
	void (*newsrc) (enum eid_vwr_source new_source);

	/** \brief new string data to be displayed in UI.
	  *
	  * Data with the given label and which should be interpreted as
	  * a string was found. These may be re-issued (with modified
	  * data) after a call to eid_vwr_convert_set_lang().
	  *
	  * \param label the label (identifier) of the data item. Should
	  * never change.
	  * \param data the data as represented in the currently active
	  * language.
	  * \see beidsdk_card_data.pdf contains the possible labels
	  */
	void (*newstringdata) (const EID_CHAR * label, const EID_CHAR * data);

	/** \brief New binary data to be displayed in UI.
	  *
	  * Data with the given label and which should be interpreted as
	  * a binary blob was found. This will not change when the
	  * language is modified.
	  *
	  * \param label the label (identifier) of the data item. Should
	  * never change.
	  * \param data the data as it is on the card.
	  * \param datalen the length of the data, in bytes.
	  */
	void (*newbindata) (const EID_CHAR * label, const unsigned char *data,
			    int datalen);

	/** \brief Log a string at the given level.
	  *
	  * \param level the level at which to log.
	  * \param line the string to log at the given level.
	  *
	  * \note only one of log() or logv() needs to be implemented;
	  * the backend will use whichever version is available.
	  */
	void (*log) (enum eid_vwr_loglevel level, const EID_CHAR * line);

	/** \brief Log a string using varargs.
	  *
	  * The advantage of having this function (as opposed to log())
	  * is that it allows for allocating a string using whatever
	  * memory management the frontend uses. If this is not
	  * necessary or desirable, implementing log() rather than this
	  * function should suffice.
	  *
	  * \param level the level at which to log.
	  * \param line the printf()-formatted string to log.
	  * \param ap arguments to line's format string.
	  *
	  * \note only one of log() or logv() needs to be implemented;
	  * the backend will use whichever version is available.
	  */
	void (*logv) (enum eid_vwr_loglevel level, const EID_CHAR * line,
		      va_list ap);

	/** \brief State machine transition.
	  *
	  * This event is issued whenever the state machine changes
	  * state. For more information, see the state machine diagram
	  * as documented at struct eid_vwr_states.
	  *
	  * \param new_state the new state we're in right now.
	  */
	void (*newstate) (enum eid_vwr_states new_state);

	/** \brief Return the result of a PIN operation
	  *
	  * When the user interface calls eid_vwr_pinop(), then at some
	  * later point this function may be called with the result of
	  * the requested PIN operation.
	  *
	  * If the state machine is not in the TOKEN_WAIT state when
	  * eid_vwr_pinop() was called, this event will never be fired.
	  *
	  * \param op the operation for which this is a result
	  * \param res the result of the operation
	  */
	void (*pinop_result) (enum eid_vwr_pinops op,
			      enum eid_vwr_result res);

	/** \brief The number of known readers has changed
	  *
	  * \param nreaders the new reader count
	  * \param slots the slot IDs and human-readable descriptions of
	  * all known readers
	  */
	void (*readers_changed) (unsigned long nreaders, slotdesc * slots);
};

/** Struct used by preview handler */
struct eid_vwr_preview
{
	void *imagedata;     ///< JPEG photo of the inspected XML file
	size_t imagelen;     ///< length of imagedata
	int have_data;	     ///< nonzero if there is actually any data
};

/** \brief Perform a PIN operation
  *
  * If the state machine is currently in the TOKEN_WAIT state, issue a
  * PIN operation. In all other cases, does nothing.
  *
  * \param op the operation to perform; EID_VWR_PINOP_TEST to log on as
  * a test to see if the PIN code is valid, or EID_VWR_PINOP_CHG to
  * change the PIN code.
  */
DllExport void eid_vwr_pinop(enum eid_vwr_pinops op);

/** \brief Initialize the callbacks
  * 
  * Must be called by every application which wants to make use of this
  * API, otherwise nothing will happen.
  *
  * \param cb the callback functions which this application implements
  * \return 0 if the callbacks were installed successfully.
  * \note the API assumes that this function is called exactly once for
  * the lifetime of the application. While it should be safe to call it
  * more than once, this is not very well tested and therefore doing so
  * is not recommended.
  */
DllExport int eid_vwr_createcallbacks(struct eid_vwr_ui_callbacks *cb);

/** \brief Create the eid_vwr_ui_callbacks struct.
  *
  * While it is possible, in theory, to create the struct by malloc()ing
  * it manually, doing so will fail when the eid-viewer library is
  * updated and a new callback function is added. For this reason,
  * applications that want to continue working with future versions of
  * the library without compatibility issues *must* use this function to
  * allocate a correctly-sized structure.
  * \return an allocated eid_vwr_ui_callbacks struct, with all members
  * set to NULL.
  */
DllExport struct eid_vwr_ui_callbacks *eid_vwr_cbstruct(void);

/** \brief Create a preview for the given file 
  *
  * This function simply parses the XML file and extracts the embedded
  * JPEG photo. It is useful for an "open file" dialog, or similar.
  * \param filename the full path to the filename from which to extract
  * the photo
  * \return a preview description
  */
DllExport struct eid_vwr_preview *eid_vwr_get_preview(const EID_CHAR *
						      filename);

/** \brief Release the data returned by eid_vwr_get_preview()
  *
  * This function drops the preview data allocated by eid_vwr_get_preview.
  * \param prv The preview data to release
  */
DllExport void eid_vwr_release_preview(struct eid_vwr_preview *prv);

/**
  * \brief Check if there is an event to be handled.
  *
  * Will handle one event and then return. Useful if an event loop
  * exists elsewhere; otherwise, using eid_vwr_be_mainloop() is
  * recommended.
  * returns zero on succes, non-zero when reader list could not be
  * formed or when no UI callback has been set
  */
DllExport int eid_vwr_poll(void);

/**
*\brief Loops over waiting function for pkcs11 card and reader insertion/removal event
*
*Will notify the viewer event handling thread by putting a DEVICE_CHANGED event on the event list
*/
#ifdef WIN32
DWORD WINAPI eid_vwr_wait_for_pkcs11_event_loop(void* val);
#else

/** \brief Loop over eid_vwr_poll().
  *
  * \return never
  */

void* eid_vwr_be_mainloop(void* val) NORETURN;
#endif



/**
  * \brief Save the currently-open data.
  *
  * Returns before the save operation has finished. The state machine
  * will switch to the STATE_TOKEN_SERIALIZE state while data is being
  * saved, and will revert to STATE_TOKEN_WAIT when the operation has
  * finished, or to STATE_TOKEN_ERROR if an error occurs.
  *
  * \param target_file the name of the file in which to save data.
  * \see eid_vwr_ui_callbacks::newstate()
  */
DllExport void eid_vwr_be_serialize(const EID_CHAR * target_file);

/**
  * \brief Open a file.
  *
  * Returns before the file has been opened; reading the file and
  * parsing it is done in a background thread. The immediate effect of
  * calling this function is a state transition to STATE_FILE. If a read
  * or parser error occurs, the state reverts to STATE_NO_TOKEN.
  * 
  * \param source_file the file to read
  */
DllExport void eid_vwr_be_deserialize(const EID_CHAR * source_file);

/**
  * \brief Get the currently-open data in XML form.
  *
  * This method can be called when the state machine is in one of the
  * STATE_FILE or STATE_TOKEN_WAIT states.
  *
  * \return a pointer to the XML-encoded data of the most recently
  * opened card data, or NULL if the state machine is not currently in
  * the right state.
  * \note this data is owned by the eid-viewer library and should *not*
  * be freed by the caller.
  */
DllExport const char *eid_vwr_be_get_xmlform(void);

/** \brief Select the current reader.
  *
  * \param automatic if nonzero, let the backend decide
  * which reader and ignore the value of manualslot.
  * \param manualslot if automatic is zero, ignore all readers except for
  * the one with this as the slot id.
  * \see eid_vwr_ui_callbacks::readers_changed()
  */
DllExport void eid_vwr_be_select_slot(int automatic,
				      unsigned long manualslot);

/** \brief Mark the current data as invalid.
  *
  * If the UI detects that the card data is invalid for some reason,
  * then this method will move the state machine to the
  * STATE_CARD_INVALID state. The backend will *not* perform this
  * operation by itself.
  *
  * This method is only valid if the state machine is currently in
  * STATE_TOKEN or one of its child states.
  *
  * \see 
  *  - eid_vwr_verify_cert() tests whether the certificate chains up to
  *    a known root certificate, and is not revoked
  *  - eid_vwr_check_data_validity() tests whether the data on the card
  *    is signed by the given RRN certificate (but does not check the
  *    RRN certificate itself)
  */
DllExport void eid_vwr_be_set_invalid(void);

/** \brief Close the current file
  *
  * If the current state is STATE_FILE, switch to STATE_NO_TOKEN. In all
  * other cases, does nothing.
  */
DllExport void eid_vwr_close_file(void);

/** \brief Set the backend language.
  *
  * This method sets the current backend language to the given language.
  * If any data has been loaded (i.e., the state machine is in state
  * STATE_FILE or STATE_TOKEN_WAIT), this will also re-issue
  * eid_vwr_ui_callbacks::newstringdata() events for which the
  * representation in the new language may be different.
  *
  * \param which the language to which to switch
  */
DllExport void eid_vwr_convert_set_lang(enum eid_vwr_langs which);

/** \brief Return the backend language.
  *
  * \return the current backend language as previously set by
  * eid_vwr_convert_set_lang(). If no language has previously been set,
  * returns EID_VWR_LANG_NONE.
  */
DllExport enum eid_vwr_langs eid_vwr_convert_get_lang(void);

DllExport void eid_vwr_init_crypto(void);

/**@}*/

#endif
