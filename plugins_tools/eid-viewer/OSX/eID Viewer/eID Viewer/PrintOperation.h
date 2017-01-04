//
//  PrintOperation.h
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 7/05/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

@interface PrintOperation:NSObject
@property NSMutableDictionary *viewDict;
@property NSView* view;
@property NSArray* printableFields;
@property NSArray* foreignerFields;
@property AppDelegate* app;
-(instancetype) initWithView:(NSView *)view app:(AppDelegate *)app;
-(BOOL) runOperation;
-(BOOL)copyDataWithArray:(NSArray*)arr;
@end
