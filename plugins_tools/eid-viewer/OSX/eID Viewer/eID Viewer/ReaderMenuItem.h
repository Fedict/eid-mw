//
//  ReaderMenuItem.h
//  eID Viewer
//
//  Created by buildslave on 17/05/16.
//  Copyright © 2016 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ReaderMenuItem: NSMenuItem
- (instancetype) initWithTitle:(NSString*) aString action:(SEL) aSelector keyEquivalent:(NSString *) charCode slotNumber:(NSInteger) slotNumber;
@property NSInteger slotNumber;
@end
