//
//  PrintOperation.m
//  eID Viewer
//
//  Created by AFI-DMAC130201 on 7/05/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

@import AppKit;
#import "PrintOperation.h"
#import "ResizablePrintView.h"
#import <BeidView/oslayer-objc.h>
#include <time.h>
#include <eid-util/labels.h>

@implementation PrintOperation
-(instancetype)initWithView:(NSView *)view app:(AppDelegate *)app {
	self = [super init];
	[self setView:view];
	[self setApp:app];
	[self setViewDict:[[NSMutableDictionary alloc]init]];
	if (![self isIndexed]) {
		[self indexViews:view];
		[self setIsIndexed:YES];
	}
	[self setPrintableFields:[[NSArray alloc] initWithObjects:@"surname", @"photo", @"location_of_birth", @"date_of_birth", @"gender", @"nationality", @"national_number", @"nobility", @"special_status", @"address_street_and_number", @"address_zip", @"address_municipality", @"chip_number", @"issuing_municipality", @"card_number", @"validity_begin_date", @"validity_end_date", @"document_type", nil]];
	struct labelnames *l = get_foreigner_labels();
	NSMutableArray* arr = [NSMutableArray arrayWithCapacity:l->len];
	for(int i=0; i<l->len; i++) {
		[arr addObject:[NSString stringWithUTF8String:l->label[i]]];
	}
	[self setForeignerFields:arr];
	return self;
}
-(void)indexViews:(NSView*)view {
	if(![view conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
		return;
	}
	NSObject<NSUserInterfaceItemIdentification>*from = (NSObject<NSUserInterfaceItemIdentification>*)view;
	NSString *key = [from identifier];
	if(key == nil) return;
	[_viewDict setObject:from forKey:key];
	NSArray* arr = [view subviews];
	for(int i=0;i<[arr count]; i++) {
		[self indexViews:[arr objectAtIndex:i]];
	}
}
-(BOOL)copyDataWithArray:(NSArray*)arr {
	for(int i=0; i<[arr count]; i++) {
		NSString* name = [arr objectAtIndex:i];
		NSView* d = [_viewDict objectForKey:name];
		NSObject* s = [_app searchObjectById:name ofClass:[d class] forUpdate:NO];
		if([s isKindOfClass:[NSTextField class]]) {
			NSTextField *src = (NSTextField*)s;
			NSTextField *dest = (NSTextField*)d;
			[dest setStringValue:[src stringValue]];
		} else if([s isKindOfClass:[NSImageView class]]) {
			NSImageView *src = (NSImageView*)s;
			NSImageView *dest = (NSImageView*)d;
			[dest setImage:[src image]];
		} else if([s isKindOfClass:[NSButton class]]){
			NSButton *src = (NSButton*)s;
			NSButton *dest = (NSButton*)d;
			[dest setState:[src state]];
		} else {
			[_app log:[NSString stringWithFormat:@"Could not find field for %@; print operation cancelled", name] withLevel:eIDLogLevelError];
			return NO;
		}
	}
	return YES;
}
-(BOOL)runOperation {
	NSTextField *o = [_viewDict objectForKey:@"firstnames"];
	NSTextField *fn = (NSTextField*)[_app searchObjectById:@"firstnames" ofClass:[NSTextField class] forUpdate:NO];
	NSTextField *ftn = (NSTextField*)[_app searchObjectById:@"first_letter_of_third_given_name" ofClass:[NSTextField class] forUpdate:NO];
	[o setStringValue:[NSString stringWithFormat:@"%@ %@", [fn stringValue], [ftn stringValue]]];
	NSString *seal;
	NSString *ctry;
	if(![self copyDataWithArray:_printableFields]) {
		return NO;
	}
	if([[self app]isForeignerCard] && ![self copyDataWithArray:_foreignerFields]) {
		return NO;
	}
	switch([eIDOSLayerBackend lang]) {
		case eIDLanguageDe:
			seal = @"coat_of_arms_de";
			ctry = @"BELGIEN";
			break;
		case eIDLanguageFr:
			seal = @"coat_of_arms_fr";
			ctry = @"BELGIQUE";
			break;
		case eIDLanguageNl:
			seal = @"coat_of_arms_nl";
			ctry = @"BELGIË";
			break;
		default:
			seal = @"coat_of_arms_en";
			ctry = @"BELGIUM";
			break;
	}
	[[_viewDict objectForKey:@"seal"] setImage:[NSImage imageNamed:seal]];
	[[_viewDict objectForKey:@"country"] setStringValue:ctry];
	time_t t = time(NULL);
	[[_viewDict objectForKey:@"printdate"] setStringValue:[NSString stringWithCString:ctime(&t) encoding:NSUTF8StringEncoding]];
	[[_viewDict objectForKey:@"printby"] setStringValue:[NSString stringWithFormat:@"eID Viewer %@ (OS X)", [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"]]];
	return [[NSPrintOperation printOperationWithView:_view] runOperation];
}
@end
