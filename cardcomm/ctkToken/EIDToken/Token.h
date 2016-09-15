/*
 Copyright (C) 2016 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 Implements PIV token core
 */


#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>
#import <CryptoTokenKit/TKSmartCardToken.h>

NS_ASSUME_NONNULL_BEGIN


static const unsigned char kBELPIC_EF_AuthCert[] =     { 0xDF, 0x00, 0x50, 0x38 };
static const unsigned char kBELPIC_EF_SignCert[] =     { 0xDF, 0x00, 0x50, 0x39 };
static const unsigned char kBELPIC_EF_TokenInfo[] =    { 0xDF, 0x00, 0x50, 0x32 };

#pragma mark TKSmartCard utility extension for sending/receiving TKBERTLVRecord-formatted APDUs

@interface TKSmartCard(PIVDataFormat)

- (nullable TKTLVRecord *)sendIns:(UInt8)ins p1:(UInt8)p1 p2:(UInt8)p2 request:(nullable TKTLVRecord *)request expectedTag:(TKTLVTag)expectedTag sw:(UInt16 *)sw error:(NSError **)error;
- (nullable NSArray<TKTLVRecord *> *)recordsOfObject:(TKTokenObjectID)objectID error:(NSError **)error;

@end

#pragma mark PIV implementation of TKToken classes

@interface PIVTokenKeychainKey : TKTokenKeychainKey

- (instancetype)initWithCertificate:(SecCertificateRef)certificateRef objectID:(TKTokenObjectID)objectID certificateID:(TKTokenObjectID)certificateID alwaysAuthenticate:(BOOL)alwaysAuthenticate NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCertificate:(nullable SecCertificateRef)certificateRef objectID:(TKTokenObjectID)objectID NS_UNAVAILABLE;

@property (readonly) TKTokenObjectID certificateID;
@property (readonly) BOOL alwaysAuthenticate;
@property (readonly) UInt8 keyID;
@property (readonly) UInt8 algID;

@end

@class PIVTokenDriver;
@class PIVToken;
@class PIVTokenSession;

static const TKTokenOperationConstraint PIVConstraintPIN = @"PIN";
static const TKTokenOperationConstraint PIVConstraintPINAlways = @"PINAlways";

@interface PIVTokenSession : TKSmartCardTokenSession<TKTokenSessionDelegate>
- (instancetype)initWithToken:(TKToken *)token delegate:(id<TKTokenSessionDelegate>)delegate NS_UNAVAILABLE;

- (instancetype)initWithToken:(PIVToken *)token;
@property (readonly) PIVToken *PIVToken;

@end

@interface PIVToken : TKSmartCardToken<TKTokenDelegate>
- (instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(nullable NSData *)AID tokenDriver:(TKSmartCardTokenDriver *)tokenDriver delegate:(id<TKTokenDelegate>)delegate NS_UNAVAILABLE;

- (nullable instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(nullable NSData *)AID PIVDriver:(PIVTokenDriver *)tokenDriver error:(NSError **)error;
@property (readonly) PIVTokenDriver *driver;

@end

@interface PIVTokenDriver : TKSmartCardTokenDriver<TKSmartCardTokenDriverDelegate>
@end

NS_ASSUME_NONNULL_END
