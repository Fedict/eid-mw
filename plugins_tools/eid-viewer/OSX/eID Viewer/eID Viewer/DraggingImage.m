//
//  DraggingImage.m
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 18/06/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "DraggingImage.h"
#import <BeidView/oslayer-objc.h>

@implementation DraggingImage
-(void)mouseDown:(NSEvent*)event {
	NSImage *img = [self image];
	if(!img) {
		return;
	}
	NSPasteboardItem *pbItem = [NSPasteboardItem new];
	[pbItem setDataProvider:self forTypes:[NSArray arrayWithObject:NSPasteboardTypeString]];
	NSDraggingItem *drItem = [[NSDraggingItem alloc] initWithPasteboardWriter:pbItem];
	[drItem setDraggingFrame:[img alignmentRect]contents:img];
	NSDraggingSession *session = [self beginDraggingSessionWithItems:[NSArray arrayWithObject:drItem] event:event source:self];
	session.animatesToStartingPositionsOnCancelOrFail = YES;
	session.draggingFormation = NSDraggingFormationDefault;
}
-(NSDragOperation)draggingSession:(NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
	if(![eIDOSLayerBackend xmlform]) {
		return NSDragOperationNone;
	}
	return NSDragOperationCopy;
}
-(BOOL)acceptsFirstMouse:(NSEvent *)theEvent {
	return YES;
}
-(void)pasteboard:(NSPasteboard *)pasteboard item:(NSPasteboardItem *)item provideDataForType:(NSString *)type {
	if([type compare:NSPasteboardTypeString] != NSOrderedSame) {
		[pasteboard setData:nil forType:type];
		return;
	}
	[pasteboard setData:[eIDOSLayerBackend xmlform] forType:type];
}
@end
