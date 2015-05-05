//
//  CertificateStore.h
//  eID Viewer
//
//  Created by buildslave on 29/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "binhandler.h"

@interface CertificateStore : NSObject<NSOutlineViewDataSource,NSOutlineViewDelegate,binhandler>
@property NSMutableDictionary* CertificateData;
@property NSOutlineView* ov;
@property AppDelegate* ui;
-(instancetype)initWithOutlineView:(NSOutlineView*)view;
@end
