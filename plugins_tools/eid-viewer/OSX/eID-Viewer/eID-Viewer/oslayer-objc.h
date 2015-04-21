#include <oslayer.h>

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
};

// Protocol for implementing a UI.
@protocol eIDOSLayerUI
-(void)newsrc:(eIDSource)which;
-(void)newstringdata:(NSString*)data withLabel:(NSString*)label;
-(void)newbindata:(NSData*)data withLabel:(NSString*)label;
-(void)log:(NSString*)line withLevel:(eIDLogLevel)level;
-(void)newstate:(eIDState)state;
@end

// Class method-only class which wraps the corresponding C-only APIs for the
// benefit of swift applications and which does some data conversion (mainly in
// the getPreview method)
@interface eIDOSLayerBackend : NSObject
+(NSInteger)pinop:(eIDPinOp)which;
+(NSInteger)setUi:(id<eIDOSLayerUI>)ui;
+(NSImage*)getPreview:(NSURL*)from;
+(void)poll;
+(void)mainloop; // does not return
+(void)mainloop_thread;
@end
