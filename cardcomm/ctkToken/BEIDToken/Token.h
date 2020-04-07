#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>
#import <CryptoTokenKit/TKSmartCardToken.h>

NS_ASSUME_NONNULL_BEGIN

static const unsigned char kBELPIC_DF_BELPIC[] =    { 0xDF, 0x00 };
static const unsigned char kBELPIC_EF_AuthCert[] =  { 0x50, 0x38 };
static const unsigned char kBELPIC_EF_SignCert[] =  { 0x50, 0x39 };
static const unsigned char kBELPIC_EF_TokenInfo[] = { 0x50, 0x32 };
static const unsigned char kBELPIC_AuthCert[] =     { 0xDF, 0x00, 0x50, 0x38 };
static const unsigned char kBELPIC_SignCert[] =     { 0xDF, 0x00, 0x50, 0x39 };
static const unsigned char kBELPIC_CACert[] =       { 0xDF, 0x00, 0x50, 0x3A };
static const unsigned char kBELPIC_ROOTCACert[] =   { 0xDF, 0x00, 0x50, 0x3B };
static const unsigned char kBELPIC_TokenInfo[] =    { 0xDF, 0x00, 0x50, 0x32 };

#pragma mark TKSmartCard utility extension for sending/receiving TKBERTLVRecord-formatted APDUs

@interface TKSmartCard(BEIDDataFormat)

- (nullable TKTLVRecord *)sendIns:(UInt8)ins p1:(UInt8)p1 p2:(UInt8)p2 request:(nullable TKTLVRecord *)request expectedTag:(TKTLVTag)expectedTag sw:(UInt16 *)sw error:(NSError **)error;
- (nullable NSArray<TKTLVRecord *> *)recordsOfObject:(TKTokenObjectID)objectID error:(NSError **)error;

@end

#pragma mark BEID implementation of TKToken classes

@interface BEIDTokenKeychainKey : TKTokenKeychainKey

- (instancetype)initWithCertificate:(SecCertificateRef)certificateRef objectID:(TKTokenObjectID)objectID certificateID:(TKTokenObjectID)certificateID alwaysAuthenticate:(BOOL)alwaysAuthenticate NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCertificate:(nullable SecCertificateRef)certificateRef objectID:(TKTokenObjectID)objectID NS_UNAVAILABLE;

@property (readonly) TKTokenObjectID certificateID;
@property (readonly) BOOL alwaysAuthenticate;
@property (readonly) UInt8 keyID;

@end

@class BEIDTokenDriver;
@class BEIDToken;
@class BEIDTokenSession;

static const TKTokenOperationConstraint BEIDConstraintPIN = @"PIN";
static const TKTokenOperationConstraint BEIDConstraintPINAlways = @"PINAlways";

@interface BEIDTokenSession : TKSmartCardTokenSession<TKTokenSessionDelegate>
- (instancetype)initWithToken:(TKToken *)token delegate:(id<TKTokenSessionDelegate>)delegate NS_UNAVAILABLE;

- (instancetype)initWithToken:(BEIDToken *)token;
@property (readonly) BEIDToken *BEIDToken;

@end

@interface BEIDToken : TKSmartCardToken<TKTokenDelegate>
- (instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(nullable NSData *)AID tokenDriver:(TKSmartCardTokenDriver *)tokenDriver delegate:(id<TKTokenDelegate>)delegate NS_UNAVAILABLE;

- (nullable instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(nullable NSData *)AID BEIDDriver:(BEIDTokenDriver *)tokenDriver error:(NSError **)error;
@property (readonly) BEIDTokenDriver *driver;

@end

@interface BEIDTokenDriver : TKSmartCardTokenDriver<TKSmartCardTokenDriverDelegate>

@end

NS_ASSUME_NONNULL_END
