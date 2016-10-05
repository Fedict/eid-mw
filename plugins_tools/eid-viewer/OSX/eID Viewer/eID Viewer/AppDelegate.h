//
//  AppDelegate.h
//  eID Viewer
//
//  Created by buildslave on 23/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <BeidView/oslayer-objc.h>

@interface AppDelegate: NSObject < NSApplicationDelegate, eIDOSLayerUI >
-(NSObject *) searchObjectById:(NSString*)identity ofClass:(Class) aClass forUpdate:(BOOL)update;
@property BOOL isForeignerCard;
@end
