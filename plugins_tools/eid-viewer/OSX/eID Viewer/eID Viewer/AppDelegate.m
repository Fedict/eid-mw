//
//  AppDelegate.m
//  eID Viewer
//
//  Created by buildslave on 23/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()
- (IBAction)file_open:(id)sender;
- (IBAction)do_pinop:(NSSegmentedControl *)sender;

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSView *IdentityTab;
@property (weak) IBOutlet NSView *CardPinTab;
@property (weak) IBOutlet NSView *CertificatesTab;
@property (unsafe_unretained) IBOutlet NSTextView *logItem;
@end

@implementation AppDelegate
- (void)log:(NSString *)line withLevel:(eIDLogLevel)level {
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        char l;
        NSAlert* alert;
        switch(level) {
            case eIDLogLevelDetail:
                l='D';
                break;
            case eIDLogLevelNormal:
                l='N';
                break;
            case eIDLogLevelCoarse:
                l='C';
                break;
            case eIDLogLevelError:
                l='E';
                alert = [[NSAlert alloc] init];
                alert.messageText = [NSString stringWithFormat:@"Error: %@", line];
                [alert runModal];
                break;
        }
        NSString* output = [NSString stringWithFormat:@"%c: %@\n", l, line];
        [[self logItem] insertText:output];
    }];
}
- (void)file_open:(id)sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    [panel beginWithCompletionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            [eIDOSLayerBackend deserialize:[[panel URLs]objectAtIndex:0]];
        }
    }];
}
- (void)do_pinop:(NSSegmentedControl *)sender {
    NSUInteger sel = [sender selectedSegment];
    if(sel>2) {
        return; // shouldn't happen
    }
    eIDPinOp which = (eIDPinOp)sel;
    [eIDOSLayerBackend pinop:which];
}
- (void)newsrc:(eIDSource)which {
    
}
- (void)newbindata:(NSData *)data withLabel:(NSString *)label {
    
}
- (void)newstate:(eIDState)state {
    
}
- (NSObject*)searchView:(NSView*)from withName:(NSString*)name {
    if(![from conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
        return nil;
    }
    id<NSUserInterfaceItemIdentification> fromId = (id<NSUserInterfaceItemIdentification>) from;
    if([[fromId identifier]isEqualToString:name]) {
        return from;
    }
    int i;
    NSArray* arr = [from subviews];
    for(i=0;i<[arr count];i++) {
        NSObject* retval = [self searchView:[arr objectAtIndex:i] withName:name];
        if(retval != nil) {
            return retval;
        }
    }
    return nil;
}
- (void)newstringdata:(NSString *)data withLabel:(NSString *)label{
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        NSArray* elems = [NSArray arrayWithObjects:[self IdentityTab],[self CardPinTab], [self CertificatesTab], nil];
        for (int i=0; i<3; i++) {
            NSObject* o = [self searchView:[elems objectAtIndex:i] withName:label];
            if(o==nil || ![o isKindOfClass:[NSTextField class]]) {
                continue;
            }
            NSTextField* tf = (NSTextField*) o;
            [tf setStringValue:data];
            return;
        }
    }];
}
- (void)awakeFromNib {
    [eIDOSLayerBackend setUi:self];
    [eIDOSLayerBackend mainloop_thread];
}
@end
