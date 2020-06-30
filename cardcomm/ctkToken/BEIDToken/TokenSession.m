
#import <os/log.h>
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import "TokenSession.h"

#import <Security/SecAsn1Coder.h>






@implementation BEIDAuthOperation

- (instancetype)initWithSession:(BEIDTokenSession *)session {
#ifdef DEBUG
    os_log_info(OS_LOG_DEFAULT, "BEID initWithSession called");
#endif
    if (self = [super init]) {
        _session = session;

        self.smartCard = session.smartCard;
        self.session.authState=BEIDAuthStateUnauthorized;
        // TODO: use template when apple implements it
 /*
        self.smartCard.cla = 0x00;
        self.smartCard.useCommandChaining = NO;
        self.smartCard.useExtendedLength = NO;
        
        const UInt8 template[] = {self.smartCard.cla, 0x20, 0x00, 0x01, 0x08, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        self.APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
        self.PINFormat = [[TKSmartCardPINFormat alloc] init];
        self.PINFormat.PINBlockByteLength = 8;
        self.PINFormat.PINLengthBitOffset = 4;
        self.PINFormat.PINLengthBitSize = 4;
        self.PINFormat.PINBitOffset = 8;
        self.PINFormat.encoding = TKSmartCardPINEncodingBCD;
        self.PINFormat.minPINLength = 4;
        self.PINFormat.maxPINLength = 12;
        self.PINFormat.charset = TKSmartCardPINCharsetNumeric;
        self.PINFormat.PINJustification = TKSmartCardPINJustificationLeft;
*/
        //self.PINByteOffset=5;
#ifdef DEBUG
         os_log_info(OS_LOG_DEFAULT, "BEID new TEST 18");
#endif
 
        BOOL (^verifySecurePIN)(NSError**) = ^(NSError** error) {

            TKSmartCardPINFormat *PINFormat;
            TKSmartCardUserInteractionForSecurePINVerification *userInter;
            NSData *APDUTemplate;
            
            const UInt8 template[] = {0x00, 0x20, 0x00, 0x01, 0x08, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
            PINFormat = [[TKSmartCardPINFormat alloc] init];
            PINFormat.PINBlockByteLength = 8;
            PINFormat.PINLengthBitOffset = 4;
            PINFormat.PINLengthBitSize = 4;
            PINFormat.PINBitOffset = 8;
            PINFormat.encoding = TKSmartCardPINEncodingBCD;
            PINFormat.minPINLength = 4;
            PINFormat.maxPINLength = 12;
            PINFormat.charset = TKSmartCardPINCharsetNumeric;
            PINFormat.PINJustification = TKSmartCardPINJustificationLeft;
            
            // try to Verify PIN on the card reader
            NSData *data = [NSData dataWithBytes:template length:sizeof template];
            // using PINByteOffset:0 (in stead of 5) as requested due to not currently used 
            //(see https://developer.apple.com/documentation/cryptotokenkit/tksmartcard/1390289-userinteractionforsecurepinverif?language=objc)
            userInter = [self.smartCard userInteractionForSecurePINVerificationWithPINFormat:PINFormat APDU:data PINByteOffset:0];
            
            if (nil == userInter)
            {
#ifdef DEBUG
                os_log_error(OS_LOG_DEFAULT, "userInteractionForSecurePINVerificationWithPINFormat returned nil. You are not using a pinpad reader");
#endif
                return NO;
            }
            else
            {
                //reader is supporting secure PIN entry
                dispatch_semaphore_t sema = dispatch_semaphore_create(0);
                NSArray *messages = @[@0];//0 is Pin insertion prompt, and only this message is needed
                userInter.PINMessageIndices = messages;
                userInter.PINCompletion = TKSmartCardPINCompletionKey;//is 2
                
                userInter.initialTimeout = 300;
                userInter.interactionTimeout = 300;
                //using the system Locale by default
                //NSLocale* PINLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"nl_BE"];
                //userInter.locale=PINLocale;
                
                
#ifdef DEBUG
                os_log_info(OS_LOG_DEFAULT, "Enter the PIN on the pinpad");
#endif
               [userInter runWithReply:^(BOOL success, NSError *error)
                 {
                     if (success)
                     {
#ifdef DEBUG
                         NSLog(@"resultData: %@", [userInter resultData]);
                         NSLog(@"resultSW: %04X", [userInter resultSW]);
                         NSLog(@"Smartcard reader name: %@", self.session.smartCard.slot.name);
#endif
                         self.session.authState=BEIDAuthStateFreshlyAuthorized;

                         // Mark card session sensitive, because we entered PIN into it and no session should access it in this state.
                         self.smartCard.sensitive = YES;
                         
                         // Remember in card context that the card is authenticated.
                         self.smartCard.context = @(YES);

                         // Mark BEIDTokenSession as freshly authorized.
                         self.session.authState = BEIDAuthStateFreshlyAuthorized;
                     }
                     else
                     {
#ifdef DEBUG
                         NSLog(@"Failure enterring PIN on cardreader");
                         NSLog(@"Error: %@", error);
#endif
                         self.session.authState=BEIDAuthStateUnauthorized;
                     }
                     dispatch_semaphore_signal(sema);
                 }];
                //wait max 30 seconds for user to enter PIN on secure PIN pad reader
                dispatch_time_t  waitTime = dispatch_time(DISPATCH_TIME_NOW,  300000000000);
                dispatch_semaphore_wait(sema ,waitTime);
#ifdef DEBUG
                os_log_info(OS_LOG_DEFAULT, "PIN handled by reader");
                //NSLog(@"PIN handled by reader");
#endif
                //some secure PIN pad readers support both secure and on PC PIN entries
                //when secure failed, try on PC as backup
                if(self.session.authState==BEIDAuthStateUnauthorized){
                    return NO;
                }
                return YES;
            }
        };
        
        NSError *error ;
        //check if smard card reader has PIN support
        BOOL success = [self.smartCard inSessionWithError:(NSError **)&error executeBlock:(BOOL(^)(NSError **error))verifySecurePIN];
        if (success == YES){
#ifdef DEBUG
            os_log_error(OS_LOG_DEFAULT, "BEID initWithSession return nil");
#endif
            return nil;
        }
    
    }
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID initWithSession return self");
#endif
    return self;
}

- (uint8_t)pinDigit:(uint8_t) digit
{
    if ('0' <= digit && digit <= '9')
        return digit - '0';
    else if ('A' <= digit && digit <= 'F')
        return digit - 'A' + 0x0A;
    else if ('a' <= digit && digit <= 'f')
        return digit - 'a' + 0x0A;
    else {
        os_log_error(OS_LOG_DEFAULT, "BEID impossible PIN digit enterred");
        return 0xFF;
    }
}

- (BOOL)finishWithError:(NSError * _Nullable __autoreleasing *)error {
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID finishWithError called");
#endif

    //use this if you want to use the auto template set in initWithSession()
    /*
    if(self.session.authState == BEIDAuthStateFreshlyAuthorized){
        return YES;
    }
    
    BOOL (^autoVerify)(NSError**) = ^(NSError** error) {
        if(![super finishWithError:error]){
            return NO;
        }
        self.session.smartCard.context=@(YES);
        self.session.authState = BEIDAuthStateFreshlyAuthorized;
        
        return YES;
    };
    
    BOOL success = [self.smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))autoVerify];
    return success;
*/
 
    BOOL (^verifyPIN)(NSError**) = ^(NSError** error) {
        if(self.PIN == nil){
            os_log_error(OS_LOG_DEFAULT, "BEID finishWithError called PIN == nil");
            return NO;
        }

        uint8_t pin[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        uint64_t PinLength = self.PIN.length;
        if(PinLength < 4 || PinLength > 12){
            os_log_error(OS_LOG_DEFAULT, "BEID PIN length not ok, Pinlength = %llu",PinLength);
            return NO;
        }
        [[self.PIN dataUsingEncoding:NSUTF8StringEncoding] getBytes:pin length:PinLength];
        
#ifdef DEBUG
        os_log(OS_LOG_DEFAULT, "verify PIN with length %lu", (unsigned long)PinLength);
#endif

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
#ifdef DEBUG
        os_log_error(OS_LOG_DEFAULT, "verify PIN returned sw: 0x%04x", sw);
#endif
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
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID beginAuthForOperation called");
#endif
    if (![constraint isEqual:BEIDConstraintPIN] && ![constraint isEqual:BEIDConstraintPINAlways]) {
        os_log_error(OS_LOG_DEFAULT, "attempt to evaluate unsupported constraint %@", constraint);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeBadParameter userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"WRONG_CONSTR", nil)}];
        }
        return nil;
    }

    
    TKTokenAuthOperation * tokenAuth = [[BEIDAuthOperation alloc] initWithSession:self];
    if(tokenAuth == nil)
    {
        return [[TKTokenAuthOperation alloc] init];
    }
    
    return tokenAuth;
}

