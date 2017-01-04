
#import <os/log.h>
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import "TokenSession.h"



@implementation BEIDAuthOperation

- (instancetype)initWithSession:(BEIDTokenSession *)session {
    os_log_error(OS_LOG_DEFAULT, "BEID initWithSession called");
    if (self = [super init]) {
        _session = session;

        self.smartCard = session.smartCard;
        /* TODO: use template when apple implements it
        //4th byte should become 0x20 + PIN length
        const UInt8 template[] = {self.session.smartCard.cla, 0x20, 0x00, 0x01, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        self.APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
        self.PINFormat = [[TKSmartCardPINFormat alloc] init];
        self.PINFormat.PINBitOffset = 5 * 8 - 4;
        self.PINFormat.encoding=TKSmartCardPINEncodingBCD;
        self.PINFormat.minPINLength=4;
        self.PINFormat.maxPINLength=12;
        self.PINFormat.charset=TKSmartCardPINCharsetNumeric;
        self.PINFormat.PINJustification=TKSmartCardPINJustificationLeft;
        //self.PINFormat.PINBlockByteLength
        self.PINFormat.PINLengthBitSize=4; */
    }

    return self;
}

- (uint8_t)pinDigit:(uint8_t) digit
{
    if ('0' <= digit && digit <= '9')
        return digit - '0';
    else if ('A' <= digit && digit <= 'F')
        return digit - 'A' + 0x10;
    else if ('a' <= digit && digit <= 'f')
        return digit - 'a' + 0x10;
    else {
        os_log_error(OS_LOG_DEFAULT, "BEID impossible PIN digit enterred");
        return 0xFF;
    }
}


// Remove this as soon as BEIDAuthOperation implements automatic PIN submission according to APDUTemplate.
- (BOOL)finishWithError:(NSError * _Nullable __autoreleasing *)error {
    os_log_error(OS_LOG_DEFAULT, "BEID finishWithError called");  
    BOOL (^verifyPIN)(NSError**) = ^(NSError** error) {
        if(self.PIN == nil){
            os_log_error(OS_LOG_DEFAULT, "BEID finishWithError called PIN == nil");
            return NO;
        }
        //NSMutableData *PINData = [NSMutableData dataWithLength:8];
        //memset(PINData.mutableBytes, 0xff, PINData.length);
        //[[self.PIN dataUsingEncoding:NSUTF8StringEncoding] getBytes:PINData.mutableBytes length:PINData.length];
        
        //uint8_t *pin;
        uint8_t pin[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        uint64_t PinLength = self.PIN.length;
        if(PinLength < 4 || PinLength > 12){
            os_log_error(OS_LOG_DEFAULT, "BEID PIN length not ok, Pinlength = %llu",PinLength);
            return NO;
        }
        [[self.PIN dataUsingEncoding:NSUTF8StringEncoding] getBytes:pin length:PinLength];
        
        os_log(OS_LOG_DEFAULT, "verify PIN with length %lu", (unsigned long)PinLength);

        // Send VERIFY command to the card.
        UInt16 sw;
        //format PIN
        uint8_t apdu[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        uint32_t offset = 0;
        
        apdu[offset++] = 0x20 + PinLength;
        for (uint32_t ix = 0; ix < PinLength;ix+=2)
        {
            apdu[offset++] = ([self pinDigit:pin[ix]] << 4) + ((ix+1) < PinLength ? [self pinDigit:pin[ix+1]] : [self pinDigit:'F']);
        }

        NSMutableData *PINData = [NSMutableData dataWithBytes:apdu length:8];
        if ([self.smartCard sendIns:0x20 p1:0x00 p2:0x01 data:PINData le:nil sw:&sw error:error] == nil) {
            os_log(OS_LOG_DEFAULT, "verify PIN sendIns failed");
            return NO;
        }
        os_log_error(OS_LOG_DEFAULT, "verify PIN returned sw: 0x%04x", sw);
        if ((sw & 0xff00) == 0x6300) {
            int triesLeft = sw & 0x3f;
            os_log(OS_LOG_DEFAULT, "Failed to verify PIN sw:0x%04x retries: %d", sw, triesLeft);
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                          @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), triesLeft]}];
            }
            return NO;
        }else if (sw == 0x6983) {
            os_log_error(OS_LOG_DEFAULT, "Failed to verify PIN, PIN blocked sw:0x%04x", sw);
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                          @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VPIN_BLOCKED", nil), 0]}];
            }
            return NO;
        }
        else if (sw != 0x9000) {
            os_log_error(OS_LOG_DEFAULT, "Failed to verify PIN sw: 0x%04x", sw);
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                          @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), 0]}];
            }
            return NO;
        }
        return YES;
    };
    
    
    
    BOOL success = [self.smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))verifyPIN];
    if (success){
        // Mark card session sensitive, because we entered PIN into it and no session should access it in this state.
        self.smartCard.sensitive = YES;
        
        // Remember in card context that the card is authenticated.
        //self.session.smartCard.context = @(YES);
        self.smartCard.context = @(YES);
        
        // Mark BEIDTokenSession as freshly authorized.
        self.session.authState = BEIDAuthStateFreshlyAuthorized;
    }
    return success;
}

