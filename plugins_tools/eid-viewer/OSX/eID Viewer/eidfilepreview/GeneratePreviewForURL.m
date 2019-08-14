#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "oslayer.h"
@import AppKit;

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
 Generate a preview for file

 This function's job is to create preview for designated file
 ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
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
		QLPreviewRequestSetDataRepresentation(preview, dat, kUTTypeImage, nil);
		NSLog(@"ok");
		CFRelease(dat);
		eid_vwr_release_preview(prev);
	}
	return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
	// Implement only if supported
}
