//
//  DataVerifier.h
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 19/05/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "binhandler.h"

@interface DataVerifier : NSObject<binhandler>
@property NSDictionary *elements;
+(DataVerifier*)verifier;
-(instancetype)init;
-(BOOL)canVerify;
-(void)clear;
-(BOOL)isValid;
@end
