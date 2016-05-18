#include <eid-viewer/oslayer.h>

@import Foundation;
@import AppKit;

typedef NS_ENUM(NSInteger, eIDSource) {
    eIDSourceNone = EID_VWR_SRC_NONE,
    eIDSourceFile = EID_VWR_SRC_FILE,
    eIDSourceCard = EID_VWR_SRC_CARD,
    eIDSourceUnknown = EID_VWR_SRC_UNKNOWN,
};

typedef NS_ENUM(NSInteger, eIDLogLevel) {
    eIDLogLevelDetail = EID_VWR_LOG_DETAIL,
    eIDLogLevelNormal = EID_VWR_LOG_NORMAL,
    eIDLogLevelCoarse = EID_VWR_LOG_COARSE,
    eIDLogLevelError = EID_VWR_LOG_ERROR,
};

typedef NS_ENUM(NSInteger, eIDPinOp) {
    eIDPinOpTest = EID_VWR_PINOP_TEST,
    eIDPinOpChange = EID_VWR_PINOP_CHG,
};

typedef NS_ENUM(NSInteger, eIDState) {
    eIDStateLibOpen = STATE_LIBOPEN,
    eIDStateCallbacks = STATE_CALLBACKS,
    eIDStateReady = STATE_READY,
    eIDStateToken = STATE_TOKEN,
    eIDStateTokenWait = STATE_TOKEN_WAIT,
    eIDStateTokenID = STATE_TOKEN_ID,
    eIDStateTokenCerts = STATE_TOKEN_CERTS,
    eIDStateTokenPinop = STATE_TOKEN_PINOP,
    eIDStateTokenSerialize = STATE_TOKEN_SERIALIZE,
    eIDStateTokenError = STATE_TOKEN_ERROR,
    eIDStateFile = STATE_FILE,
    eIDStateCardInvalid = STATE_CARD_INVALID,
    eIDStateNoToken = STATE_NO_TOKEN,
    eIDStateNoReader = STATE_NO_READER,
};

typedef NS_ENUM(NSInteger, eIDLanguage) {
    eIDLanguageNone = EID_VWR_LANG_NONE,
    eIDLanguageDe = EID_VWR_LANG_DE,
    eIDLanguageEn = EID_VWR_LANG_EN,
    eIDLanguageFr = EID_VWR_LANG_FR,
    eIDLanguageNl = EID_VWR_LANG_NL,
};

typedef NS_ENUM(NSInteger, eIDResult) {
    eIDResultFailed = EID_VWR_RES_FAILED,
    eIDResultSuccess = EID_VWR_RES_SUCCESS,
    eIDResultUnknown = EID_VWR_RES_UNKNOWN,
};

// Protocol for implementing a UI.
@protocol eIDOSLayerUI
-(void)newsrc:(eIDSource)which;
-(void)newstringdata:(NSString*)data withLabel:(NSString*)label;
-(void)newbindata:(NSData*)data withLabel:(NSString*)label;
-(void)log:(NSString*)line withLevel:(eIDLogLevel)level;
-(void)newstate:(eIDState)state;
-(void)pinop_result:(eIDResult)result forOperation:(eIDPinOp)operation;
-(void)readersFound:(NSArray*)readers withSlotNumbers:(NSArray*)slots;
@end

// Class method-only class which wraps the corresponding C-only APIs for the
// benefit of swift applications and which does some data conversion (mainly in
// the getPreview method)
@interface eIDOSLayerBackend : NSObject
+(NSInteger)pinop:(eIDPinOp)which;
+(NSInteger)setUi:(id<eIDOSLayerUI>)ui;
+(NSImage*)getPreview:(NSURL*)from;
+(void)setLang:(eIDLanguage)language;
+(eIDLanguage)lang;
+(void)poll;
+(void)mainloop; // does not return
+(void)mainloop_thread;
+(void)deserialize:(NSURL*)from;
+(void)serialize:(NSURL*)to;
+(NSData*)xmlform;
+(void)close_file;
+(void)set_invalid;
+(eIDResult)validateCert:(NSData*)certificate withCa:(NSData*)ca;
+(void)selectReader:(NSInteger)readerNumber;
+(void)setReaderAuto:(BOOL)automatic;
@end
