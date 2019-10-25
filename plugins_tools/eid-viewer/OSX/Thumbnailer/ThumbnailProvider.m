//
//  ThumbnailProvider.m
//  Thumbnailer
//
//  Created by Wouter Verhelst on 17/10/2019.
//

#import "ThumbnailProvider.h"
#import <BeidView/oslayer-objc.h>

@implementation ThumbnailProvider

- (void)provideThumbnailForFileRequest:(QLFileThumbnailRequest *)request completionHandler:(void (^)(QLThumbnailReply * _Nullable, NSError * _Nullable))handler {
    
    // There are three ways to provide a thumbnail through a QLThumbnailReply. Only one of them should be used.
    
    // First way: Draw the thumbnail into the current context, set up with AppKit's coordinate system.
    handler([QLThumbnailReply replyWithContextSize:request.maximumSize currentContextDrawingBlock:^BOOL {
        // Draw the thumbnail here.
        
        // Return YES if the thumbnail was successfully drawn inside this block.
        return YES;
    }], nil);
}

@end
