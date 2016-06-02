#import <Foundation/Foundation.h>
#include "trustdirname.h"

const char* eid_vwr_osl_objc_trustdirname() {
	static char pathname[PATH_MAX];
	NSBundle *mainBundle = [NSBundle mainBundle];
	NSString *str = [mainBundle pathForResource:@"certs" ofType:nil];
	[str getCString:pathname maxLength:PATH_MAX encoding:NSUTF8StringEncoding];
	return pathname;
}