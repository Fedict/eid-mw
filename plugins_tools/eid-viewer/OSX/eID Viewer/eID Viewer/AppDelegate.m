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
#import "PrintOperation.h"
#import "DataVerifier.h"

@interface AppDelegate ()
- (IBAction)file_open:(id)sender;
- (IBAction)file_close:(id)sender;
- (IBAction)do_pinop:(NSSegmentedControl *)sender;
- (IBAction)setLanguage:(NSMenuItem *)sender;
- (IBAction)log_buttonaction:(NSSegmentedControl *)sender;
- (IBAction)changeLogLevel:(NSPopUpButton *)sender;
- (IBAction)print:(id)sender;
- (IBAction)showDetail:(id)sender;
- (IBAction)export:(NSMenuItem *)sender;

@property CertificateStore *certstore;
@property NSDictionary *bindict;
@property NSMutableDictionary *viewdict;
@property (weak) IBOutlet NSImageView *photoview;
@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSWindow *CardReadSheet;
@property (weak) IBOutlet NSView *IdentityTab;
@property (weak) IBOutlet NSView *CardPinTab;
@property (weak) IBOutlet NSView *CertificatesTab;
@property (unsafe_unretained) IBOutlet NSTextView *logItem;
@property (weak) IBOutlet NSPopUpButton *logLevel;
@property (weak) IBOutlet NSOutlineView *CertificatesView;
@property (weak) IBOutlet NSView *printop_view;
@property (weak) IBOutlet NSProgressIndicator *spinner;

@property (weak) IBOutlet NSMenuItem *menu_file_open;
@property (weak) IBOutlet NSMenuItem *menu_file_close;
@property (weak) IBOutlet NSMenuItem *menu_file_save;
@property (weak) IBOutlet NSMenuItem *menu_file_print;
@property (weak) IBOutlet NSSegmentedControl *pinop_ctrl;

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
- (void)saveDocument:(id)sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setAllowedFileTypes:[NSArray arrayWithObjects: @"be.fedict.eid.eidviewer", nil]];
    [panel setNameFieldStringValue:[NSString stringWithFormat:@"%@.eid", [(NSTextField*)[self searchObjectById:@"national_number" ofClass:[NSTextField class]] stringValue]]];
    [panel beginWithCompletionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            [eIDOSLayerBackend serialize:[panel URL]];
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
    [_photoview setImage:nil];
    [_certstore clear];
    [_viewdict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop){
        if(![obj isKindOfClass:[NSTextField class]]) {
            return;
        }
        NSTextField* tf = (NSTextField*)obj;
        [tf setStringValue:@""];
    }];
}
- (void)newbindata:(NSData *)data withLabel:(NSString *)label {
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        id<binhandler> handler = [self.bindict objectForKey:label];
        [handler handle_bin_data:data forLabel:label withUi:self];
    }];
}
- (void) endSheet:(NSWindow*)sheet returnCode:(NSInteger)returnCode contextInfo:(void*)ctxInfo {
    [sheet orderOut:self];
}
- (void)newstate:(eIDState)state {
    DataVerifier *v = [DataVerifier verifier];
    BOOL fileOpen = NO;
    BOOL filePrint = NO;
    BOOL fileSave = NO;
    BOOL fileClose = NO;
    BOOL pinops = NO;
    switch(state) {
        case eIDStateReady:
            fileOpen = YES;
            break;
        case eIDStateToken:
        {
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                [_spinner startAnimation:self];
                [NSApp beginSheet:_CardReadSheet modalForWindow:_window modalDelegate:self didEndSelector:@selector(endSheet:returnCode:contextInfo:) contextInfo:nil];
            }];
        }
            break;
        case eIDStateTokenWait:
            filePrint = YES;
            fileSave = YES;
            pinops = YES;
        {
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                [NSApp endSheet:_CardReadSheet];
                [_spinner stopAnimation:self];
            }];
            if(!([v canVerify] && [v isValid])) {
                [self log:@"Cannot load card: data signature invalid!" withLevel:eIDLogLevelCoarse];
                [eIDOSLayerBackend set_invalid];
            }
        }
            break;
        case eIDStateFile:
            fileClose = YES;
            filePrint = YES;
            break;
        default:
            break;
    }
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        [_menu_file_print setEnabled:filePrint];
        [_menu_file_open setEnabled:fileOpen];
        [_menu_file_close setEnabled:fileClose];
        [_menu_file_save setEnabled: fileSave];
        [_pinop_ctrl setEnabled:pinops];
    }];
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
- (IBAction)print:(id)sender {
    [[[PrintOperation alloc] initWithView:_printop_view app:self] runOperation];
}

- (IBAction)showDetail:(id)sender {
}

- (IBAction)export:(NSMenuItem *)sender {
    NSString* key = [_CertificatesView itemAtRow:[_CertificatesView selectedRow]];
    if(key == nil) {
        return;
    }
    NSSavePanel* panel = [NSSavePanel savePanel];
    panel.title = @"Export";
    panel.nameFieldStringValue = [NSString stringWithFormat: @"%@%s.%s", [_certstore fileNameForKey:key], sender.tag == 3 ? "_chain" : "", sender.tag == 2 ? "der" : "pem"];
    [panel beginWithCompletionHandler:^(NSInteger result) {
        if(!result) return;
        int fd=open([panel.URL fileSystemRepresentation], O_CREAT|O_RDWR, S_IWRITE|S_IREAD);
        [_certstore dumpFile:fd forKey:key withFormat:sender.tag == 2 ? eIDDumpTypeDer : eIDDumpTypePem];
        if(sender.tag == 3) {
            NSString* newKey = [_certstore keyForParent:key];
            while(newKey != nil) {
                [_certstore dumpFile:fd forKey:key withFormat:eIDDumpTypePem];
                newKey = [_certstore keyForParent:newKey];
            }
        }
        close(fd);
    }];
}
- (void)awakeFromNib {
    if(_certstore != nil) {
        return;
    }
    // Set up the certificate pane
    _certstore = [[CertificateStore alloc] initWithOutlineView:_CertificatesView];
    DataVerifier *ver = [DataVerifier verifier];
    _bindict = [[NSDictionary alloc] initWithObjectsAndKeys:[photohandler alloc], @"PHOTO_FILE",
                _certstore, @"Root",
                _certstore, @"CA",
                _certstore, @"Authentication",
                _certstore, @"Signature",
                _certstore, @"CERT_RN_FILE",
                ver, @"SIGN_DATA_FILE",
                ver, @"SIGN_ADDRESS_FILE",
                ver, @"DATA_FILE",
                ver, @"ADDRESS_FILE",
                ver, @"photo_hash",
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