@end

@implementation BEIDTokenSession

- (instancetype)initWithToken:(BEIDToken *)token {
    return [super initWithToken:token];
}

- (TKTokenAuthOperation *)tokenSession:(TKTokenSession *)session beginAuthForOperation:(TKTokenOperation)operation constraint:(TKTokenOperationConstraint)constraint error:(NSError * _Nullable __autoreleasing *)error {
    os_log_error(OS_LOG_DEFAULT, "BEID beginAuthForOperation called");
    if (![constraint isEqual:BEIDConstraintPIN] && ![constraint isEqual:BEIDConstraintPINAlways]) {
        os_log_error(OS_LOG_DEFAULT, "attempt to evaluate unsupported constraint %@", constraint);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeBadParameter userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"WRONG_CONSTR", nil)}];
        }
        return nil;
    }

    return [[BEIDAuthOperation alloc] initWithSession:self];
}

- (BOOL)tokenSession:(TKTokenSession *)session supportsOperation:(TKTokenOperation)operation usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm {
    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called");
    BEIDTokenKeychainKey *keyItem = (BEIDTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:nil];
    if (keyItem == nil) {
        return NO;
    }
    //NSString* algo = algorithm.description;
    //NSData *bytes = [algo dataUsingEncoding:NSUTF8StringEncoding];
    //os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is %s",[bytes bytes]);
/*if([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA256])
{
    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA256");
}
    if([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA1");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmRSASignatureRaw");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA1])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA1");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA256])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmRSASignatureMessagePKCS1v15SHA1");
    }*/

    switch (operation) {
        case TKTokenOperationSignData:
            os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called TKTokenOperationSignData");
            if (keyItem.canSign) {
                if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
                    // We support only RAW data format and PKCS1 padding.  Once SecKey gets support for PSS padding,
                    // we should add it here.
                    BOOL returnValue = ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw] && [algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw]);
                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation returning %i",returnValue);
                    return returnValue;
                }
            }
        break;

        default:
            os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called default");
            break;
    }
    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation returning NO");
    return NO;
}

- (BEIDTokenKeychainKey *)authenticatedKeyForObjectID:(TKTokenObjectID)keyObjectID error:(NSError **)error {
    // Check for authentication status.
    os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID called");
    BEIDTokenKeychainKey *keyItem = (BEIDTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID keyItem == nil");
        return nil;
    }
    //select key on token
    const uint8_t keyId = keyItem.keyID;//0x82;
    os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID keyId = %ux", keyItem.keyID);
    
    BOOL (^selectKey)(NSError**) = ^(NSError** err) {
        //first select key and algo
        BOOL retVAL = [self selectKeyForSign:keyId smartCard:self.smartCard error:error];
        if (retVAL == NO){
            os_log_error(OS_LOG_DEFAULT, "BEID egeneralAuthenticateWithData selectKeyForSign failed");
            return retVAL;
        }
        return YES;
    };
    
    if (self.authState == BEIDAuthStateUnauthorized ||
        (keyItem.alwaysAuthenticate && self.authState == BEIDAuthStateAuthorizedButAlreadyUsed) ||
        self.smartCard.context == nil) {
        
        BOOL success = [self.smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))selectKey];
        if(success == NO){
            os_log_error(OS_LOG_DEFAULT, "BEID selectKey failed");
            return nil;
        }
        os_log_error(OS_LOG_DEFAULT, "BEID selectKey success");
        
        os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID BEIDAuthStateUnauthorized, setting keyItem = nil");
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationNeeded userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID failed with error %@",[*error localizedDescription]);
        }
        return nil;
    }
    return keyItem;
}

