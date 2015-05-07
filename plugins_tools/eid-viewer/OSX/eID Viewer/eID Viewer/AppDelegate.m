//
//  AppDelegate.m
//  eID Viewer
//
//  Created by buildslave on 23/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "AppDelegate.h"
#import "CertificateStore.h"
#import "photohandler.h"

@interface AppDelegate ()
- (IBAction)file_open:(id)sender;
- (IBAction)file_close:(id)sender;
- (IBAction)do_pinop:(NSSegmentedControl *)sender;
- (IBAction)setLanguage:(NSMenuItem *)sender;
- (IBAction)log_buttonaction:(NSSegmentedControl *)sender;
- (IBAction)changeLogLevel:(NSPopUpButton *)sender;

@property CertificateStore *certstore;
@property NSDictionary *bindict;
@property NSMutableDictionary *viewdict;
@property (weak) IBOutlet NSImageView *photoview;
@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSView *IdentityTab;
@property (weak) IBOutlet NSView *CardPinTab;
@property (weak) IBOutlet NSView *CertificatesTab;
@property (unsafe_unretained) IBOutlet NSTextView *logItem;
@property (weak) IBOutlet NSPopUpButton *logLevel;
@property (weak) IBOutlet NSOutlineView *CertificatesView;
@end

@implementation AppDelegate
- (void)log:(NSString *)line withLevel:(eIDLogLevel)level {
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        char l;
        NSAlert* alert;
        if([self.logLevel indexOfSelectedItem] > level) {
            return;
        }
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
- (void)file_close:(id)sender {
    [eIDOSLayerBackend close_file];
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
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        id<binhandler> handler = [self.bindict objectForKey:label];
        [handler handle_bin_data:data forLabel:label withUi:self];
    }];
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
- (NSObject*)searchObjectById:(NSString*)identity ofClass:(Class)aClass {
    NSObject* o = [_viewdict objectForKey:identity];
    if([o isKindOfClass:aClass]) {
        return o;
    }
    static NSArray* elems = NULL;
    if(!elems) {
        elems = [NSArray arrayWithObjects:[self IdentityTab],[self CardPinTab],[self CertificatesTab], nil];
    }
    for(int i=0;i<3;i++) {
        NSObject* o = [self searchView:[elems objectAtIndex:i] withName:identity];
        if(o != nil) {
            [_viewdict setValue:o forKey:identity];
        }
        if([o isKindOfClass:aClass]) {
            return o;
        }
    }
    return nil;
}
- (void)newstringdata:(NSString *)data withLabel:(NSString *)label{
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        NSTextField* tf = (NSTextField*)[self searchObjectById:label ofClass:[NSTextField class]];
        [tf setStringValue:data];
    }];
}
- (void)awakeFromNib {
    if(_certstore != nil) {
        return;
    }
    // Set up the certificate pane
    _certstore = [[CertificateStore alloc] initWithOutlineView:_CertificatesView];
    _bindict = [[NSDictionary alloc] initWithObjectsAndKeys:[photohandler alloc], @"PHOTO_FILE",
                _certstore, @"Root",
                _certstore, @"CA",
                _certstore, @"Authentication",
                _certstore, @"Signature",
                _certstore, @"CERT_RN_FILE",
                nil];
    _viewdict = [[NSMutableDictionary alloc] init];
    [_CertificatesView setDataSource:_certstore];
    [_CertificatesView setDelegate:_certstore];
    [eIDOSLayerBackend setUi:self];
    
    // Load preferences (language, log level)
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
    eIDLanguage langcode = [prefs integerForKey:@"ContentLanguage"];
    if(langcode == eIDLanguageNone||langcode > eIDLanguageNl) {
        NSArray* langs = [NSLocale preferredLanguages];
        for(int i=0; i<[langs count] && langcode == eIDLanguageNone;i++) {
            NSString* str = [langs objectAtIndex:i];
            if([str isEqualToString:@"de"]) {
                langcode = eIDLanguageDe;
            } else if([str isEqualToString:@"en"]) {
                langcode = eIDLanguageEn;
            } else if([str isEqualToString:@"fr"]) {
                langcode = eIDLanguageFr;
            } else if([str isEqualToString:@"nl"]) {
                langcode = eIDLanguageNl;
            }
        }
    }
    eIDLogLevel level = [prefs integerForKey:@"log_level"];
    [_logLevel selectItemAtIndex:level];
    [eIDOSLayerBackend setLang:langcode];
    [eIDOSLayerBackend mainloop_thread];
}
- (void)setLanguage:(NSMenuItem *)sender {
    NSString* keyeq = sender.keyEquivalent;
    eIDLanguage langcode = eIDLanguageNone;
    if([keyeq isEqualToString:@"d"]) {
        langcode = eIDLanguageDe;
    } else if([keyeq isEqualToString:@"e"]) {
        langcode = eIDLanguageEn;
    } else if([keyeq isEqualToString:@"f"]) {
        langcode = eIDLanguageFr;
    } else if([keyeq isEqualToString:@"n"]) {
        langcode = eIDLanguageNl;
    }
    if(langcode == eIDLanguageNone) {
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = @"Error: could not determine language";
        [alert runModal];
        return;
    }
    [self log:[NSString stringWithFormat:@"Setting language to %@", keyeq] withLevel:eIDLogLevelNormal];
    [eIDOSLayerBackend setLang:langcode];
    [[NSUserDefaults standardUserDefaults] setInteger:langcode forKey:@"ContentLanguage"];
}

- (IBAction)log_buttonaction:(NSSegmentedControl *)sender {
    NSUInteger sel = [sender selectedSegment];
    NSRange r;
    if (sel > 1) {
        return;
    }
    switch(sel) {
        case 0:
            [_logItem setString:@""];
            break;
        case 1:
            r = [_logItem selectedRange];
            [_logItem selectAll:self];
            [_logItem copy:self];
            [_logItem setSelectedRange:r];
            break;
    }
}
-(void)pinop_result:(eIDResult)result forOperation:(eIDPinOp)operation {
    [[NSOperationQueue mainQueue]addOperationWithBlock:^{
        NSString *msg = [NSString stringWithFormat:@"Pin code %@ %@.", (operation == eIDPinOpTest) ? @"test" : @"change", (result == eIDResultSuccess) ? @"successful" : @"failed"];
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = msg;
        [alert runModal];
        [self log:msg withLevel:eIDLogLevelDetail];
    }];
}
-(void)changeLogLevel:(NSPopUpButton *)logLevel {
    [[NSUserDefaults standardUserDefaults] setInteger:[logLevel indexOfSelectedItem] forKey:@"log_level"];
}
@end
