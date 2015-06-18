#import "oslayer-objc.h"
#include <state.h>
#include <pthread.h>

static id <eIDOSLayerUI>currUi = NULL;

static void osl_objc_newsrc(enum eid_vwr_source src) {
    [currUi newsrc:(eIDSource)src];
}

static void osl_objc_newstringdata(const char* label, const char* data) {
    [currUi newstringdata:[NSString stringWithCString:data encoding:NSUTF8StringEncoding] withLabel:[NSString stringWithCString:label encoding:NSUTF8StringEncoding]];
}

static void osl_objc_newbindata(const char* label, const void* data, int datalen) {
    [currUi newbindata:[NSData dataWithBytes:data length:datalen] withLabel:[NSString stringWithCString:label encoding:NSUTF8StringEncoding]];
}

static void osl_objc_log(enum eid_vwr_loglevel level, const char* line, va_list ap) {
    [currUi log:[[NSString alloc] initWithFormat:[NSString stringWithCString:line encoding:NSUTF8StringEncoding] arguments:ap] withLevel:(eIDLogLevel)level];
}

static void osl_objc_newstate(enum eid_vwr_states state) {
    [currUi newstate:(eIDState)state];
}

static void osl_objc_pinop_result(enum eid_vwr_pinops p, enum eid_vwr_result r) {
    [currUi pinop_result:(eIDResult)r forOperation:(eIDPinOp)p];
}

static void* threadmain(void* val) {
    eid_vwr_be_mainloop();
    
    assert(1 == 0); // we shouldn't ever get here...
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
+(NSString*)xmlform {
    return [NSString stringWithCString:eid_vwr_be_get_xmlform() encoding:NSUTF8StringEncoding];
}
@end