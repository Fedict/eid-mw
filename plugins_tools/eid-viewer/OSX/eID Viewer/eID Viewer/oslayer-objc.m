#import "oslayer-objc.h"
#include <state.h>
#include <pthread.h>
#include "verify_cert.h"
#include "p11.h"

static id <eIDOSLayerUI>currUi = NULL;

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
		names[i] = [NSString stringWithCString:(const char*)(slots[i].description) encoding:NSUTF8StringEncoding];
		slotnumbers[i] = [NSNumber numberWithUnsignedLong:slots[i].slot];
	}
	NSArray *readerNames = [NSArray arrayWithObjects:names count:nreaders];
	NSArray *slotNumbers = [NSArray arrayWithObjects:slotnumbers count:nreaders];
	[currUi readersFound:readerNames withSlotNumbers:slotNumbers];
}

static void* threadmain(void* val) {
	eid_vwr_be_mainloop();

	assert(1 == 0); // we shouldn't ever get here...
}

static const void* osl_objc_perform_ocsp_request(char* url, void* data, long len, long* retlen, void** handle) {
	NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
	NSData* dat = [NSData dataWithBytes:data length:len];
	[req setValue:@"application/ocsp-request" forHTTPHeaderField: @"Content-Type"];
	req.HTTPBody = dat;
	req.HTTPMethod = @"POST";
	NSURLResponse* resp;
	NSError *err;
	NSData *respdata = [NSURLConnection sendSynchronousRequest:req returningResponse:&resp error:&err];
	if (respdata != nil) {
		*retlen = (long)respdata.length;
		*handle = (void*)CFBridgingRetain(respdata);
		return [respdata bytes];
	} else {
		return NULL;
	}
}

static void osl_objc_free_ocsp_request(void* data) {
	CFRelease(data);
}

@implementation eIDOSLayerBackend
+(NSInteger)pinop:(eIDPinOp)which {
	return (NSInteger)eid_vwr_pinop((enum eid_vwr_pinops)which);
}
+(NSInteger)setUi:(id<eIDOSLayerUI>)ui {
	struct eid_vwr_ui_callbacks *cb = eid_vwr_cbstruct();
	cb->log = osl_objc_log;
	cb->newbindata = osl_objc_newbindata;
	cb->newsrc = osl_objc_newsrc;
	cb->newstate = osl_objc_newstate;
	cb->newstringdata = osl_objc_newstringdata;
	cb->pinop_result = osl_objc_pinop_result;
	cb->readers_changed = osl_objc_readers_found;
	currUi = ui;
	return eid_vwr_createcallbacks(cb);
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
	eid_vwr_be_mainloop();
}
+(void)poll {
	eid_vwr_poll();
}
+(void)mainloop_thread {
	pthread_t thread;
	pthread_create(&thread, NULL, threadmain, NULL);
	pthread_detach(thread);
}
+(void)deserialize:(NSURL*)from {
	eid_vwr_be_deserialize(from.fileSystemRepresentation);
}
+(void)serialize:(NSURL*)target {
	eid_vwr_be_serialize(target.fileSystemRepresentation);
}
+(void)setLang:(eIDLanguage)language {
	enum eid_vwr_langs l = (enum eid_vwr_langs) language;
	convert_set_lang(l);
}
+(eIDLanguage)lang {
	eIDLanguage l = (eIDLanguage) convert_get_lang();
	return l;
}
+(void)close_file {
	sm_handle_event(EVENT_CLOSE_FILE, NULL, NULL, NULL);
}
+(void)set_invalid {
	sm_handle_event(EVENT_DATA_INVALID, NULL, NULL, NULL);
}
+(NSData*)xmlform {
	const char* xml = eid_vwr_be_get_xmlform();
	if(!xml) return nil;
	return [NSData dataWithBytes:xml length:strlen(xml)];
}
+(eIDResult)validateCert:(NSData*)certificate withCa:(NSData*)ca {
	return (eIDResult)eid_vwr_verify_cert([certificate bytes], [certificate length], [ca bytes], [ca length],osl_objc_perform_ocsp_request, osl_objc_free_ocsp_request);
}
+(void)setReaderAuto:(BOOL)automatic {
	eid_vwr_p11_select_slot(automatic ? CK_TRUE : CK_FALSE, 0);
}
+(void)selectReader:(NSInteger)readerNumber {
	eid_vwr_p11_select_slot(CK_FALSE, (CK_SLOT_ID)readerNumber);
}
@end