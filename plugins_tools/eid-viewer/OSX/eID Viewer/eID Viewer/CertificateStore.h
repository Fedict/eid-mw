//
//  CertificateStore.h
//  eID Viewer
//
//  Created by buildslave on 29/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "binhandler.h"
#include <certhelpers.h>

typedef NS_ENUM(NSInteger, eIDDumpType) {
    eIDDumpTypeDer = DUMP_DER,
    eIDDumpTypePem = DUMP_PEM,
};

@interface CertificateStore : NSObject<NSOutlineViewDataSource,NSOutlineViewDelegate,binhandler>
@property NSMutableDictionary* CertificateData;
@property NSOutlineView* ov;
@property AppDelegate* ui;
-(instancetype)initWithOutlineView:(NSOutlineView*)view;
-(void)dumpFile:(int)fd forKey:(NSString*)key withFormat:(eIDDumpType)format;
-(NSString*)fileNameForKey:(NSString*)key;
-(NSString*)keyForParent:(NSString*)key;
-(void)clear;
@end
