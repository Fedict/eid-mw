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
    *range = NSMakeRange(0, 1);
    return YES;
}

-(NSRect)rectForPage:(NSInteger)page {
    NSPrintInfo *pi = [[NSPrintOperation currentOperation] printInfo];
    
    //_leadingAnchor
    
    [self layoutSubtreeIfNeeded];
    return self.bounds;
}
@end
