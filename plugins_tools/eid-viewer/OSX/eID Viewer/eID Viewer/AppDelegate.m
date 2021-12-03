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
#import "ReaderMenuItem.h"

#include <eid-util/utftranslate.h>
#include <eid-util/labels.h>

@interface AppDelegate ()
- (IBAction)file_open:(id)sender;
- (IBAction)file_close:(id)sender;
- (IBAction)do_pinop:(NSSegmentedControl *)sender;
- (IBAction)setLanguage:(NSSegmentedControl *)sender;
- (IBAction)log_buttonaction:(NSSegmentedControl *)sender;
- (IBAction)changeLogLevel:(NSPopUpButton *)sender;
- (IBAction)print:(id)sender;
- (IBAction)showDetail:(id)sender;
- (IBAction)export:(NSMenuItem *)sender;
- (IBAction)validateNow:(id)sender;
- (IBAction)changeValidatePolicy:(id)sender;
- (IBAction)selectAutoReader:(NSMenuItem*)sender;
- (IBAction)selectManualReader:(NSMenuItem*)sender;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;
- (IBAction)closeDetail:(id)sender;
- (BOOL)application:(NSApplication*)sender openFile:(nonnull NSString *)filename;
- (IBAction)basicKeyCheck:(id)sender;
- (IBAction)showPreferences:(id)sender;
- (IBAction)changeUpdatePref:(id)sender;
- (IBAction)checkUpdatesNow:(id)sender;

@property CertificateStore *certstore;
@property NSDictionary *bindict;
@property NSMutableDictionary *viewdict;
@property NSArray *readerSelections;
@property BOOL hasBasicKey;
@property (weak) IBOutlet NSImageView *photoview;
@property (weak) IBOutlet NSImageView *certview;
@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSWindow *CardReadSheet;
@property BOOL sheetIsActive;
@property (weak) IBOutlet NSView *IdentityTab;
@property (weak) IBOutlet NSView *CardPinTab;
@property (weak) IBOutlet NSView *CertificatesTab;
@property (unsafe_unretained) IBOutlet NSTextView *logItem;
@property (weak) IBOutlet NSPopUpButton *logLevel;
@property (weak) IBOutlet NSOutlineView *CertificatesView;
@property (weak) IBOutlet NSView *printop_view;
@property (weak) IBOutlet NSView *foreigner_printop_view;
@property (weak) IBOutlet NSProgressIndicator *spinner;
@property (weak) IBOutlet NSButton *alwaysValidate;
@property (weak) IBOutlet NSButton *validateNow;

@property (weak) IBOutlet NSMenuItem *menu_file_open;
@property (weak) IBOutlet NSMenuItem *menu_file_close;
@property (weak) IBOutlet NSMenuItem *menu_file_save;
@property (weak) IBOutlet NSMenuItem *menu_file_print;
@property (weak) IBOutlet NSMenuItem *auto_reader;
@property (weak) IBOutlet NSSegmentedControl *pinop_ctrl;
@property (unsafe_unretained) IBOutlet NSTextView *CertDetailView;
@property (weak) IBOutlet NSWindow *CertDetailSheet;
@property (weak) IBOutlet NSBox *centeringLine;
@property (weak) IBOutlet NSBox *bottomLine;
@property (weak) IBOutlet NSTextField *lowestItem;
@property (weak) IBOutlet NSLayoutConstraint *verticalLineBottomConstraint;
@property (weak) IBOutlet NSButton *memberOfFamilyState;
@property (weak) IBOutlet NSImageView *readerLogo;
@property (weak) IBOutlet NSButton *BasicKeyButton;
@property (weak) IBOutlet NSPanel *prefsPane;
@property (weak) IBOutlet NSPopUpButton *selectedReader;
@property (weak) IBOutlet NSSegmentedControl *currentLanguage;
@property (weak) IBOutlet NSButton *updateCheck;

@end

@implementation AppDelegate
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

- (IBAction)basicKeyCheck:(id)sender {
	[eIDOSLayerBackend doChallengeInternal];
}

-(BOOL) useDefaultChallengeResult {
	return YES;
}

