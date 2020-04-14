//
//  CertificateStore.m
//  eID Viewer
//
//  Created by buildslave on 29/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import "CertificateStore.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

@implementation CertificateStore
-(id)initWithOutlineView:(NSOutlineView *)view {
	if(self = [super init]) {
		_CertificateData = [[NSMutableDictionary alloc] init];
	}
	[self setOv:view];
	return self;
}
-(void)handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
	X509 *cert = NULL;
	BIO *bio = BIO_new(BIO_s_mem());
	char *buf;
#ifndef APPLE_INTERNAL_CRAP
	char errbuf[NAME_MAX];
#endif /* APPLE_INTERNAL_CRAP */
	size_t size = data.length;
	unsigned char *bytes = malloc(size);
	unsigned char *bytes_b = bytes;
	id arr[CERT_COL_NCOLS];

	_ui = ui;

	for(int i=0; i<CERT_COL_NCOLS; i++) {
		arr[i] = @"";
	}
	while(ERR_get_error()) {
		continue;
	}
	arr[CERT_COL_DATA] = data;
	[data getBytes:bytes length:size];
	if(d2i_X509(&cert, (const unsigned char**)&bytes, size) == NULL) {
#ifndef APPLE_INTERNAL_CRAP
		ERR_load_crypto_strings();
		unsigned long err;
		[_ui log:[[NSString alloc] initWithFormat:@"Could not parse %@ certificate:", label] withLevel:eIDLogLevelCoarse];
		while((err = ERR_get_error()) > 0) {
			[_ui log:[[NSString alloc] initWithFormat:@"... %s", ERR_error_string(err, errbuf)] withLevel:eIDLogLevelCoarse];
		}
#else /* APPLE_INTERNAL_CRAP */
        [_ui log:[[NSString alloc] initWithFormat:@"Could not parse %@ certificate:", label] withLevel:eIDLogLevelCoarse];
#endif /* !APPLE_INTERNAL_CRAP */

		return;
	}
	arr[CERT_COL_LABEL] = [NSString stringWithCString:eid_vwr_describe_cert(label.UTF8String, cert) encoding:NSUTF8StringEncoding];

	ASN1_TIME_print(bio, X509_get_notBefore(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)) + 1);
	BIO_read(bio, buf, (int)size);
	buf[size]='\0';
	arr[CERT_COL_VALIDFROM] = [NSString stringWithCString:buf encoding:NSUTF8StringEncoding];
	free(buf);

	ASN1_TIME_print(bio, X509_get_notAfter(cert));
	buf = malloc((size = BIO_ctrl_pending(bio)) + 1);
	BIO_read(bio, buf, (int)size);
	buf[size]='\0';
	arr[CERT_COL_VALIDTO] = [NSString stringWithCString:buf encoding:NSUTF8StringEncoding];
	free(buf);

	arr[CERT_COL_DESC] = [NSString stringWithCString:eid_vwr_detail_cert(label.UTF8String, cert) encoding:NSUTF8StringEncoding];
	arr[CERT_COL_USE] = [NSString stringWithCString:eid_vwr_get_use_flags(label.UTF8String, cert) encoding:NSUTF8StringEncoding];
	arr[CERT_COL_VALIDITY] = @(eIDResultUnknown);
	NSString *str = [[NSBundle mainBundle] pathForResource:@"certificate_large" ofType:@"png"];
	NSImage *img = [[NSImage alloc] initByReferencingFile:str];
    if(!img) {
        img = [[NSImage alloc] initWithSize:NSMakeSize(10, 10)];
    }
	arr[CERT_COL_IMAGE] = img;
	[self.CertificateData setObject:[NSMutableArray arrayWithObjects:arr count:CERT_COL_NCOLS] forKey:label];

	BIO_free(bio);
	free(bytes_b);
	[_ov reloadData];
	[_ov expandItem:nil expandChildren:YES];
}
-(BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item {
	if([item isEqualToString:@"Root"]) {
		return YES;
	}
	if([item isEqualToString:@"CA"]) {
		return YES;
	}
	return NO;
}
-(NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item {
	if(item == nil) {
		if([self.CertificateData objectForKey:@"Root"] != nil) {
			return 1;
		} else {
			return 0;
		}
	}
	if([item isEqualToString:@"Authentication"] ||
	   [item isEqualToString:@"Signature"] ||
	   [item isEqualToString:@"CERT_RN_FILE"]) {
		return 0;
	}
	int count=0;
	if([item isEqualToString:@"Root"]) {
		if([self.CertificateData objectForKey:@"CA"] != nil) {
			count++;
		}
		if([self.CertificateData objectForKey:@"CERT_RN_FILE"] != nil) {
			count++;
		}
	}else if([item isEqualToString:@"CA"]) {
		if([self.CertificateData objectForKey:@"Authentication"] != nil) {
			count++;
		}
		if([self.CertificateData objectForKey:@"Signature"] != nil) {
			count++;
		}
	}
	return count;
}
-(id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item {
	if([item isEqualToString:@"Authentication"] ||
	   [item isEqualToString:@"Signature"] ||
	   [item isEqualToString:@"CERT_RN_FILE"]) {
		return nil;
	}
	id arr[2] = { nil, nil };
	int i=0;
	id o;
	if(item == nil) {
		o = [self.CertificateData objectForKey:@"Root"];
		if(o != nil) {
			arr[i++] = @"Root";
		}
	}
	else if([item isEqualToString:@"Root"]) {
		o = [self.CertificateData objectForKey:@"CERT_RN_FILE"];
		if (o != nil) {
			arr[i++] = @"CERT_RN_FILE";
		}
		o = [self.CertificateData objectForKey:@"CA"];
		if (o != nil) {
			arr[i++] = @"CA";
		}
	}
	else if([item isEqualToString:@"CA"]) {
		o = [self.CertificateData objectForKey:@"Authentication"];
		if (o != nil) {
			arr[i++] = @"Authentication";
		}
		o = [self.CertificateData objectForKey:@"Signature"];
		if (o != nil) {
			arr[i++] = @"Signature";
		}
	}
	if(index >= i) {
		return nil;
	}
	return arr[index];
}
-(id)objectValueForField:(NSUInteger)index byItem:(id)item {
	return [[self.CertificateData objectForKey:item] objectAtIndex:index];
}
-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
	// There's only one column, really
	return [[_CertificateData objectForKey:item] objectAtIndex:CERT_COL_LABEL];
}
-(NSView*)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	NSTableCellView *cell = [outlineView makeViewWithIdentifier:[tableColumn identifier] owner:_ui];
	NSTextField *field = [cell textField];
	[field setStringValue:[[_CertificateData objectForKey:item] objectAtIndex:CERT_COL_LABEL]];
	NSImageView *img = [cell imageView];
	[img setImage:[[_CertificateData objectForKey:item] objectAtIndex:CERT_COL_IMAGE]];

	return cell;
}
-(void)outlineViewSelectionDidChange:(NSNotification *)notification {
	NSString* key = [_ov itemAtRow:[_ov selectedRow]];
	NSArray* arr = [_CertificateData objectForKey:key];
	[_ui newstringdata:[arr objectAtIndex:CERT_COL_DESC] withLabel:@"certdata"];
	[_ui newstringdata:[arr objectAtIndex:CERT_COL_VALIDFROM] withLabel:@"certvalfromval"];
	[_ui newstringdata:[arr objectAtIndex:CERT_COL_VALIDTO] withLabel:@"certvaltilval"];
	[_ui newstringdata:[arr objectAtIndex:CERT_COL_USE] withLabel:@"certuseval"];
	[_ui newbindata:[arr objectAtIndex:CERT_COL_IMAGE] withLabel:@"certimage"];
	NSString *trust;
	eIDResult res = arr == nil ? eIDResultUnknown : [[arr objectAtIndex:CERT_COL_VALIDITY] integerValue];
	switch(res) {
		case eIDResultUnknown:
			trust = @"";
			break;
		case eIDResultSuccess:
			trust = @"Valid";
			break;
		case eIDResultFailed:
			trust = @"NOT VALID";
			break;
	}
	[_ui newstringdata:trust withLabel:@"certtrustval"];
}
-(NSData*) certificateForKey:(NSString*)key {
	return [[_CertificateData objectForKey:key] objectAtIndex:CERT_COL_DATA];
}
-(void)dumpFile:(int)fd forKey:(NSString*)key withFormat:(eIDDumpType)format {
	NSData *dat = [self certificateForKey:key];
	eid_vwr_dumpcert(fd, [dat bytes], (int)[dat length], (enum dump_type)format);
}
-(NSString*)fileNameForKey:(NSString*)key {
	NSString* label = [[_CertificateData objectForKey:key] objectAtIndex:CERT_COL_LABEL];
	NSUInteger len = [label length];
	char string[len+1];
	memcpy(string, [label cStringUsingEncoding:NSUTF8StringEncoding], len);
	string[len]='\0';
	int d=0;
	for(int s=0; s<len;s++) {
		string[d] = tolower(string[s]);
		switch(string[s]) {
			case '(':
			case ')':
				break;
			case ' ':
				string[d]='_';
			default:
				d++;
		}
	}
	string[d]='\0';
	NSString *rv = [NSString stringWithCString:string encoding:NSUTF8StringEncoding];
	return rv;
}
-(NSString*)keyForParent:(NSString *)key {
	if([key isEqualToString:@"CERT_RN_FILE"] || [key isEqualToString:@"CA"]) {
		return @"Root";
	}
	if([key isEqualToString:@"Authentication"] || [key isEqualToString:@"Signature"]) {
		return @"CA";
	}
	return nil;
}
-(void)clear {
	[_CertificateData removeAllObjects];
	[_ov reloadData];
}
-(void)setValid:(eIDResult)valid forKey:(NSString *)key {
	NSMutableArray* arr = [_CertificateData objectForKey:(key)];
	if(arr == nil) {
		return;
	}
	arr[CERT_COL_VALIDITY] = @(valid);
	NSString *name;
	switch(valid) {
		case eIDResultFailed:
			name = @"certificate_bad";
			break;
		case eIDResultSuccess:
			name = @"certificate_checked";
			break;
		case eIDResultUnknown:
			name = @"certificate_warn";
			break;
	}
	NSString *filename = [[NSBundle mainBundle] pathForResource:name ofType:@"png"];
	NSImage *img = [[NSImage alloc] initByReferencingFile:filename];
	arr[CERT_COL_IMAGE] = img;
	[_ov reloadData];
	[self outlineViewSelectionDidChange:[NSNotification notificationWithName:@"validation" object:_ov]];
}
@end
