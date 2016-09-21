
#import <Foundation/Foundation.h>
#import <CryptoTokenKit/CryptoTokenKit.h>

#import "Token.h"
#import <os/log.h>

@implementation PIVTokenDriver

- (TKSmartCardToken *)tokenDriver:(TKSmartCardTokenDriver *)driver createTokenForSmartCard:(TKSmartCard *)smartCard AID:(NSData *)AID error:(NSError * _Nullable __autoreleasing *)error {
    //assert(0);
    os_log_error(OS_LOG_DEFAULT, "BEID attaching tokendriver");
    return [[PIVToken alloc] initWithSmartCard:smartCard AID:AID PIVDriver:self error:error];
}

@end
