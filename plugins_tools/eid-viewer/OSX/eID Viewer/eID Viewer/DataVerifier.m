//
//  DataVerifier.m
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 19/05/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "DataVerifier.h"
#include <openssl/x509.h>
#include <eid-viewer/certhelpers.h>

static DataVerifier* ver;

@implementation DataVerifier
+(DataVerifier*)verifier {
    if(ver == nil) {
        ver = [[DataVerifier alloc]init];
    }
    return ver;
}
-(instancetype)init {
    if(self = [super init]) {
        _elements = [[NSMutableDictionary alloc] init];
    }
    return self;
}
-(void)clear {
    NSMutableDictionary *dict = (NSMutableDictionary*)_elements;
    [dict removeAllObjects];
}
-(BOOL)canVerify {
    return ([_elements count] == 7) ? YES : NO;
}
-(void)handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
    NSMutableDictionary* dict = (NSMutableDictionary*)_elements;
    [dict setObject:data forKey:label];
}
-(BOOL)isValid {
    NSData* photohash = [_elements objectForKey:@"photo_hash"];
    NSData *photo = [_elements objectForKey:@"PHOTO_FILE"];
    NSData *rrncert = [_elements objectForKey:@"CERT_RN_FILE"];
    NSData *datfile = [_elements objectForKey:@"DATA_FILE"];
    NSData *datsig = [_elements objectForKey:@"SIGN_DATA_FILE"];
    NSData *address = [_elements objectForKey:@"ADDRESS_FILE"];
    NSData *addrsig = [_elements objectForKey:@"SIGN_ADDRESS_FILE"];

    return eid_vwr_check_data_validity(photo.bytes, (int)photo.length, photohash.bytes, (int)photohash.length, datfile.bytes, (int)datfile.length, datsig.bytes, (int)datsig.length, address.bytes, (int)address.length, addrsig.bytes, (int)addrsig.length, rrncert.bytes, (int)rrncert.length)? YES : NO;
}
@end
