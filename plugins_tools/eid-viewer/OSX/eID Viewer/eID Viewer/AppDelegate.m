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
- (IBAction)setLanguage:(NSMenuItem *)sender;
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

@property CertificateStore *certstore;
@property NSDictionary *bindict;
@property NSMutableDictionary *viewdict;
@property NSArray *readerSelections;
@property (weak) IBOutlet NSImageView *photoview;
@property (weak) IBOutlet NSImageView *certview;
@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSWindow *CardReadSheet;
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
@property (weak) IBOutlet NSMenu *menu_file_reader;
@property (weak) IBOutlet NSMenuItem *menu_file_reader_auto;
@property (weak) IBOutlet NSSegmentedControl *pinop_ctrl;
@property (unsafe_unretained) IBOutlet NSTextView *CertDetailView;
@property (weak) IBOutlet NSWindow *CertDetailSheet;
@property (weak) IBOutlet NSBox *centeringLine;
@property (weak) IBOutlet NSBox *bottomLine;
@property (weak) IBOutlet NSTextField *lowestItem;
@property (weak) IBOutlet NSLayoutConstraint *verticalLineBottomConstraint;
@property (weak) IBOutlet NSButton *memberOfFamilyState;

@end

@implementation AppDelegate
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}
- (void)log:(NSString *)line withLevel:(eIDLogLevel)level {
	if([self.logLevel indexOfSelectedItem] > level) {
		return;
	}
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
		[_photoview setImage:nil];
		[_certview setImage:nil];
		[_certstore clear];
		[_viewdict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop){
			if(![obj isKindOfClass:[NSTextField class]]) {
				return;
			}
			NSTextField* tf = (NSTextField*)obj;
			[tf setStringValue:@""];
		}];
		[_memberOfFamilyState setState:NSOffState];
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
	BOOL fileOpen = NO;
	BOOL filePrint = NO;
	BOOL fileSave = NO;
	BOOL fileClose = NO;
	BOOL pinops = NO;
	BOOL validate = NO;
	BOOL doValidateNow = NO;
	BOOL sheet = NO;
	BOOL dnd = NO;
	switch(state) {
		case eIDStateReady:
			fileOpen = YES;
			break;
		case eIDStateNoReader:
			fileOpen = YES;
			break;
		case eIDStateToken:
		case eIDStateTokenID:
		case eIDStateTokenCerts:
			sheet = YES;
			break;
		case eIDStateTokenWait:
			filePrint = YES;
			fileSave = YES;
			pinops = YES;
			validate = YES;
			dnd = YES;
			if([_alwaysValidate state] == NSOnState) {
				doValidateNow = YES;
			}
			break;
		case eIDStateFileWait:
			fileClose = YES;
			filePrint = YES;
			dnd = YES;
			if([_alwaysValidate state] == NSOnState) {
				doValidateNow = YES;
			}
		default:
			break;
	}
	[[NSOperationQueue mainQueue] addOperationWithBlock:^{
		[_menu_file_print setEnabled:filePrint];
		[_menu_file_open setEnabled:fileOpen];
		[_menu_file_close setEnabled:fileClose];
		[_menu_file_save setEnabled: fileSave];
		[_pinop_ctrl setEnabled:pinops];
		[_alwaysValidate setEnabled:validate];
		[_validateNow setEnabled:validate];
		if(sheet) {
			[_spinner startAnimation:self];
			[NSApp beginSheet:_CardReadSheet modalForWindow:_window modalDelegate:self didEndSelector:@selector(endSheet:returnCode:contextInfo:) contextInfo:nil];
		} else {
			[NSApp endSheet:_CardReadSheet];
			[_spinner stopAnimation:self];
		}
		if(doValidateNow) {
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
	[NSApp beginSheet:_CertDetailSheet modalForWindow:_window modalDelegate:self didEndSelector:@selector(endSheet:returnCode:contextInfo:) contextInfo:nil];
	[_CertDetailView selectAll:nil];
	[_CertDetailView delete:nil];
	[_CertDetailView setEditable:YES];
	[_CertDetailView insertText:details];
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
	_bindict = [[NSDictionary alloc] initWithObjectsAndKeys:[photohandler alloc], @"PHOTO_FILE",
		    _certstore, @"Root",
		    _certstore, @"CA",
		    _certstore, @"Authentication",
		    _certstore, @"Signature",
		    _certstore, @"CERT_RN_FILE",
		    self, @"certimage",
		    self, @"document_type_raw",
		    self, @"member_of_family",
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
	BOOL alw_val = [prefs boolForKey:@"always_validate"];
	if(alw_val) {
		[_alwaysValidate setState:NSOnState];
	} else {
		[_alwaysValidate setState:NSOffState];
	}
	[self setIsForeignerCard:NO];
	[_logLevel selectItemAtIndex:level];
	[eIDOSLayerBackend setLang:langcode];
	[eIDOSLayerBackend mainloopThread];
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
	[self log:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"LanguageChosenLog", nil, [NSBundle mainBundle], @"Setting language to %@", ""), keyeq] withLevel:eIDLogLevelNormal];
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
-(void)handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
	assert(ui == self);
	if([label isEqualToString:@"certimage"]) {
		[(NSImageView*) [ui searchObjectById:label ofClass:[NSImageView class] forUpdate:NO] setImage:(NSImage*)data];
		return;
	}
	if([label isEqualToString:@"member_of_family"]) {
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			[_memberOfFamilyState setState:NSOnState];
		}];
	}
	if([label isEqualToString:@"document_type_raw"]) {
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
				[_IdentityTab removeConstraint:_verticalLineBottomConstraint];
				if([self isForeignerCard]) {
					_verticalLineBottomConstraint = [NSLayoutConstraint constraintWithItem:_centeringLine attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:_lowestItem attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0.0];
				} else {
					_verticalLineBottomConstraint = [NSLayoutConstraint constraintWithItem:_centeringLine attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:_bottomLine attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0.0];
				}
				[_IdentityTab addConstraint:_verticalLineBottomConstraint];
				[_IdentityTab layoutSubtreeIfNeeded];
			}];
		}
	}
}
-(void)changeLogLevel:(NSPopUpButton *)logLevel {
	[[NSUserDefaults standardUserDefaults] setInteger:[logLevel indexOfSelectedItem] forKey:@"log_level"];
}
-(IBAction)validateNow:(id)sender {
	NSData* ca = [_certstore certificateForKey:@"CA"];
	eIDResult resSig = [eIDOSLayerBackend validateCert:[_certstore certificateForKey:@"Signature"] withCa:ca];
	eIDResult resAuth = [eIDOSLayerBackend validateCert:[_certstore certificateForKey:@"Authentication"] withCa:ca];
	eIDResult resRRN = [eIDOSLayerBackend validateRrnCert:[_certstore certificateForKey:@"CERT_RN_FILE"]];
	eIDResult resParents;
	if(resSig == resAuth) {
		resParents = resSig;
	} else {
		if(resSig < resAuth) {
			resParents = resSig;
		} else {
			resParents = resAuth;
		}
	}
	[_certstore setValid:resParents forKey:@"Root"];
	[_certstore setValid:resParents forKey:@"CA"];
	[_certstore setValid:resRRN forKey:@"CERT_RN_FILE"];
	[_certstore setValid:resAuth forKey:@"Authentication"];
	[_certstore setValid:resSig forKey:@"Signature"];
	if(resParents == eIDResultFailed || resRRN == eIDResultFailed) {
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
	[_menu_file_reader_auto setState: NSOffState];
	for(int i=0; i<[_readerSelections count]; i++) {
		[[_readerSelections objectAtIndex:i] setState: NSOffState];
	}
	[sender setState:NSOnState];
	[eIDOSLayerBackend selectReader:[sender slotNumber]];
}
-(void)readersFound:(NSArray *)readers withSlotNumbers:(NSArray *)slots {
	NSInteger count = [readers count];
	assert(count == [slots count]);
	ReaderMenuItem* newreaders[count];
	for(int i=0; i<count; i++) {
		NSInteger slot = [[slots objectAtIndex:i]integerValue];
		NSString *readerName = [readers objectAtIndex:i];
		newreaders[i] = [[ReaderMenuItem alloc] initWithTitle:readerName action:@selector(selectManualReader:) keyEquivalent:@"" slotNumber:slot];
		[_menu_file_reader addItem:newreaders[i]];
	}
	[_menu_file_reader_auto setEnabled:count > 0 ? YES : NO];
	_readerSelections = [NSArray arrayWithObjects:newreaders count:count];
}

- (IBAction)closeDetail:(id)sender {
	[NSApp endSheet:_CertDetailSheet];
}
@end
