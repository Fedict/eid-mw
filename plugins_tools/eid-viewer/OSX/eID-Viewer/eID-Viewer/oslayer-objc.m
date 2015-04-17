#import "oslayer-objc.h"

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

@implementation eIDOSLayerBackend
+(NSInteger)pinop:(eIDPinOp)which {
    return (NSInteger)eid_vwr_pinop((enum eid_vwr_pinops)which);
}
+(NSInteger)setUi:(NSObject *)ui {
    struct eid_vwr_ui_callbacks *cb = eid_vwr_cbstruct();
    cb->log = osl_objc_log;
    cb->newbindata = osl_objc_newbindata;
    cb->newsrc = osl_objc_newsrc;
    cb->newstate = osl_objc_newstate;
    cb->newstringdata = osl_objc_newstringdata;
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
@end