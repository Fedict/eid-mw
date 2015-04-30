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
#include <certhelpers.h>

@implementation CertificateStore
-(id)init {
    if(self = [super init]) {
        _CertificateData = [[NSMutableDictionary alloc] init];
    }
    return self;
}
-(void)handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui {
    X509 *cert = NULL;
    BIO *bio = BIO_new(BIO_s_mem());
    char *buf;
    char errbuf[120];
    size_t size = data.length;
    unsigned char *bytes = malloc(size);
    unsigned char *bytes_b = bytes;
    id arr[CERT_COL_NCOLS];

    for(int i=0; i<CERT_COL_NCOLS; i++) {
        arr[i] = @"";
    }
    while(ERR_get_error()) {
        continue;
    }
    [data getBytes:bytes length:size];
    if(d2i_X509(&cert, &bytes, size) == NULL) {
        ERR_load_crypto_strings();
        unsigned long err;
        [ui log:[[NSString alloc] initWithFormat:@"Could not parse %@ certificate:", label] withLevel:eIDLogLevelCoarse];
        while((err = ERR_get_error()) > 0) {
            [ui log:[[NSString alloc] initWithFormat:@"... %s", ERR_error_string(err, errbuf)] withLevel:eIDLogLevelCoarse];
        }
        return;
    }
    arr[CERT_COL_LABEL] = [NSString stringWithCString:describe_cert(label.UTF8String, cert) encoding:NSUTF8StringEncoding];

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

    arr[CERT_COL_DESC] = [NSString stringWithCString:describe_cert(label.UTF8String, cert) encoding:NSUTF8StringEncoding];
    arr[CERT_COL_USE] = [NSString stringWithCString:get_use_flags(label.UTF8String, cert) encoding:NSUTF8StringEncoding];
    [self.CertificateData setObject:[NSArray arrayWithObjects:arr count:CERT_COL_NCOLS] forKey:label];

    BIO_free(bio);
    free(bytes_b);
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
-(id)objectValueForField:(NSUInteger)index byItem:(id)item {
    return [[self.CertificateData objectForKey:item] objectAtIndex:index];
}
-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
    // we only have one column, really
    return item;
}
@end