- (BOOL)application:(NSApplication*)sender openFile:(nonnull NSString *)filename {
	[eIDOSLayerBackend deserialize:[NSURL URLWithString:filename]];
	return YES;
}
- (void)log:(NSString *)line withLevel:(eIDLogLevel)level {
	[[NSOperationQueue mainQueue] addOperationWithBlock:^{
		if([self.logLevel indexOfSelectedItem] > level) {
			return;
		}
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
				alert.messageText = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"errorprefix", nil, [NSBundle mainBundle], @"Error: %@", "error prefix"), line];
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
	[panel setNameFieldStringValue:[NSString stringWithFormat:@"%@.eid", [(NSTextField*)[self searchObjectById:@"national_number" ofClass:[NSTextField class] forUpdate:NO] stringValue]]];
	[panel beginWithCompletionHandler:^(NSInteger result) {
		if(result == NSFileHandlingPanelOKButton) {
			[eIDOSLayerBackend serialize:[panel URL]];
		}
	}];
}
- (void)file_close:(id)sender {
	[eIDOSLayerBackend closeFile];
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
	[[NSOperationQueue mainQueue] addOperationWithBlock:^{
		[self.photoview setImage:nil];
		[self.certview setImage:nil];
		[self.certstore clear];
		[self.viewdict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop){
			if(![obj isKindOfClass:[NSTextField class]]) {
				return;
			}
			NSTextField* tf = (NSTextField*)obj;
			[tf setStringValue:@""];
			[tf setTextColor:NULL];
		}];
		[self.memberOfFamilyState setState:NSOffState];
	}];
}
- (void)newbindata:(NSData *)data withLabel:(NSString *)label {
	[[NSOperationQueue mainQueue] addOperationWithBlock:^{
		id<binhandler> handler = [self.bindict objectForKey:label];
		[handler handle_bin_data:data forLabel:label withUi:self];
	}];
}
- (void)newstate:(eIDState)state {
	static eIDState prevState = eIDStateLibOpen;
	BOOL fileOpen = NO;
	BOOL filePrint = NO;
	BOOL fileSave = NO;
	BOOL fileClose = NO;
	BOOL pinops = NO;
	BOOL validate = NO;
	BOOL doValidateNow = NO;
	BOOL sheet = NO;
	BOOL dnd = NO;
	BOOL doTest = NO;
	switch(state) {
		case eIDStateReady:
			fileOpen = YES;
			[self setHasBasicKey:NO];
			break;
		case eIDStateNoReader:
			fileOpen = YES;
			break;
		case eIDStateToken:
		case eIDStateTokenID:
		case eIDStateTokenCerts:
			sheet = YES;
			break;
		case eIDStateTokenIdle:
			filePrint = YES;
			fileSave = YES;
			pinops = YES;
			validate = YES;
			dnd = YES;
			doValidateNow = YES;
			doTest = YES;
			break;
		case eIDStateFileWait:
			fileClose = YES;
			filePrint = YES;
			dnd = YES;
			validate = YES;
			doValidateNow = YES;
		default:
			break;
	}
	if(prevState == eIDStateNoReader || prevState == eIDStateTokenError) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			[self.readerLogo setHidden:YES];
		}];
	}
	if(state == eIDStateNoReader) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			[self.readerLogo setImage:[NSImage imageNamed:@"state_noreaders.png"]];
			[self.readerLogo setHidden:NO];
		}];
	}
	if(state == eIDStateTokenError) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			[self.readerLogo setImage:[NSImage imageNamed:@"state_error.png"]];
			[self.readerLogo setHidden:NO];
		}];
	}
	prevState = state;
	[[NSOperationQueue mainQueue] addOperationWithBlock:^{
		[self.menu_file_print setEnabled:filePrint];
		[self.menu_file_open setEnabled:fileOpen];
		[self.menu_file_close setEnabled:fileClose];
		[self.menu_file_save setEnabled: fileSave];
		[self.pinop_ctrl setEnabled:pinops];
		[self.alwaysValidate setEnabled:validate];
		[self.validateNow setEnabled:validate];
		if([self hasBasicKey] && doTest) {
			[self.BasicKeyButton setEnabled:YES];
		} else {
			[self.BasicKeyButton setEnabled:NO];
		}
		if(sheet && ![self sheetIsActive]) {
			[self.spinner startAnimation:self];
			[self.window beginSheet:self.CardReadSheet completionHandler:nil];
			[self setSheetIsActive:YES];
		}
		if(!sheet && [self sheetIsActive]) {
			[self.window endSheet:self.CardReadSheet];
			[self.CardReadSheet orderOut:self.window];
			[self.spinner stopAnimation:self];
			[self setSheetIsActive:NO];
		}
		if(doValidateNow && ([self.alwaysValidate state] == NSOnState)) {
			[self validateNow:nil];
		}
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
- (NSObject*)searchObjectById:(NSString*)identity ofClass:(Class)aClass forUpdate:(BOOL)update {
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
		if(o != nil && update == YES) {
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
		NSTextField* tf = (NSTextField*)[self searchObjectById:label ofClass:[NSTextField class] forUpdate:YES];
		[tf setStringValue:data ? data : @""];
	}];
}
- (IBAction)print:(id)sender {
	[[[PrintOperation alloc] initWithView:([self isForeignerCard] ? [self foreigner_printop_view] : [self printop_view]) app:self] runOperation];
}

- (IBAction)showDetail:(id)sender {
	NSString* details = [eIDOSLayerBackend getCertDetail:[_certstore certificateForKey:[_CertificatesView itemAtRow:[_CertificatesView selectedRow]]]];
	if(details == nil) return;
	[_window beginSheet:_CertDetailSheet completionHandler:^(NSModalResponse r){
		[self.window endSheet:self.CertDetailSheet];
	}];
//make editable to do some changes
	[_CertDetailView setEditable:YES];
	[_CertDetailView selectAll:nil];
	[_CertDetailView delete:nil];
	[_CertDetailView insertText:details];
//no more changes
	[_CertDetailView setEditable:NO];
}

- (IBAction)export:(NSMenuItem *)sender {
	NSString* key = [_CertificatesView itemAtRow:[_CertificatesView selectedRow]];
	if(key == nil) {
		return;
	}
	NSSavePanel* panel = [NSSavePanel savePanel];
	panel.title = NSLocalizedStringWithDefaultValue(@"ExportTitle", nil, [NSBundle mainBundle], @"Export", "");
	panel.nameFieldStringValue = [NSString stringWithFormat: @"%@%s.%s", [_certstore fileNameForKey:key], sender.tag == 3 ? "_chain" : "", sender.tag == 2 ? "der" : "pem"];
	[panel beginWithCompletionHandler:^(NSInteger result) {
		if(!result) return;
		int fd=open([panel.URL fileSystemRepresentation], O_CREAT|O_RDWR, S_IWRITE|S_IREAD);
		[self.certstore dumpFile:fd forKey:key withFormat:sender.tag == 2 ? eIDDumpTypeDer : eIDDumpTypePem];
		if(sender.tag == 3) {
			NSString* newKey = [self.certstore keyForParent:key];
			while(newKey != nil) {
				[self.certstore dumpFile:fd forKey:key withFormat:eIDDumpTypePem];
				newKey = [self.certstore keyForParent:newKey];
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
	_bindict = [[NSDictionary alloc] initWithObjectsAndKeys:[photohandler alloc], @"PHOTO_FILE",
		    _certstore, @"Root",
		    _certstore, @"CA",
		    _certstore, @"Authentication",
		    _certstore, @"Signature",
		    _certstore, @"CERT_RN_FILE",
		    self, @"certimage",
		    self, @"document_type_raw",
		    self, @"member_of_family",
		    self, @"basic_key_hash",
		    self, @"basic_key_verify:valid",
            self, @"carddata_appl_version",
		    nil];
	_viewdict = [[NSMutableDictionary alloc] init];
	[_CertificatesView setDataSource:_certstore];
	[_CertificatesView setDelegate:_certstore];
	[self setSheetIsActive:NO];

	// Load preferences (language, log level)
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	eIDLanguage langcode = [prefs integerForKey:@"ContentLanguage"];
	if(langcode == eIDLanguageNone||langcode > eIDLanguageNl) {
		langcode = eIDLanguageNone;
		NSArray* langs = [NSLocale preferredLanguages];
		for(int i=0; i<[langs count] && langcode == eIDLanguageNone;i++) {
			NSString* str = [[langs objectAtIndex:i] substringToIndex:2];
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
		if(langcode == eIDLanguageNone) {
			// system language doesn't work, choose English instead
			langcode = eIDLanguageEn;
		}
	}
        [_currentLanguage setSelectedSegment:(NSInteger)langcode - 1];
	eIDLogLevel level = [prefs integerForKey:@"log_level"];
	BOOL alw_val = [prefs boolForKey:@"always_validate"];
	if(alw_val) {
		[_alwaysValidate setState:NSOnState];
	} else {
		[_alwaysValidate setState:NSOffState];
	}
        BOOL startup_update = [prefs boolForKey:@"check_update"];
        if(startup_update) {
                [self checkUpdatesNow:self];
                [_updateCheck setState:NSOnState];
        } else {
                [_updateCheck setState:NSOffState];
        }
	[self setIsForeignerCard:NO];
	[_logLevel selectItemAtIndex:level];
	[eIDOSLayerBackend setLang:langcode];
	[eIDOSLayerBackend setUi:self];
}
- (void)setLanguage:(NSSegmentedControl *)sender {
        NSUInteger sel = [sender selectedSegment];
	eIDLanguage langcode = eIDLanguageNone;
        NSString *s = @"";
        langcode = (eIDLanguage)(sel + 1);
        if(langcode > eIDLanguageNl) {
		NSAlert *alert = [[NSAlert alloc] init];
		alert.messageText = @"Error: could not determine language";
		[alert runModal];
		return;
	}
	[self log:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"LanguageChosenLog", nil, [NSBundle mainBundle], @"Setting language to %@", ""), s] withLevel:eIDLogLevelNormal];
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
		NSString *msgTemplate;
		if(operation == eIDPinOpTest) {
			msgTemplate = NSLocalizedStringWithDefaultValue(@"PinCodeTestMsg", nil, [NSBundle mainBundle], @"Pin code test: %@.", "");
		} else {
			msgTemplate = NSLocalizedStringWithDefaultValue(@"PinCodeChangeMsg", nil, [NSBundle mainBundle], @"Pin code change: %@.", "");
		}
		NSString *msg = [NSString stringWithFormat:msgTemplate, (result == eIDResultSuccess) ? NSLocalizedStringWithDefaultValue(@"PinSuccess", nil, [NSBundle mainBundle], @"successful", "") : NSLocalizedStringWithDefaultValue(@"PinFailed", nil, [NSBundle mainBundle], @"failed", "")];
		NSAlert *alert = [[NSAlert alloc] init];
		alert.messageText = msg;
		[alert runModal];
		[self log:msg withLevel:eIDLogLevelDetail];
	}];
}
-(void)setHex:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
	size_t length = [data length];
	char string[length * 2 + 1];
	const unsigned char *source = [data bytes];
	for(int i = 0; i<length; i+=2) {
		snprintf(&string[i], 3, "%02x", (unsigned int)(source[i/2]));
	}
	string[length * 2] = '\0';
	[ui newstringdata:[NSString stringWithCString:string encoding:NSUTF8StringEncoding] withLabel:label];
}
-(void)handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
	assert(ui == self);
	if([label isEqualToString:@"certimage"]) {
		[(NSImageView*) [ui searchObjectById:label ofClass:[NSImageView class] forUpdate:NO] setImage:(NSImage*)data];
	}
	else if([label isEqualToString:@"member_of_family"]) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			[self.memberOfFamilyState setState:NSOnState];
		}];
    }
    else if([label isEqualToString:@"basic_key_hash"]) {
		[self setHex:data forLabel:label withUi:ui];
		[self setHasBasicKey:YES];
	}
	else if([label isEqualToString:@"basic_key_verify:valid"]) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			NSTextField *t = (NSTextField*)[self searchObjectById:@"basic_key_hash" ofClass:[NSTextField class] forUpdate:NO];
			int a;
			[data getBytes:&a length:sizeof(a)];
			[t setTextColor:(a == 1 ? [NSColor systemGreenColor] : [NSColor systemRedColor])];
		}];
	}
	else if([label isEqualToString:@"document_type_raw"]) {
		BOOL new_foreigner;
		char b0, b1;
		if([data length] > 1) {
			[data getBytes:&b0 length:1];
			[data getBytes:&b1 range:NSMakeRange(1, 1)];
		} else {
			b0 = ' ';
			[data getBytes:&b1 length:1];
		}
		if((b0 == ' ' || b0 == '0') && b1 == '1') {
			new_foreigner = NO;
		} else {
			new_foreigner = YES;
		}
		if([self isForeignerCard] != new_foreigner) {
			[self setIsForeignerCard: new_foreigner];
			[[NSOperationQueue mainQueue] addOperationWithBlock:^{
				struct labelnames* toggles = get_foreigner_labels();
				int i;
				for(i=0; i<toggles->len; i++) {
					NSView *v = (NSView*)[self searchObjectById:[NSString stringWithUTF8String:toggles->label[i]] ofClass:[NSView class] forUpdate:NO];
					[v setHidden:!new_foreigner];
					v = (NSView*)[self searchObjectById:[NSString stringWithFormat:@"title_%s",toggles->label[i]] ofClass:[NSView class] forUpdate:NO];
					[v setHidden:!new_foreigner];
				}
				[self.IdentityTab removeConstraint:self.verticalLineBottomConstraint];
				if([self isForeignerCard]) {
					self.verticalLineBottomConstraint = [NSLayoutConstraint constraintWithItem:self.centeringLine attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:self.lowestItem attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0.0];
				} else {
					self.verticalLineBottomConstraint = [NSLayoutConstraint constraintWithItem:self.centeringLine attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:self.bottomLine attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0.0];
				}
				[self.IdentityTab addConstraint:self.verticalLineBottomConstraint];
				[self.IdentityTab layoutSubtreeIfNeeded];
			}];
		}
	}
    else if([label isEqualToString:@"carddata_appl_version"]) {
        char vers;
        [data getBytes:&vers length:1];
        switch(vers) {
            case 0x17:
                [self newstringdata:@"v1.7" withLabel:label];
                break;
            case 0x18:
                [self newstringdata:@"v1.8" withLabel:label];
                break;
            default:
                [self newstringdata:@"?" withLabel:label];
                break;
        }
    }
}
-(void)changeLogLevel:(NSPopUpButton *)logLevel {
	[[NSUserDefaults standardUserDefaults] setInteger:[logLevel indexOfSelectedItem] forKey:@"log_level"];
}
#define maybe_fail(r, c, j) if(((r) = (c)) == eIDResultFailed) { have_fail = true; if(j) goto failed; }
-(IBAction)validateNow:(id)sender {
	NSData* ca = [_certstore certificateForKey:@"CA"];
	NSData* root = [_certstore certificateForKey:@"Root"];
	bool have_fail = false;
        NSString *filePath = [[NSBundle mainBundle] pathForResource:@"extraopts" ofType:@"plist"];
        NSDictionary *extraopts = [NSDictionary dictionaryWithContentsOfFile:filePath];
        BOOL disableWhitelist = [[extraopts valueForKey:@"disableWhitelist"] boolValue];
        
	eIDResult resRoot = eIDResultUnknown,
		resCa = eIDResultUnknown,
		resSig = eIDResultUnknown,
		resAuth = eIDResultUnknown,
		resRRN = eIDResultUnknown;
	maybe_fail(resRoot, [eIDOSLayerBackend validateRootCert:root], true);
	maybe_fail(resRRN, [eIDOSLayerBackend validateRrnCert:[_certstore certificateForKey:@"CERT_RN_FILE"] withRoot:root], false);
	maybe_fail(resCa, [eIDOSLayerBackend validateIntCert:ca withCa:root], true);
	maybe_fail(resSig, [eIDOSLayerBackend validateCert:[_certstore certificateForKey:@"Signature"] withCa:ca], true);
	maybe_fail(resAuth, [eIDOSLayerBackend validateCert:[_certstore certificateForKey:@"Authentication"] withCa:ca andAllowList:!disableWhitelist], true);
failed:
	if(resCa != eIDResultSuccess && resSig == eIDResultSuccess) {
		resSig = resCa;
	}
	if(resCa != eIDResultSuccess && resAuth == eIDResultSuccess) {
		resAuth = resCa;
	}
#undef maybe_fail
	[_certstore setValid:resRoot forKey:@"Root"];
	[_certstore setValid:resCa forKey:@"CA"];
	[_certstore setValid:resRRN forKey:@"CERT_RN_FILE"];
	[_certstore setValid:resAuth forKey:@"Authentication"];
	[_certstore setValid:resSig forKey:@"Signature"];
	if(have_fail) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			NSAlert* error = [[NSAlert alloc ] init];
			[error setAlertStyle:NSWarningAlertStyle];
			[error setMessageText:NSLocalizedStringWithDefaultValue(@"InvalidCertsFound", nil, [NSBundle mainBundle], @"One or more of the certificates on this card were found to be invalid or revoked.", "")];
			[error setInformativeText:NSLocalizedStringWithDefaultValue(@"InvalidCertsMoreInfo", nil, [NSBundle mainBundle], @"For more information, please see the log tab", "")];
			[error runModal];
		}];
	}
}
-(void)changeValidatePolicy:(id)sender {
	BOOL on = ([_alwaysValidate state] == NSOnState);
	[[NSUserDefaults standardUserDefaults] setBool:on forKey:@"always_validate"];
	if(on) {
		[self validateNow:sender];
	}
}
-(IBAction)selectAutoReader:(NSMenuItem*)sender {
	if([_readerSelections count] == 0) {
		// We haven't seen the readers yet, can't do a manual selection. This should be impossible by the fact that we disabled the menu item, but it doesn't hurt to be sure.
		return;
	}
	for(int i=0; i<[_readerSelections count]; i++) {
		[[_readerSelections objectAtIndex:i] setState:NSOffState];
	}
	[sender setState:NSOnState];
	[eIDOSLayerBackend setReaderAuto:YES];
}
-(IBAction)selectManualReader:(ReaderMenuItem*)sender {
	[_auto_reader setState: NSOffState];
	for(int i=0; i<[_readerSelections count]; i++) {
		[[_readerSelections objectAtIndex:i] setState: NSOffState];
	}
	[sender setState:NSOnState];
	[eIDOSLayerBackend selectReader:[sender slotNumber]];
}
-(void)readersFound:(NSArray *)readers withSlotNumbers:(NSArray *)slots {
	NSInteger count = [readers count];
	assert(count == [slots count]);
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                ReaderMenuItem* newreaders[count];
                for(int i=0; i<count; i++) {
                        NSInteger slot = [[slots objectAtIndex:i]integerValue];
                        NSString *readerName = [readers objectAtIndex:i];
                        newreaders[i] = [[ReaderMenuItem alloc] initWithTitle:readerName action:@selector(selectManualReader:) keyEquivalent:@"" slotNumber:slot];
                        [[_selectedReader menu] addItem:newreaders[i]];
                }
                [_auto_reader setEnabled:count > 0 ? YES : NO];
                _readerSelections = [NSArray arrayWithObjects:newreaders count:count];
        }];
}

