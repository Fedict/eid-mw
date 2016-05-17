//
//  ReaderMenuItem.m
//  eID Viewer
//
//  Created by buildslave on 17/05/16.
//  Copyright © 2016 Fedict. All rights reserved.
//

#import "ReaderMenuItem.h"

@implementation ReaderMenuItem
-(instancetype)initWithTitle:(NSString *)aString action:(SEL)aSelector keyEquivalent:(NSString *)charCode slotNumber:(NSInteger)slotNumber {
	self = [super initWithTitle:aString action:aSelector keyEquivalent:charCode];
	[self setSlotNumber:slotNumber];
	return self;
}
@end