- (BOOL)tokenSession:(TKTokenSession *)session supportsOperation:(TKTokenOperation)operation usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm {
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called");
#endif
    BEIDTokenKeychainKey *keyItem = (BEIDTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:nil];
    if (keyItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation returning NO, keyItem == nil");
        return NO;
    }
    //NSString* algo = algorithm.description;
    //NSData *bytes = [algo dataUsingEncoding:NSUTF8StringEncoding];
    //os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is %s",[bytes bytes]);
/*if([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA256])
{
    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmRSASignatureDigestPKCS1v15SHA256");
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
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureRFC4754])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmECDSASignatureRFC4754");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureRFC4754])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureRFC4754");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, isAlgorithm kSecKeyAlgorithmECDSASignatureDigestX962");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureDigestX962");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA1])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmECDSASignatureDigestX962SHA1");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA224])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmECDSASignatureDigestX962SHA224");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA256])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmECDSASignatureDigestX962SHA256");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA384])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureDigestX962SHA384");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA512])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureDigestX962SHA512");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureMessageX962SHA1])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureMessageX962SHA1");
    }
    if([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureMessageX962SHA224])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, algoritm to check is kSecKeyAlgorithmECDSASignatureMessageX962SHA224");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureMessageX962SHA256])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureMessageX962SHA256");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureMessageX962SHA384])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureMessageX962SHA384");
    }
    if([algorithm supportsAlgorithm:kSecKeyAlgorithmECDSASignatureMessageX962SHA512])
    {
        os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called, supportsAlgorithm kSecKeyAlgorithmECDSASignatureMessageX962SHA512");
    }*/
    
    switch (operation) {
        case TKTokenOperationSignData:
#ifdef DEBUG
            os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation called TKTokenOperationSignData");
#endif
            if (keyItem.canSign) {
                if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
                    // We support only RAW data format and PKCS1 padding.  Once SecKey gets support for PSS padding,
                    // we should add it here.
                    BOOL returnValue = ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw] && [algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw]);
#ifdef DEBUG
                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation RSA returning %i",returnValue);
                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation RSA description %s",algorithm.description.UTF8String);
#endif
                    return returnValue;
                }
                if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeECDSA]) {
                    
                    BOOL returnValue = ([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureRFC4754] ||
                    [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962] ||
                    [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA256] ||
                    [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA384] ||
                    [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA512]);
     
#ifdef DEBUG
                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation ECDSA class name %s",algorithm.className.UTF8String);
                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation ECDSA description %s",algorithm.description.UTF8String);
                    

                    os_log_error(OS_LOG_DEFAULT, "BEID supportsOperation ECDSA returning %i",returnValue);
#endif
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
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID called");
#endif
    BEIDTokenKeychainKey *keyItem = (BEIDTokenKeychainKey *)[self.token.keychainContents keyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID keyItem == nil");
        return nil;
    }
    //select key on token
    const uint8_t keyId = keyItem.keyID;//0x82 0r 0x83;
    uint8_t hashAlgId = 0x01;
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID keyId = %ux", keyItem.keyID);
#endif
    
    if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeECDSA] ||
        [keyItem.keyType isEqual:(id)kSecAttrKeyTypeEC] ||
        [keyItem.keyType isEqual:(id)kSecAttrKeyTypeECSECPrimeRandom])
    {
        hashAlgId = 0x40;
    }
    
    BOOL (^selectKey)(NSError**) = ^(NSError** error) {
        //first select key and algo
        BOOL retVAL = [self selectKeyForSign:keyId algId:hashAlgId smartCard:self.smartCard error:error];
        if (retVAL == NO){
            os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID selectKeyForSign failed");
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
#ifdef DEBUG
        os_log_error(OS_LOG_DEFAULT, "BEID selectKey success");
        os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID BEIDAuthStateUnauthorized, setting keyItem = nil");
#endif
        //TODO: move to the (succes == NO section)
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationNeeded userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID authenticatedKeyForObjectID failed with error %@",[*error localizedDescription]);
        }
        return nil;
    }
    return keyItem;
}

