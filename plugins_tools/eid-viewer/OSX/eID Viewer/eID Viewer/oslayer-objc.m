#import "oslayer-objc.h"
#include <state.h>
#include <pthread.h>
#include <verify_cert.h>
#include "dataverify.h"
#include "p11.h"

static id <eIDOSLayerUI>currUi = nil;

static void osl_objc_newsrc(enum eid_vwr_source src) {
	[currUi newsrc:(eIDSource)src];
}

static void osl_objc_newstringdata(const char* label, const char* data) {
	[currUi newstringdata:[NSString stringWithCString:data encoding:NSUTF8StringEncoding] withLabel:[NSString stringWithCString:label encoding:NSUTF8StringEncoding]];
}

static void osl_objc_newbindata(const char* label, const unsigned char* data, int datalen) {
	[currUi newbindata:[NSData dataWithBytes:data length:datalen] withLabel:[NSString stringWithCString:label encoding:NSUTF8StringEncoding]];
}

static void osl_objc_log(enum eid_vwr_loglevel level, const char* line) {
	[currUi log:[NSString stringWithCString:line encoding:NSUTF8StringEncoding]withLevel:(eIDLogLevel) level];
}

static void osl_objc_newstate(enum eid_vwr_states state) {
	[currUi newstate:(eIDState)state];
}

static void osl_objc_pinop_result(enum eid_vwr_pinops p, enum eid_vwr_result r) {
	[currUi pinop_result:(eIDResult)r forOperation:(eIDPinOp)p];
}

static void osl_objc_readers_found(unsigned long nreaders, slotdesc* slots) {
	NSString* names[nreaders];
	NSNumber* slotnumbers[nreaders];
	for(int i=0; i<nreaders; i++) {
		if(slots[i].description == NULL) {
			names[i] = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"cardreaderno", nil, [NSBundle mainBundle], @"Card reader #%d", "Fallback string for card reader name in case none was provided")];
		} else {
			names[i] = [NSString stringWithCString:(const char*)(slots[i].description) encoding:NSUTF8StringEncoding];
		}
		slotnumbers[i] = [NSNumber numberWithUnsignedLong:slots[i].slot];
	}
	NSArray *readerNames = [NSArray arrayWithObjects:names count:nreaders];
	NSArray *slotNumbers = [NSArray arrayWithObjects:slotnumbers count:nreaders];
	[currUi readersFound:readerNames withSlotNumbers:slotNumbers];
}