- (IBAction)closeDetail:(id)sender {
	[self.window endSheet:_CertDetailSheet];
}
- (IBAction)showPreferences:(id)sender {
        [_prefsPane setIsVisible:YES];
}
- (IBAction)checkUpdatesNow:(id)sender {
        NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL URLWithString:@"https://eid.belgium.be/sites/default/files/software/eidversions.xml"]];
        NSURLResponse *resp;
        NSError *err;
        NSData *respdata = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
        if(respdata != nil) {
                NSString *xml = [[NSString alloc] initWithData:respdata encoding:NSUTF8StringEncoding];
                eIDVersionTriplet *tr = [[eIDVersionTriplet alloc] init];
                NSString *version = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
                NSArray *versionArray = [version componentsSeparatedByString:@"."];
                [tr setMajor:[versionArray[0] integerValue]];
                [tr setMinor:[versionArray[1] integerValue]];
                [tr setBuild:[versionArray[2] integerValue]];
                eIDUpgradeInfo *info = [eIDOSLayerBackend parseUpgradeInfoForXml:xml  currentVersion:tr];
                if([info haveUpgrade]) {
                        NSAlert *alert = [[NSAlert alloc] init];
                        eIDVersionTriplet *newVersion = [info newVersion];
                        alert.messageText = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"updatemessage", nil, [NSBundle mainBundle], @"Version %d.%d.%d of the eID Viewer is available; you are running an older version. Please go to %@ to download the update", "message shown when a new version of the viewer is available"), [newVersion major], [newVersion minor], [newVersion build], [info upgradeUrl]];
                        [alert runModal];
                }
        }
}

- (IBAction)changeUpdatePref:(NSButton *)sender {
        NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
        [prefs setBool:([sender state] == NSOnState) ? YES : NO forKey:@"check_update"];
}
@end
