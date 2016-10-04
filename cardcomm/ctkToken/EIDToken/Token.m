

#import <os/log.h>
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import "NSData_Zip.h"

@implementation NSData(hexString)

- (NSString *)hexString {

    NSUInteger capacity = self.length * 2;
    NSMutableString *stringBuffer = [NSMutableString stringWithCapacity:capacity];
    const unsigned char *dataBuffer = self.bytes;

    for (NSInteger i = 0; i < self.length; i++) {
        [stringBuffer appendFormat:@"%02lX", (unsigned long)dataBuffer[i]];
    }

    return stringBuffer;
}

@end

@implementation PIVTokenKeychainKey

- (instancetype)initWithCertificate:(SecCertificateRef)certificateRef objectID:(TKTokenObjectID)objectID certificateID:(TKTokenObjectID)certificateID alwaysAuthenticate:(BOOL)alwaysAuthenticate {
    if (self = [super initWithCertificate:certificateRef objectID:objectID]) {
        _certificateID = certificateID;
        _alwaysAuthenticate = alwaysAuthenticate;
    }
    return self;
}

- (UInt8)keyID {
    return [self.objectID unsignedCharValue];
}

- (UInt8)algID {
    //SP 800-78-4 Table 6-2 and 6-3
    if ([self.keyType isEqual:(id)kSecAttrKeyTypeECSECPrimeRandom]) {
        switch (self.keySizeInBits) {
            case 256:
                return 0x11; //EC 256
            case 384:
                return 0x14; //EC 384
        }
    } else if ([self.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
        switch (self.keySizeInBits) {
            case 1024:
                return 0x06;    //RSA 1024
            case 2048:
                return 0x07;    //RSA 2048
        }
    }
    return 0;
}

@end

@implementation TKTokenKeychainItem(PIVDataFormat)

- (void)setName:(NSString *)name {
    if (self.label != nil) {
        self.label = [NSString stringWithFormat:@"%@ (%@)", name, self.label];
    } else {
        self.label = name;
    }
}

@end


@implementation PIVToken

- (BOOL)selectAbsFile:(const unsigned char[]) absFileId len:(UInt8) len smartCard:(TKSmartCard *)smartCard error:(NSError **)error
{
    //select file with absolute path
    //select certificate file:
    //the length of the data serves as Lc field of the APDU
    os_log_error(OS_LOG_DEFAULT, "BEID selectFile called");
    //select authentication certificate
    NSData *data = [NSData dataWithBytes:absFileId length:len];
    UInt16 sw = 0;
    [smartCard sendIns:0xA4 p1:0x08 p2:0x0C data:data le:nil sw:&sw error:error];
    if(sw != 0x9000){
        os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with SW: %d", sw);
        os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with error: %@", *error);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with error: %@", *error);
        }
        return NO;
    }
    return YES;
}

- (BOOL)selectRelFile:(const unsigned char[]) relFileId len:(UInt8) len smartCard:(TKSmartCard *)smartCard error:(NSError **)error
{
    //select file with ID
    //select certificate file:
    //the length of the data serves as Lc field of the APDU
    os_log_error(OS_LOG_DEFAULT, "BEID selectRelFile called");
    //select authentication certificate
    NSData *data = [NSData dataWithBytes:relFileId length:len];
    UInt16 sw = 0;
    [smartCard sendIns:0xA4 p1:0x02 p2:0x0C data:data le:nil sw:&sw error:error];
    if(sw != 0x9000){
        os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with SW: %x", sw);
        os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with error: %@", *error);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            os_log_error(OS_LOG_DEFAULT, "BEID selectFile failed with error: %@", *error);
        }
        return NO;
    }
    return YES;
}