- (BOOL) selectKeyForSign:(const uint8_t)keyId smartCard:(TKSmartCard *)smartCard error:(NSError **)error
{
    os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign called");

    // Select signing algorithm, pkcs1 padding and key keyId
    //unsigned char command[] = { 0x00, 0x22, 0x41, 0xB6, 0x05, 0x04, 0x80, 0x01, 0x84, keyId };
    unsigned char command[] = { 0x04, 0x80, 0x01, 0x84, 0x82 };//1 for unknown hash
    //unsigned char command[] = { 0x04, 0x80, 0x08, 0x84, 0x82 };//8 for sha256
    
    NSData *data = [NSData dataWithBytes:command length:5];
    UInt16 sw = 0;
    [smartCard sendIns:0x22 p1:0x41 p2:0xB6 data:data le:nil sw:&sw error:error];
    if(sw != 0x9000){
        os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign failed with SW: %x", sw);
        os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign failed with error: %@", *error);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign failed with error: %@", *error);
        }
        return NO;
    }
    os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign success");
    return YES;
}

- (BOOL) signData:(NSData *)data result:(NSData **)result resultLength:(NSNumber *)resultLength smartCard:(TKSmartCard *)smartCard error:(NSError **)error
{
    UInt16 sw = 0;

    *result = [smartCard sendIns:0x2A p1:0x9E p2:0x9A data:data le:resultLength sw:&sw error:error];
    
    if(sw != 0x9000){
        os_log_error(OS_LOG_DEFAULT, "BEID signData failed with SW: %x", sw);
        os_log_error(OS_LOG_DEFAULT, "BEID signData failed with error: %@", *error);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID signData failed with error: %@", *error);
        }
        return NO;
    }
    os_log_error(OS_LOG_DEFAULT, "BEID signData result length is %lu", (unsigned long)(*result).length);

    return YES;
}

// Wrapper around GENERAL AUTHENTICATE card command used for sign.
- (NSData *)generalAuthenticateWithData:(NSData *)data session:(BEIDTokenSession *)session algorithm:(TKTokenKeyAlgorithm *)algorithm usingKey:(TKTokenObjectID)keyObjectID error:(NSError **)error {
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData called");
    
    BEIDTokenKeychainKey *keyItem = [self authenticatedKeyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData failed, keyItem == nil");
        return nil;
    }

    NSNumber *resultLen = [NSNumber numberWithUnsignedChar:keyItem.keySizeInBits];
    __block NSData *statResponse;
    
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData original Data to be signed length = %lu, data :",data.length);
    Byte* dataBytes = (Byte*)data.bytes;
    int i;
    for ( i = 0 ; i < data.length ; i++)
    {
        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",i, dataBytes[i]);
    }
    
    //remove the pkcs1 padding 00 01 ff ff ... ff 00
    for ( i = 0 ; i < data.length ; i++)
    {
        if ((i > 2)&&(dataBytes[i]!=0xff))
            break;
    }
    //also remove the ending 0
    i++;
 
    NSData * blockData = [[NSData alloc] initWithBytes:&dataBytes[i] length:((data.length)-(i))];
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData tempered Data to be signed length = %lu, data :",blockData.length);
/*    dataBytes = (Byte*)blockData.bytes;
    for ( i = 0 ; i < blockData.length ; i++)
    {
        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",i, dataBytes[i]);
    }*/

 
    BOOL (^sign)(NSError**) = ^(NSError** err) {

        BOOL retVAL = [self signData:blockData result:&statResponse resultLength:resultLen smartCard:self.smartCard error:error];
        if (retVAL == NO){
            os_log_error(OS_LOG_DEFAULT, "BEID egeneralAuthenticateWithData signData failed");
            return retVAL;
        }
        
        return YES;
    };
    
    BOOL success = [self.smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))sign];
    if(success == NO){
        os_log_error(OS_LOG_DEFAULT, "BEID sign failed");
        return nil;
    }
    
    // Mark BEIDTokenSession as already authorized and authorization used.
    session.authState = BEIDAuthStateAuthorizedButAlreadyUsed;
    
    os_log_error(OS_LOG_DEFAULT, "BEID signData signature length = %lu",(unsigned long)statResponse.length);
/*    Byte* responseBytes = (Byte*)statResponse.bytes;
    int iii;
    for ( iii = 0 ; iii < statResponse.length ; iii++)
    {
        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",iii, responseBytes[iii]);
    }*/
    return statResponse;
    
}

- (NSData *)tokenSession:(TKTokenSession *)session signData:(NSData *)dataToSign usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError * _Nullable __autoreleasing *)error {
    os_log_error(OS_LOG_DEFAULT, "BEID signData called");
    return [self generalAuthenticateWithData:dataToSign session:(BEIDTokenSession *)session algorithm:(TKTokenKeyAlgorithm *)algorithm usingKey:keyObjectID error:error];
}

@end
