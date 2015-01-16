//
//  DataItem.h
//  eID Middleware tool
//
//  Created by buildslave on 14/01/15.
//  Copyright (c) 2015 FedICT. All rights reserved.
//

#import <Foundation/Foundation.h>
@import AppKit;

@interface DataItem : NSObject<NSPasteboardWriting>
@property NSString *title;
@property NSString *value;
-(NSString*) description;
@end