- (nullable NSData *)readBinary:(TKSmartCard *)smartCard error:(NSError **)error
{
    os_log_error(OS_LOG_DEFAULT, "BEID readBinary called");
    // Attempt to read 250 bytes
    UInt16 sw = 0;
    UInt16 offset = 0;
    NSMutableData *fileData;
    
    NSData *replyData = [smartCard sendIns:0xB0 p1:0x00 p2:0x00 data:nil le:@250 sw:&sw error:error];
    os_log_error(OS_LOG_DEFAULT, "BEID readBinary returned %lu bytes; SW=0x%x",(unsigned long)replyData.length, sw);
    
    if (replyData != nil) {
        os_log_error(OS_LOG_DEFAULT,"replyData = %@", replyData);
        
        while ((replyData.length > 0) && (sw = 0x9000)) {
            os_log_error(OS_LOG_DEFAULT, "BEID readBinary returned %lu bytes; SW=0x%x",(unsigned long)replyData.length, sw);
            offset += replyData.length;
            if(fileData.length == 0){
                fileData = [replyData mutableCopy];
            }
            else {
                    [fileData appendData:replyData];
            }
            
            if(replyData.length == 250)
            {
                os_log_error(OS_LOG_DEFAULT, "BEID readBinary reading with offset p1 = %x p2 = %x",(offset&0xFF00)>>8, offset&0x00FF);
                replyData = [smartCard sendIns:0xB0 p1:(offset&0xFF00)>>8 p2:offset&0x00FF data:nil le:@250 sw:&sw error:error];
                os_log_error(OS_LOG_DEFAULT, "BEID readBinary fileData length is %lu bytes",(unsigned long)fileData.length);

                if (replyData == nil) {
                    break;
                }
            }
            else{
                //we received less then asked for, assuming we reached end of file
                break;
            }
        }
    }
   
    if ( (sw&0xFF00) == 0x6100 || (sw&0xFF00) == 0x6c00 ){
         //we asked for an incorrect length of data, the correct length to ask for resides in SW2
        os_log_error(OS_LOG_DEFAULT, "_BEID readBinary returned %lu bytes; SW=0x%x; offset = %hu",(unsigned long)replyData.length, sw, offset);
        NSNumber *sw2 = [NSNumber numberWithInt:(sw & 0xFF)];
        replyData = [smartCard sendIns:0xB0 p1:(offset&0xFF00)>>8 p2:offset&0x00FF data:nil le:sw2 sw:&sw error:error];
        if (replyData != nil) {
            os_log_error(OS_LOG_DEFAULT, "-_BEID readBinary returned %lu bytes; SW=0x%x",(unsigned long)replyData.length, sw);
            if(fileData.length == 0){
                fileData = [replyData mutableCopy];
            }
            else {
                [fileData appendData:replyData];
            }
        }
    }
    os_log_error(OS_LOG_DEFAULT, "__BEID readBinary returned %lu bytes; SW=0x%x",(unsigned long)fileData.length, sw);
    return fileData;
}

- (nullable NSData *)dataOfCertificate:(UInt16)certificateObjectID smartCard:(TKSmartCard *)smartCard error:(NSError * _Nullable __autoreleasing *)error {

    // Read authentication certificate
    os_log_error(OS_LOG_DEFAULT, "BEID dataOfCertificate called");
    __block NSData *certificateData;
    const unsigned char *absFileId;
    
    switch(certificateObjectID){
    case 0x5038:
        absFileId = kBELPIC_AuthCert;
        break;
    case 0x5039:
        absFileId = kBELPIC_SignCert;
        break;
    case 0x503A:
        absFileId = kBELPIC_CACert;
        break;
    case 0x503B:
        absFileId = kBELPIC_ROOTCACert;
        break;
    default:
        return nil;
    }
    
    BOOL (^readCertData)(NSError**) = ^(NSError** err) {

        BOOL selectFile = [self selectAbsFile:absFileId len:4 smartCard:smartCard error:error];
        if (selectFile == NO){
            return NO;
        }
        
        certificateData = [self readBinary:smartCard error:error];

        if (certificateData == nil) {
            os_log_error(OS_LOG_DEFAULT, "BEID readCertData certificateData == nil");
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            }
            return NO;
        }
        return YES;
    };
    
    BOOL success = [smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))readCertData];
    if(success == NO){
        os_log_error(OS_LOG_DEFAULT, "BEID dataOfCertificate failed");
        certificateData = nil;
    }
    return certificateData;
}

- (BOOL)populateIdentityFromSmartCard:(TKSmartCard *)smartCard into:(NSMutableArray<TKTokenKeychainItem *> *)items certificateTag:(UInt16)certificateTag name:(NSString *)certificateName keyTag:(UInt16)keyTag name:(NSString *)keyName sign:(BOOL)sign keyManagement:(BOOL)keyManagement alwaysAuthenticate:(BOOL)alwaysAuthenticate error:(NSError **)error {
    // Read certificate data.
    os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard called");
    TKTokenObjectID certificateID = @(certificateTag) ;//[TKBERTLVRecord dataForTag:certificateTag];
    NSData *certificateData = [self dataOfCertificate:certificateTag smartCard:smartCard error:error];
    if (certificateData == nil) {
        // If certificate cannot be found, just silently skip the operation, otherwise report an error.
        os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard: no certificate data retrieved");
        return (error != nil && [(*error).domain isEqual:TKErrorDomain] && (*error).code == TKErrorCodeObjectNotFound);
    }

    // Create certificate item.
    id certificate = CFBridgingRelease(SecCertificateCreateWithData(kCFAllocatorDefault, (CFDataRef)certificateData));
    if (certificate == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard certificate == NIL");
        //if (error != nil) {
        //    *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeCorruptedData userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"CORRUPTED_CERT", nil)}];
        //}
        //sometimes not all certificates are present on the card, in that case; don't search for any matching keys either
        return YES;
    }
    os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard created certificate");
    TKTokenKeychainCertificate *certificateItem = [[TKTokenKeychainCertificate alloc] initWithCertificate:(__bridge SecCertificateRef)certificate objectID:certificateID];
    if (certificateItem == nil) {
        os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard certificateItem == NIL");
        return NO;
    }
    os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard created certificateItem");
    [certificateItem setName:certificateName];

    if(keyTag != 0) {
        // Create key item.
        TKTokenKeychainKey *keyItem = [[PIVTokenKeychainKey alloc] initWithCertificate:(__bridge SecCertificateRef)certificate objectID:@(keyTag) certificateID:certificateItem.objectID alwaysAuthenticate:alwaysAuthenticate];
        if (keyItem == nil) {
            os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard keyItem == NIL");
            return NO;
        }
        os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard created keyItem");
        [keyItem setName:keyName];
        
        NSMutableDictionary<NSNumber *, TKTokenOperationConstraint> *constraints = [NSMutableDictionary dictionary];
        keyItem.canSign = sign;
        keyItem.suitableForLogin = NO;
        keyItem.canDecrypt = NO;
        keyItem.canPerformKeyExchange = NO;
        
        TKTokenOperationConstraint constraint = alwaysAuthenticate ? PIVConstraintPINAlways : PIVConstraintPIN;
        if (sign) {
            constraints[@(TKTokenOperationSignData)] = constraint;
            keyItem.constraints = constraints;
        }
        /*if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
            os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]");
            keyItem.canDecrypt = keyManagement;
            if (keyManagement) {
                constraints[@(TKTokenOperationDecryptData)] = constraint;
            }
        }*/

        [items addObject:keyItem];
    }
    [items addObject:certificateItem];
    
    os_log_error(OS_LOG_DEFAULT, "BEID populateIdentityFromSmartCard leave success");
    return YES;
}

