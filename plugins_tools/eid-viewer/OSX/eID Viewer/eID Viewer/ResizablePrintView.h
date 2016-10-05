//
//  ResizablePrintView.h
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 24/09/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ResizablePrintView: NSView
- (BOOL) knowsPageRange:(NSRangePointer)range;
- (NSRect) rectForPage:(NSInteger)page;
@end
