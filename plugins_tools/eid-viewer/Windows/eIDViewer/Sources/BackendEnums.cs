//TODO: make it so this file gets auto-generated from the C header file

using System;
using System.Runtime.InteropServices;

namespace eIDViewer
{
    /** Describes a slot (reader) */
    [StructLayout(LayoutKind.Sequential)]
    public struct eid_slotdesc
    {
        [MarshalAsAttribute(UnmanagedType.U4)]
        public UInt32 slot; ///< The number of the slot, to be used with eid_vwr_be_select_slot()
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string description; ///< A human-readable name for the reader
    };

    enum MessageLevel
    {
        None,           // Message is ignored
        InfoMessage,    // Message is written to info port.
        InfoWarning,    // Message is written to info port and warning is issued
        Popup           // User is alerted to the message
    };

//certificate status, ordered from trusted to untrusted
    public enum eid_cert_status
    {
        EID_CERT_STATUS_VALID = 0,
        EID_CERT_STATUS_UNKNOWN = 1,
        EID_CERT_STATUS_WARNING = 2,
        EID_CERT_STATUS_INVALID = 3,       
    }

    public enum eid_vwr_source
    {
        EID_VWR_SRC_NONE,
        EID_VWR_SRC_FILE,
        EID_VWR_SRC_CARD,
        EID_VWR_SRC_UNKNOWN,
    };

    public enum eid_vwr_loglevel
    {
        EID_VWR_LOG_DETAIL = 0,
        EID_VWR_LOG_NORMAL = 1,
        EID_VWR_LOG_COARSE = 2,
        EID_VWR_LOG_ERROR = 3,
    };

    public enum eid_vwr_pinops
    {
        EID_VWR_PINOP_TEST,
        EID_VWR_PINOP_CHG,
    };

    /** Possible languages. */
    public enum eid_vwr_langs
    {
        EID_VWR_LANG_NONE, ///< No language has yet been selected. Used as initializer.
        EID_VWR_LANG_DE, ///< German
        EID_VWR_LANG_EN, ///< English
        EID_VWR_LANG_FR, ///< French
        EID_VWR_LANG_NL, ///< Dutch
    };

    public enum eid_vwr_result
    {
        EID_VWR_RES_FAILED,
        EID_VWR_RES_SUCCESS,
	EID_VWR_RES_UNKNOWN,
    };

    public enum eid_vwr_states
    {
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
        STATE_FILE, ///< We're dealing with files
        STATE_FILE_READING, ///< We're reading from the file currently
        STATE_FILE_WAIT, ///< We finished parsing the file.
        STATE_CARD_INVALID, ///< The data was determined to be invalid. That is, the card could be read, but signature validation failed.
        STATE_NO_TOKEN, ///< We don't have a card, and we also don't have a file.
        STATE_NO_READER, ///< We don't have a reader (yet?)
        STATE_TOKEN_IDLE, /// < done reading identity data and certificates
        STATE_TOKEN_CHALLENGE, ///< Performing a challenge operation

        STATE_COUNT,
    };
}