- (nullable instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(nullable NSData *)AID PIVDriver:(PIVTokenDriver *)tokenDriver error:(NSError **)error {
    // Read and parse Card Holder Unique Identifier.
    // get card serial number and store it in instanceID
    
    os_log_error(OS_LOG_DEFAULT, "BEID initWithSmartCard called");
    __block NSString* instanceID;
    
    BOOL (^readInstanceID)(NSError**) = ^(NSError** err) {
        BOOL retVAL = false;
        
        [self selectRelFile:kBELPIC_DF_BELPIC len:2 smartCard:smartCard error:error];
        if (*error != nil){
            os_log_error(OS_LOG_DEFAULT, "BEID error selecting kBELPIC_DF_BELPIC");
            return retVAL;
        }
        
        [self selectRelFile:kBELPIC_EF_TokenInfo len:2 smartCard:smartCard error:error];
        if (*error != nil){
            os_log_error(OS_LOG_DEFAULT, "BEID error selecting kBELPIC_EF_TokenInfo");
            return retVAL;
        }
        
        NSData *tokenInfo = [self readBinary:smartCard error:error];

        if (tokenInfo == nil) {
            os_log_error(OS_LOG_DEFAULT, "BEID tokenInfo == nil");
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
            }
            return retVAL;
        }
        NSRange range;
        range.length=16;
        range.location=7;
        NSData *tokenSerial = [tokenInfo subdataWithRange:(NSRange)range];
        
        NSString *stringBuffer = [tokenSerial hexString];
        
        instanceID = [@"BEID-" stringByAppendingString:stringBuffer];

        os_log_error(OS_LOG_DEFAULT, "instanceID = %@",instanceID);
        retVAL = true;
        return retVAL;
    };

    BOOL success = [smartCard inSessionWithError:(NSError **)error executeBlock:(BOOL(^)(NSError **error))readInstanceID];
    if (success){
        
        if (self = [super initWithSmartCard:smartCard AID:AID instanceID:instanceID tokenDriver:tokenDriver]) {
            // Prepare array with keychain items representing on card objects.
            NSMutableArray<TKTokenKeychainItem *> *items = [NSMutableArray arrayWithCapacity:4];
            if (![self populateIdentityFromSmartCard:smartCard into:items certificateTag:0x5038 name:NSLocalizedString(@"BEID_AUTH_CERT", nil) keyTag:0x82 name:NSLocalizedString(@"BEID_AUTH_KEY", nil) sign:YES keyManagement:NO alwaysAuthenticate:NO error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items certificateTag:0x5039 name:NSLocalizedString(@"BEID_SIG_CERT", nil) keyTag:0x83 name:NSLocalizedString(@"BEID_SIG_KEY", nil) sign:YES keyManagement:NO alwaysAuthenticate:YES error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items certificateTag:0x503A name:NSLocalizedString(@"BEID_CA_CERT", nil) keyTag:0 name:NSLocalizedString(@"NO_KEY", nil) sign:NO keyManagement:NO alwaysAuthenticate:NO error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items certificateTag:0x503B name:NSLocalizedString(@"BEID_ROOTCA_CERT", nil) keyTag:0 name:NSLocalizedString(@"NO_KEY", nil) sign:NO keyManagement:NO alwaysAuthenticate:NO error:error]
                )
            {
                return nil;
            }
            
            // Populate keychain state with certificates and keys.
            [self.keychainContents fillWithItems:items];
        }
    }
    return self;
}

- (TKTokenSession *)token:(TKToken *)token createSessionWithError:(NSError * _Nullable __autoreleasing *)error {
    os_log_error(OS_LOG_DEFAULT, "BEID createSessionWithError called");
    return [[PIVTokenSession alloc] initWithToken:self];
}

@end
