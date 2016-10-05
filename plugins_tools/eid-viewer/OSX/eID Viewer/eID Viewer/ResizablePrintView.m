//
//  ResizablePrintView.m
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 24/09/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "ResizablePrintView.h"
@implementation ResizablePrintView
-(BOOL)knowsPageRange:(NSRangePointer)range {
	*range = NSMakeRange(1, 1);
	return YES;
}

-(NSRect)rectForPage:(NSInteger)page {
	NSPrintInfo *pi = [[NSPrintOperation currentOperation] printInfo];

	NSSize ps = [pi paperSize];
	[self addConstraint:[NSLayoutConstraint constraintWithItem:self attribute:NSLayoutAttributeWidth relatedBy: NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:ps.width]];
	[self addConstraint:[NSLayoutConstraint constraintWithItem:self attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:ps.height]];
	[self layoutSubtreeIfNeeded];
	return self.bounds;
}
@end
