//
//  PrintOperation.m
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 7/05/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "PrintOperation.h"

@implementation PrintOperation
-(instancetype)initWithView:(NSView *)view app:(AppDelegate *)app {
    self = [super init];
    [self setView:view];
    [self setApp:app];
    [self setViewDict:[[NSMutableDictionary alloc]init]];
    [self indexViews:view];
    [self setPrintableFields:[[NSArray alloc] initWithObjects:@"surname", @"photo", @"firstnames", @"location_of_birth", @"date_of_birth", @"gender", @"nationality", @"national_number", @"nobility", @"special_status", @"address_street_and_number", @"address_zip", @"address_municipality", @"card_number", @"issuing_municipality", @"chip_number", @"validity_begin_date", @"validity_end_date", nil]];
    return self;
}
-(void)indexViews:(NSView*)view {
    if(![view conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
        return;
    }
    NSObject<NSUserInterfaceItemIdentification>*from = (NSObject<NSUserInterfaceItemIdentification>*)view;
    [_viewDict setObject:from forKey:[from identifier]];
    NSArray* arr = [view subviews];
    for(int i=0;i<[arr count]; i++) {
        [self indexViews:[arr objectAtIndex:i]];
    }
}
-(BOOL)runOperation {
    for(int i=0; i<[_printableFields count]; i++) {
        NSString* name = [_printableFields objectAtIndex:i];
        NSObject* d = [_viewDict objectForKey:name];
        NSObject* s = [_app searchObjectById:name ofClass:[d class]];
        SEL readsel;
        SEL writesel;
        if([s isKindOfClass:[NSTextField class]]) {
            readsel = @selector(stringValue);
            writesel = @selector(setStringValue:);
        } else if([s isKindOfClass:[NSImageView class]]) {
            readsel = @selector(image);
            writesel = @selector(setImage:);
        } else {
            [_app log:[NSString stringWithFormat:@"Could not find field for %@; print operation cancelled", name] withLevel:eIDLogLevelCoarse];
            return NO;
        }
        [d performSelector:writesel withObject:[s performSelector:readsel]];
    }
    return [[NSPrintOperation printOperationWithView:_view] runOperation];
}
@end
