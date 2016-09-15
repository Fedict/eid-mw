/*
 Copyright (C) 2016 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 Implements PIV token session
 */

#import <os/log.h>
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import "TokenSession.h"



@implementation PIVAuthOperation

- (instancetype)initWithSession:(PIVTokenSession *)session {
    if (self = [super init]) {
        _session = session;

        self.smartCard = session.smartCard;
        const UInt8 template[] = {self.session.smartCard.cla, 0x20, 0x00, 0x80, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        self.APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
        self.PINFormat = [[TKSmartCardPINFormat alloc] init];
        self.PINFormat.PINBitOffset = 5 * 8;
    }

    return self;
}

// Remove this as soon as PIVAuthOperation implements automatic PIN submission according to APDUTemplate.
- (BOOL)finishWithError:(NSError * _Nullable __autoreleasing *)error {
    // Format PIN as UTF-8, right padded with 0xff to 8 bytes.
    NSMutableData *PINData = [NSMutableData dataWithLength:8];
    memset(PINData.mutableBytes, 0xff, PINData.length);
    [[self.PIN dataUsingEncoding:NSUTF8StringEncoding] getBytes:PINData.mutableBytes length:PINData.length];

    // Send VERIFY command to the card.
    UInt16 sw;
    if ([self.session.smartCard sendIns:0x20 p1:0x00 p2:0x80 data:PINData le:nil sw:&sw error:error] == nil) {
        return NO;
    }
    if ((sw & 0xff00) == 0x6300) {
        int triesLeft = sw & 0x3f;
        os_log(OS_LOG_DEFAULT, "Failed to verify PIN sw:0x%04x retries: %d", sw, triesLeft);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                      @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), triesLeft]}];
        }
        return NO;
    } else if (sw != 0x9000) {
        os_log_error(OS_LOG_DEFAULT, "Failed to verify PIN sw: 0x%04x", sw);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                      @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), 0]}];
        }
        return NO;
    }

    // Mark card session sensitive, because we entered PIN into it and no session should access it in this state.
    self.session.smartCard.sensitive = YES;

    // Remember in card context that the card is authenticated.
    self.session.smartCard.context = @(YES);

    // Mark PIVTokenSession as freshly authorized.
    self.session.authState = PIVAuthStateFreshlyAuthorized;
    return YES;
}

@end

@implementation PIVTokenSession

- (instancetype)initWithToken:(PIVToken *)token {
    return [super initWithToken:token];
}

- (TKTokenAuthOperation *)tokenSession:(TKTokenSession *)session beginAuthForOperation:(TKTokenOperation)operation constraint:(TKTokenOperationConstraint)constraint error:(NSError * _Nullable __autoreleasing *)error {
    if (![constraint isEqual:PIVConstraintPIN] && ![constraint isEqual:PIVConstraintPINAlways]) {
        os_log_error(OS_LOG_DEFAULT, "attempt to evaluate unsupported constraint %@", constraint);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeBadParameter userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"WRONG_CONSTR", nil)}];
        }
        return nil;
    }

    return [[PIVAuthOperation alloc] initWithSession:self];
}

- (BOOL)tokenSession:(TKTokenSession *)session supportsOperation:(TKTokenOperation)operation usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm {
    PIVTokenKeychainKey *keyItem = (PIVTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:nil];
    if (keyItem == nil) {
        return NO;
    }

    switch (operation) {
        case TKTokenOperationSignData:
            if (keyItem.canSign) {
                if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
                    // We support only RAW data format and PKCS1 padding.  Once SecKey gets support for PSS padding,
                    // we should add it here.
                    return [algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw] &&
                    [algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw];
                } else if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeECSECPrimeRandom]) {
                    if (keyItem.keySizeInBits == 256) {
                        return [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA256];
                    } else if (keyItem.keySizeInBits == 384) {
                        return [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA384];
                    }
                }
            }
            break;
        case TKTokenOperationDecryptData:
            if (keyItem.canDecrypt && [keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
                return [algorithm isAlgorithm:kSecKeyAlgorithmRSAEncryptionRaw];
            }
            break;
        case TKTokenOperationPerformKeyExchange:
            if (keyItem.canPerformKeyExchange && [keyItem.keyType isEqual:(id)kSecAttrKeyTypeECSECPrimeRandom]) {
                // For NIST p256 and p384, there is no difference between standard and cofactor variants, so answer that both of them are supported.
                return [algorithm isAlgorithm:kSecKeyAlgorithmECDHKeyExchangeStandard] || [algorithm isAlgorithm:kSecKeyAlgorithmECDHKeyExchangeCofactor];
            }
            break;
        default:
            break;
    }
    return NO;
}

- (PIVTokenKeychainKey *)authenticatedKeyForObjectID:(TKTokenObjectID)keyObjectID error:(NSError **)error {
    // Check for authentication status.
    PIVTokenKeychainKey *keyItem = (PIVTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        return nil;
    }

    if (self.authState == PIVAuthStateUnauthorized ||
        (keyItem.alwaysAuthenticate && self.authState == PIVAuthStateAuthorizedButAlreadyUsed) ||
        self.smartCard.context == nil) {
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationNeeded userInfo:nil];
        }
        return nil;
    }
    return keyItem;
}

// Wrapper around GENERAL AUTHENTICATE card command used for sign, decrypt and ECDH.
- (NSData *)generalAuthenticateWithData:(NSData *)data dataTag:(TKTLVTag)dataTag usingKey:(TKTokenObjectID)keyObjectID error:(NSError **)error {
    PIVTokenKeychainKey *keyItem = [self authenticatedKeyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        return nil;
    }

    UInt16 sw;
    TKTLVRecord *request = [[TKBERTLVRecord alloc] initWithTag:0x7c records:@[
                                [[TKBERTLVRecord alloc] initWithTag:dataTag value:[NSData data]],
                                [[TKBERTLVRecord alloc] initWithTag:0x81 value:data]
                            ]];
    self.smartCard.useCommandChaining = YES; //NIST SP 800-78-4 Table 2
    TKTLVRecord *response = [self.smartCard sendIns:0x87 p1:keyItem.algID p2:keyItem.keyID request:request expectedTag:0x7c sw:&sw error:error];
    if (response == nil) {
        return nil;
    }
    TKTLVRecord *dataRecord = [TKBERTLVRecord recordFromData:response.value];
    if (dataRecord.tag != 0x82) {
        os_log_error(OS_LOG_DEFAULT, "%@: crypto with key %@ returned bad formatted response %@", self.token, keyObjectID, response);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeCorruptedData userInfo:nil];
        }
        return nil;
    }
    return dataRecord.value;
}

- (NSData *)tokenSession:(TKTokenSession *)session signData:(NSData *)dataToSign usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError * _Nullable __autoreleasing *)error {
    return [self generalAuthenticateWithData:dataToSign dataTag:0x82 usingKey:keyObjectID error:error];
}

- (NSData *)tokenSession:(TKTokenSession *)session decryptData:(NSData *)ciphertext usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError * _Nullable __autoreleasing *)error {
    return [self generalAuthenticateWithData:ciphertext dataTag:0x82 usingKey:keyObjectID error:error];
}

- (NSData *)tokenSession:(TKTokenSession *)session performKeyExchangeWithPublicKey:(NSData *)otherPartyPublicKeyData usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm parameters:(TKTokenKeyExchangeParameters *)parameters error:(NSError * _Nullable __autoreleasing *)error {
    return [self generalAuthenticateWithData:otherPartyPublicKeyData dataTag:0x85 usingKey:keyObjectID error:error];
}

@end
