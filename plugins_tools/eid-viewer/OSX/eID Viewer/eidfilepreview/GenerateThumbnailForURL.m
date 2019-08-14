#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "oslayer.h"
#include "backend.h"
@import AppKit;

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);

/* -----------------------------------------------------------------------------
 Generate a thumbnail for file

 This function's job is to create thumbnail for designated file as fast as possible
 ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
	@autoreleasepool {
		NSLog(@"generating a preview");
		NSLog(@"URL: %@", (__bridge NSURL*)url);
		const char *filename = [(__bridge NSURL*)url fileSystemRepresentation];
		NSLog(@"Filename: %s", filename);
		if(!filename) return noSuchIconErr;
		struct eid_vwr_preview *prev = eid_vwr_get_preview(filename);
		if(!prev->have_data) {
			NSLog(@"Could not load preview");
			return noErr;
		}
		CFDataRef dat = CFDataCreate(NULL, prev->imagedata, prev->imagelen);
		NSLog(@"data generated");
		QLThumbnailRequestSetImageWithData(thumbnail, dat, NULL);
		NSLog(@"ok");
		CFRelease(dat);
		eid_vwr_release_preview(prev);
	}
	return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
	// Implement only if supported
}

void be_log(enum eid_vwr_loglevel level, const char* line, ...) {
	va_list ap;
	va_start(ap, line);
	NSLogv([NSString stringWithCString:line encoding:NSUTF8StringEncoding], ap);
	va_end(ap);
}
