#include <BeidView/oslayer.h>

/**
 * \defgroup ObjC Objective-C wrapper API (OSX only)
 *
 * @{
 */

/** \file oslayer-objc.h */

@import Foundation;
@import AppKit;

typedef NS_ENUM(NSInteger, eIDSource)
{
	eIDSourceNone = EID_VWR_SRC_NONE,
	eIDSourceFile = EID_VWR_SRC_FILE,
	eIDSourceCard = EID_VWR_SRC_CARD,
	eIDSourceUnknown = EID_VWR_SRC_UNKNOWN,
};

typedef NS_ENUM(NSInteger, eIDLogLevel)
{
	eIDLogLevelDetail = EID_VWR_LOG_DETAIL,
	eIDLogLevelNormal = EID_VWR_LOG_NORMAL,
	eIDLogLevelCoarse = EID_VWR_LOG_COARSE,
	eIDLogLevelError = EID_VWR_LOG_ERROR,
};

typedef NS_ENUM(NSInteger, eIDPinOp)
{
	eIDPinOpTest = EID_VWR_PINOP_TEST, eIDPinOpChange = EID_VWR_PINOP_CHG,
};

typedef NS_ENUM(NSInteger, eIDState)
{
	eIDStateLibOpen = STATE_LIBOPEN,
	eIDStateCallbacks = STATE_CALLBACKS,
	eIDStateReady = STATE_READY,
	eIDStateToken = STATE_TOKEN,
	eIDStateTokenWait = STATE_TOKEN_WAIT,
	eIDStateTokenID = STATE_TOKEN_ID,
	eIDStateTokenCerts = STATE_TOKEN_CERTS,
	eIDStateTokenIdle = STATE_TOKEN_IDLE,
	eIDStateTokenPinop = STATE_TOKEN_PINOP,
	eIDStateTokenSerialize = STATE_TOKEN_SERIALIZE,
	eIDStateTokenError = STATE_TOKEN_ERROR,
	eIDStateFile = STATE_FILE,
	eIDStateFileReading = STATE_FILE_READING,
	eIDStateFileWait = STATE_FILE_WAIT,
	eIDStateCardInvalid = STATE_CARD_INVALID,
	eIDStateNoToken = STATE_NO_TOKEN,
	eIDStateNoReader = STATE_NO_READER,
};

typedef NS_ENUM(NSInteger, eIDLanguage)
{
	eIDLanguageNone = EID_VWR_LANG_NONE,
	eIDLanguageDe = EID_VWR_LANG_DE,
	eIDLanguageEn = EID_VWR_LANG_EN,
	eIDLanguageFr = EID_VWR_LANG_FR,
	eIDLanguageNl = EID_VWR_LANG_NL,
};

typedef NS_ENUM(NSInteger, eIDResult)
{
	eIDResultFailed = EID_VWR_RES_FAILED,
	eIDResultSuccess = EID_VWR_RES_SUCCESS,
	eIDResultUnknown = EID_VWR_RES_UNKNOWN,
	eIDResultWarning = EID_VWR_RES_WARNING,
};

/** \brief Protocol for implementing a UI.
 * \see cbstruct for details on what each method does; these methods are
 * straightforward C-to-ObjC translations */
@protocol eIDOSLayerUI <NSObject>
@optional
-(void) newsrc:(eIDSource) which;						///< called by eid_vwr_ui_callbacks::newsrc()
-(void) newstringdata: (NSString *) data /**< . */ withLabel:(NSString *)label; ///< called by eid_vwr_ui_callbacks::newstringdata()
-(void) newbindata:(NSData *) data withLabel:(NSString *) label;		///< called by eid_vwr_ui_callbacks::newbindata()
-(void) log:(NSString *) line withLevel:(eIDLogLevel) level;			///< called by eid_vwr_ui_callbacks::log()
-(void) newstate:(eIDState) state;						///< called by eid_vwr_ui_callbacks::newstate()
-(void) pinop_result:(eIDResult) result forOperation:(eIDPinOp) operation;	///< called by eid_vwr_ui_callbacks::pinop_result()
-(void) readersFound:(NSArray *) readers withSlotNumbers:(NSArray *) slots;	///< called by eid_vwr_ui_callbacks::readers_changed()
@end

/** Class method-only class which wraps the corresponding C-only APIs for the
 benefit of ObjC applications. */
@interface eIDOSLayerBackend: NSObject
+(void) pinop:(eIDPinOp) which;							///< calls eid_vwr_pinop()
+(NSInteger) setUi:(id < eIDOSLayerUI >) ui;					///< calls eid_vwr_ui_callbacks() and eid_vwr_createcallbacks()
+(NSImage *) getPreview:(NSURL *) from;						///< calls eid_vwr_get_preview(), and converts the result to an NSImage*
+(void) setLang:(eIDLanguage) language;						///< calls eid_vwr_convert_set_lang()
+(eIDLanguage) lang;								///< calls eid_vwr_convert_get_lang()
+(void) poll;									///< calls eid_vwr_poll()
+(void) mainloop;								///< calls eid_vwr_be_mainloop(), which does not return
+(void) deserialize:(NSURL *) from;						///< calls eid_vwr_be_deserialize()
+(void) serialize:(NSURL *) to;							///< calls eid_vwr_be_serialize()
+(NSData *) xmlform;								///< calls eid_vwr_be_get_xmlform(), and converts the result to an NSData*
+(void) closeFile;								///< calls eid_vwr_close_file()
+(void) setInvalid;								///< calls eid_vwr_be_set_invalid()
+(eIDResult) validateCert:(NSData *) certificate withCa:(NSData *) ca;		///< calls eid_vwr_verify_cert() with valid perform_ocsp_request and free_ocsp_request function pointers
+(eIDResult) validateRrnCert:(NSData *) certificate withRoot:(NSData *) root;
+(eIDResult) validateRootCert:(NSData *) certificate;				///< calls eid_vwr_verify_root_cert()
+(eIDResult) validateIntCert:(NSData *) certificate withCa:(NSData *)ca;	///< calls eid_vwr_verify_int_cert() with valid perform_http_request and free_http_request function pointers
+(void) selectReader:(NSInteger) readerNumber;					///< calls eid_vwr_be_select_slot() with 0 as the first parameter, and the given reader number as the second parameter.
+(void) setReaderAuto:(BOOL) automatic;						///< calls eid_vwr_be_select_slot with nonzero (if YES) or zero (if NO) as the first parameter, and 0 as the second parameter.
+(NSString *) getCertDetail:(NSData *) certificate;
@end

/**@}*/