- (BOOL) selectKeyForSign:(const uint8_t)keyId algId:(const uint8_t)algId smartCard:(TKSmartCard *)smartCard error:( NSError **)error
{
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign called");
#endif
    // Select signing algorithm, pkcs1 padding and key keyId
    //unsigned char command[] = { 0x00, 0x22, 0x41, 0xB6, 0x05, 0x04, 0x80, 0x01, 0x84, keyId };
    
    unsigned char command[] = { 0x04, 0x80, algId, 0x84, keyId };//40 for unknown hash, EC
    
    //unsigned char command[] = { 0x04, 0x80, 0x08, 0x84, keyId };//8 for sha256
    
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
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID selectKeyForSign success");
#endif
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
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID signData result length is %lu", (unsigned long)(*result).length);
#endif
    return YES;
}

// Wrapper around GENERAL AUTHENTICATE card command used for sign.
- (NSData *)generalAuthenticateWithData:(NSData *)data session:(BEIDTokenSession *)session algorithm:(TKTokenKeyAlgorithm *)algorithm usingKey:(TKTokenObjectID)keyObjectID error:(NSError **)error {
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData called");
#endif
    BEIDTokenKeychainKey *keyItem = [self authenticatedKeyForObjectID:keyObjectID error:error];
    if (keyItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData failed, keyItem == nil");
        return nil;
    }

    NSNumber *resultLen = [NSNumber numberWithUnsignedChar:96];
//keyItem.keySizeInBits *2 for EC
    __block NSData *statResponse;
    
    Byte* dataBytes = (Byte*)data.bytes;
    int i = 0;
    
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData original Data to be signed length = %lu, data :",data.length);
    for ( i = 0 ; i < data.length ; i++)
    {
        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",i, dataBytes[i]);
    }
#endif
    
      if ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw] && [algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw])
          {
              resultLen = [NSNumber numberWithUnsignedChar:keyItem.keySizeInBits];
              //remove the pkcs1 padding 00 01 ff ff ... ff 00
              for ( i = 0 ; i < data.length ; i++)
              {
                  if ((i > 2)&&(dataBytes[i]!=0xff))
                      break;
              }
              //also remove the ending 0
              i++;
          }
    NSData * blockData = [[NSData alloc] initWithBytes:&dataBytes[i] length:((data.length)-(i))];
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData tempered Data to be signed length = %lu, data :",blockData.length);
#endif
/*    dataBytes = (Byte*)blockData.bytes;
    for ( i = 0 ; i < blockData.length ; i++)
    {
        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",i, dataBytes[i]);
    }*/

 
    BOOL (^sign)(NSError**) = ^(NSError** error) {

        BOOL retVAL = [self signData:blockData result:&statResponse resultLength:resultLen smartCard:self.smartCard error:error];
        if (retVAL == NO){
            os_log_error(OS_LOG_DEFAULT, "BEID generalAuthenticateWithData signData failed");
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
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID signData signature length = %lu",(unsigned long)statResponse.length);
#endif
//   Byte* responseBytes = (Byte*)statResponse.bytes;
//    int iii;
//    for ( iii = 0 ; iii < statResponse.length ; iii++)
//    {
//        os_log_error(OS_LOG_DEFAULT, "%d: 0x%x ",iii, responseBytes[iii]);
//    }
    
    if ([algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962] ||
        [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA1] ||
        [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA224] ||
        [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA256] ||
        [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA384] ||
        [algorithm isAlgorithm:kSecKeyAlgorithmECDSASignatureDigestX962SHA512])
    {
#ifdef DEBUG
        os_log_error(OS_LOG_DEFAULT, "BEID package signature into ASN1 sequence");
#endif
       
        typedef struct {
            SecAsn1Item r;
            SecAsn1Item s;
        } ECDSA;
        
        static const SecAsn1Template ECDSATemplate[] = {
            { SEC_ASN1_SEQUENCE, 0, nil, sizeof(ECDSA) },
            { SEC_ASN1_INTEGER, offsetof(ECDSA, r) },
            { SEC_ASN1_INTEGER, offsetof(ECDSA, s) },
            { 0 }                                           //marks the end of the template
        };
        
        uint8 *bytes = (uint8*)statResponse.bytes;
        
        ECDSA ecdsa = {
            { statResponse.length / 2, bytes },
            { statResponse.length / 2, bytes + (statResponse.length / 2) },
        };
        
        SecAsn1CoderRef coder;
        SecAsn1CoderCreate(&coder);
    
        SecAsn1Item dest = {0, nil};
        OSStatus ortn = SecAsn1EncodeItem(coder, &ecdsa, ECDSATemplate, &dest);
        
        statResponse = [NSData dataWithBytes:dest.Data length:dest.Length];

        SecAsn1CoderRelease(coder);
        //NSLog(@"TokenSession SecAsn1EncodeItem %i %@", ortn, statResponse);
    }
    
    return statResponse;
}

- (NSData *)tokenSession:(TKTokenSession *)session signData:(NSData *)dataToSign usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError * _Nullable __autoreleasing *)error {
#ifdef DEBUG
    os_log_error(OS_LOG_DEFAULT, "BEID signData called");
#endif
    return [self generalAuthenticateWithData:dataToSign session:(BEIDTokenSession *)session algorithm:(TKTokenKeyAlgorithm *)algorithm usingKey:keyObjectID error:error];
}

@end
