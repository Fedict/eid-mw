//
//  DraggingImage.h
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 18/06/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DraggingImage:NSImageView<NSPasteboardItemDataProvider, NSDraggingSource>
@end