#if MAC_OS_X_VERSION_MIN_REQUIRED >= 101100
static NSData *urlSendSynchronousRequest(NSURLRequest *request, NSURLResponse *__autoreleasing *responsePointer, NSError *__autoreleasing *errorPointer)
{
    dispatch_semaphore_t semaphore;
    __block NSData *result = nil;

    semaphore = dispatch_semaphore_create(0);

    void (^completionHandler)(NSData * __nullable data, NSURLResponse * __nullable response, NSError * __nullable error);
    completionHandler = ^(NSData * __nullable data, NSURLResponse * __nullable response, NSError * __nullable error)
    {
        if ( errorPointer != NULL )
        {
            *errorPointer = error;
        }

        if ( responsePointer != NULL )
        {
            *responsePointer = response;
        }

        if ( error == nil )
        {
            result = data;
        }

        dispatch_semaphore_signal(semaphore);
    };

    [[[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:completionHandler] resume];

    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

    return result;
}
#endif

static const void* osl_objc_perform_ocsp_request(char* url, void* data, long len, long* retlen, void** handle) {
	NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
	NSData* dat = [NSData dataWithBytes:data length:len];
	[req setValue:@"application/ocsp-request" forHTTPHeaderField: @"Content-Type"];
	req.HTTPBody = dat;
	req.HTTPMethod = @"POST";
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 101100
    NSURLResponse *resp = nil;
    NSError *err = nil;
    NSData *respdata = urlSendSynchronousRequest(req, &resp, &err);
#else
	NSURLResponse* resp;
	NSError *err;
	NSData *respdata = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
#endif
	if (respdata != nil) {
		void *retval = malloc(respdata.length);
		*retlen = (long)respdata.length;
		*handle = retval;
		[respdata getBytes:retval length:*retlen];
		return retval;
	} else {
		return NULL;
	}
}

static const void* osl_objc_perform_http_request(char* url, long *retlen, void** handle) {
	NSURLRequest* req = [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
	NSURLResponse *resp;
	NSError *err;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 101100
    NSData *respdata = urlSendSynchronousRequest(req, &resp, &err);
#else
	NSData *respdata = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
#endif
	if(respdata != nil) {
		void *retval = malloc(respdata.length+1);
		*retlen = (long)respdata.length;
		*handle = retval;
		[respdata getBytes:retval length:*retlen];
		return retval;
	} else {
		return NULL;
	}
}

static void osl_objc_free_ocsp_request(void* data) {
	free(data);
}

static void osl_objc_challenge_result(const unsigned char *response, int responselen, enum eid_vwr_result result) {
	NSData *resp = [NSData dataWithBytes:response length:responselen];
	eIDResult res = (eIDResult) result;
	[currUi challengeResult:res withResponse:resp];
}

@implementation eIDOSLayerBackend
+(void)pinop:(eIDPinOp)which {
	eid_vwr_pinop((enum eid_vwr_pinops)which);
}
+(NSInteger)setUi:(id<eIDOSLayerUI>)ui {
	NSInteger rv;
	struct eid_vwr_ui_callbacks *cb = eid_vwr_cbstruct();
	if([ui respondsToSelector:@selector(log:withLevel:)]) {
		cb->log = osl_objc_log;
	}
	if([ui respondsToSelector:@selector(newbindata:withLabel:)]) {
		cb->newbindata = osl_objc_newbindata;
	}
	if([ui respondsToSelector:@selector(newsrc:)]) {
		cb->newsrc = osl_objc_newsrc;
	}
	if([ui respondsToSelector:@selector(newstate:)]) {
		cb->newstate = osl_objc_newstate;
	}
	if([ui respondsToSelector:@selector(newstringdata:withLabel:)]) {
		cb->newstringdata = osl_objc_newstringdata;
	}
	if([ui respondsToSelector:@selector(pinop_result:forOperation:)]) {
		cb->pinop_result = osl_objc_pinop_result;
	}
	if([ui respondsToSelector:@selector(readersFound:withSlotNumbers:)]) {
		cb->readers_changed = osl_objc_readers_found;
	}
	if([ui respondsToSelector:@selector(useDefaultChallengeResult)]) {
		cb->challenge_result = eid_vwr_challenge_result;
	} else if([ui respondsToSelector:@selector(challengeResult:withResponse:)]) {
		cb->challenge_result = osl_objc_challenge_result;
	}
	currUi = ui;
	rv = eid_vwr_createcallbacks(cb);
	eid_vwr_init_crypto();
	return rv;
}
+(NSImage*)getPreview:(NSURL *)from {
	if(![from isFileReferenceURL]) {
		return NULL;
	}
	char buf[PATH_MAX];
	[from getFileSystemRepresentation:buf maxLength:PATH_MAX];
	struct eid_vwr_preview* prv = eid_vwr_get_preview(buf);
	if(!prv->have_data) {
		return NULL;
	}
	return [[NSImage alloc] initWithData:[NSData dataWithBytes:prv->imagedata length:prv->imagelen]];
}
+(void)mainloop {
	eid_vwr_be_mainloop(NULL);
}
+(void)poll {
	eid_vwr_poll();
}
+(void)deserialize:(NSURL*)from {
	eid_vwr_be_deserialize(from.fileSystemRepresentation);
}
+(void)serialize:(NSURL*)target {
	eid_vwr_be_serialize(target.fileSystemRepresentation);
}
+(void)setLang:(eIDLanguage)language {
	enum eid_vwr_langs l = (enum eid_vwr_langs) language;
	eid_vwr_convert_set_lang(l);
}
+(void)closeFile {
	sm_handle_event(EVENT_CLOSE_FILE, NULL, NULL, NULL);
}
+(void)setInvalid {
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
}
+(NSData*)xmlform {
	const char* xml = eid_vwr_be_get_xmlform();
	if(!xml) return nil;
	return [NSData dataWithBytes:xml length:strlen(xml)];
}
+(eIDResult)validateCert:(NSData*)certificate withCa:(NSData*)ca {
	if(certificate == nil || ca == nil) {
		return eIDResultUnknown;
	}
	return (eIDResult)eid_vwr_verify_cert([certificate bytes], [certificate length], [ca bytes], [ca length],osl_objc_perform_ocsp_request, osl_objc_free_ocsp_request);
}
+(eIDResult)validateIntCert:(NSData *)certificate withCa:(NSData *)ca {
	if(certificate == nil || ca == nil) {
		return eIDResultUnknown;
	}
	return (eIDResult)eid_vwr_verify_int_cert([certificate bytes], [certificate length], [ca bytes], [ca length], osl_objc_perform_http_request, osl_objc_free_ocsp_request);
}
+(eIDResult)validateRootCert:(NSData *)certificate {
	if(certificate == nil) {
		return eIDResultUnknown;
	}
	return (eIDResult)eid_vwr_verify_root_cert([certificate bytes], [certificate length]);
}
+(eIDResult)validateRrnCert:(NSData *)certificate withRoot:(NSData *) root {
	if(certificate == nil || root == nil) {
		return eIDResultUnknown;
	}
	return (eIDResult)eid_vwr_verify_rrncert([certificate bytes], [certificate length], [root bytes], [root length]);
}
+(void)setReaderAuto:(BOOL)automatic {
	eid_vwr_be_select_slot(automatic ? CK_TRUE : CK_FALSE, 0);
}
+(void)selectReader:(NSInteger)readerNumber {
	eid_vwr_be_select_slot(CK_FALSE, (CK_SLOT_ID)readerNumber);
}
+(eIDLanguage)lang {
	return (eIDLanguage)eid_vwr_convert_get_lang();
}
+(NSString*)getCertDetail:(NSData *)certificate {
	char* details = eid_vwr_x509_get_details([certificate bytes], [certificate length]);
	NSString* rv = [NSString stringWithUTF8String:details];
	free(details);
	return rv;
}
+(void)doChallenge:(NSData*)challenge {
	eid_vwr_challenge([challenge bytes], (int)[challenge length]);
}
+(void)doChallengeInternal {
	eid_vwr_maybe_perform_challenge();
}
@end
