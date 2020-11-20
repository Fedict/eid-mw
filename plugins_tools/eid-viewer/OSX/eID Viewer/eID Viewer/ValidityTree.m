//
//  ValidityTree.m
//  eID Viewer
//
//  Created by Wouter Verhelst on 20/11/2020.
//

#import "ValidityTree.h"
#import <Cocoa/Cocoa.h>
#import <BeidView/oslayer-objc.h>

@implementation ValidityTree
- (instancetype)initWithOutlineView:(NSOutlineView *)view {
	self = [super init];
	if (self) {
		[self setOv:view];
		NSImage *img = [[NSImage alloc] initByReferencingFile:[[NSBundle mainBundle] pathForImageResource:@"status_unknown"]];
		if(!img) {
			img = [[NSImage alloc]initWithSize:NSMakeSize(10, 10)];
		}
		NSImage *valid = [[NSImage alloc] initByReferencingFile:[[NSBundle mainBundle] pathForImageResource:@"checkmark_large"]];
		NSImage *warning = [[NSImage alloc] initByReferencingFile:[[NSBundle mainBundle] pathForImageResource:@"warning_large"]];
		NSImage *fail = [[NSImage alloc] initByReferencingFile:[[NSBundle mainBundle] pathForImageResource:@"stop_large"]];
		NSImage *arr[4];
		arr[eIDResultWarning] = warning;
		arr[eIDResultSuccess] = valid;
		arr[eIDResultUnknown] = img;
		arr[eIDResultFailed] = fail;
		[self setImages:[NSArray arrayWithObjects:arr count:4]];
		NSArray *fields = @[ @"status", @"comments", @"children", @"item", ];
		NSMutableDictionary *basic_sign     = [NSMutableDictionary dictionaryWithObjects:@[ img, @"Untested", @[], @"Signature with basic key", ] forKeys:fields];
		NSMutableDictionary *basic_hash     = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[basic_sign], @"Basic key checksum", ] forKeys:fields];
		NSMutableDictionary *photo_hash     = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[], @"Photo file checksum", ] forKeys:fields];
		NSMutableDictionary *identity_sig   = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[basic_hash, photo_hash], @"Identity file signature", ] forKeys:fields];
		NSMutableDictionary *address_sig    = [NSMutableDictionary dictionaryWithObjects:@[ fail, @"RRN signature does not match", @[], @"Address file signature", ] forKeys:fields];
		NSMutableDictionary *rrn_cert       = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[identity_sig, address_sig], @"RRN certificate signature", ] forKeys:fields];
		NSMutableDictionary *auth_cert      = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[], @"Authentication certificate signature", ] forKeys:fields];
		NSMutableDictionary *signature_cert = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[], @"Signature certificate signature", ] forKeys:fields];
		NSMutableDictionary *ca_cert        = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[auth_cert, signature_cert], @"CA certificate signature", ] forKeys:fields];
		NSMutableDictionary *root_cert      = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[rrn_cert, ca_cert], @"Root certificate checksum", ] forKeys:fields];
		NSMutableDictionary *online_checks  = [NSMutableDictionary dictionaryWithObjects:@[ valid, @"OK", @[], @"Online certificate revocation checks", ] forKeys:fields];
		[self setTree:                        [NSMutableDictionary dictionaryWithObjects:@[ fail, @"One or more tests failed", @[online_checks, root_cert], @"Identity card validity", ] forKeys:fields]];
		NSDictionary *index = @{
			@"basic_sign_valid": basic_sign,
			@"basic_hash_valid": basic_hash,
			@"photo_hash_valid": photo_hash,
			@"identity_sign_valid": identity_sig,
			@"address_sign_valid": address_sig,
			@"rrn_sign_valid": rrn_cert,
			@"auth_sign_valid": auth_cert,
			@"signature_sign_valid": signature_cert,
			@"ca_sign_valid": ca_cert,
			@"root_known": root_cert,
			@"online_valid": online_checks,
			@"global_status": [self tree],
		};
		[self setIndex:index];
	}
	return self;
}
-(BOOL) outlineView:(NSOutlineView *)outlineView isItemExpandable:(NSDictionary *)item {
	NSArray *children = [item objectForKey:@"children"];
	if(!children || [children count] == 0) {
		return NO;
	}
	return YES;
}
-(NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(NSDictionary *)item {
	if(item == nil) {
		return 1;
	}
	return [[item objectForKey:@"children"]count];
}
-(id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item {
	if(item == nil) {
		return [self tree];
	}
	return [[item objectForKey:@"children"]objectAtIndex:index];
}
-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
	return [item objectForKey:[tableColumn identifier]];
}
-(void)setStatus:(eIDResult)status andComments:(NSString *)comments forItem:(NSString *)item {
	NSMutableDictionary *d = [[self index] objectForKey:item];
	[d setValue:[self images][status] forKey:@"status"];
	[d setValue:comments forKey:@"comments"];
	[[self ov] reloadItem:d];
}
@end
