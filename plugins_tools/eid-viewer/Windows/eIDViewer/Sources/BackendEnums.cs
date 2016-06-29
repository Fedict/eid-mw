//TODO: make it so this file gets auto-generated from the C header file

namespace eIDViewer
{
    enum MessageLevel
    {
        None,           // Message is ignored
        InfoMessage,    // Message is written to info port.
        InfoWarning,    // Message is written to info port and warning is issued
        Popup           // User is alerted to the message
    };

    public enum eid_cert_status
    {
        EID_CERT_STATUS_UNKNOWN,
        EID_CERT_STATUS_INVALID,
        EID_CERT_STATUS_VALID,
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

        STATE_COUNT,
    };
}
