
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import <os/log.h>

@implementation BEIDTokenDriver

- (TKSmartCardToken *)tokenDriver:(TKSmartCardTokenDriver *)driver createTokenForSmartCard:(TKSmartCard *)smartCard AID:(NSData *)AID error:(NSError * _Nullable __autoreleasing *)error {

    UInt8 eIDCardATRBytes[13] = {0x3b, 0x98, 0x13, 0x40, 0x0a, 0xa5, 0x03, 0x01, 0x01, 0x01, 0xAD, 0x13, 0x11};
    UInt8 eIDCardATRBytes_v18[20] = {0x3b, 0x7f, 0x96, 0x00, 0x00, 0x80, 0x31, 0x80, 0x65, 0xb0, 0x85, 0x04, 0x01, 0x20, 0x12, 0x0f, 0xff, 0x82, 0x90, 0x00};

    NSData * eIDCardATR = [NSData dataWithBytes:eIDCardATRBytes length:13];
    NSData * eIDCardATR_v18 = [NSData dataWithBytes:eIDCardATRBytes_v18 length:20];
    NSData * atrBytes = smartCard.slot.ATR.bytes;
    
#ifdef DEBUG
    Byte* dataBytes = (Byte*)atrBytes.bytes;
    int i;
    
    for ( i = 0 ; i < atrBytes.length ; i++)
    {
        os_log_info(OS_LOG_DEFAULT, "%d: 0x%2x ",i, dataBytes[i]);
    }
#endif
    if ((![atrBytes isEqualToData:eIDCardATR]) && (![atrBytes isEqualToData:eIDCardATR_v18])){
        os_log_error(OS_LOG_DEFAULT, "NOT attaching BEID tokendriver, no ATR match");
        return nil;
    }
    
    
    
//Keep this log message
    os_log(OS_LOG_DEFAULT, "BEID attaching tokendriver");
    return [[BEIDToken alloc] initWithSmartCard:smartCard AID:AID BEIDDriver:self error:error];
}

@end
