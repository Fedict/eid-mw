/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2015 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

 **************************************************************************** */

#import "AppDelegate.h"
#import "DataItem.h"
#include <sys/utsname.h>
#include <sys/stat.h>
#include "beidversions.h"

@interface AppDelegate ()
@property (weak) IBOutlet NSTableView *table;
@property (weak) IBOutlet NSWindow *window;
@property NSMutableArray *vals;
@property (weak) IBOutlet NSArrayController *ctrl;

- (IBAction)copyData:(id)sender;
@end

struct utsname uts;
long darwinver = 0;

static const long max_supported_darwin_version = 19; // 10.15
static const long min_supported_darwin_version = 10; // 10.6

NSString *getOsRel(void) {
	NSMutableString *retval = [NSMutableString stringWithCapacity:64];
	char* tmp;
	char* majrel;

	uname(&uts);
	tmp = strdup(uts.release);
	majrel = strtok(tmp, ".");
	darwinver = strtol(majrel, NULL, 10);
	free(tmp);

	NSString * OSVersionString = [[NSProcessInfo processInfo] operatingSystemVersionString];
	if (OSVersionString == nil) {
		[retval appendString:@"unknown OS X release"];
	} else {
		[retval appendString:OSVersionString];
	}

	if (darwinver > max_supported_darwin_version) {
		[retval appendString:@" (unknown OS X release; please upgrade the eID middleware)"];
	}

	if (darwinver < min_supported_darwin_version) {
		[retval appendString:@" (no longer supported by this version of the middleware)"];
	}

	return retval;
}

NSString *getOsArch(void) {
	return [NSString stringWithCString:uts.machine encoding:NSUTF8StringEncoding];
}

NSString *getPcscdStartStatus(void) {
	struct stat stbuf;

	if (stat("/Library/LaunchDaemons/org.opensc.pcscd.autostart.plist", &stbuf) < 0) {
         switch(errno) {
             case ENOENT:
                 break;

             default:
                 return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
         }
    } else {
        return @"Found";
    }

    if (stat("/System/Library/LaunchDaemons/org.opensc.pcscd.autostart.plist", &stbuf) < 0) {
		switch(errno) {
			case ENOENT:
				if (darwinver >= 14) {
					return @"Not found (OK on OSX 10.10+)";
				}

				return @"Not found";

			default:
				return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
		}
	}

	return @"Found";
}

NSString *getMwVersion(void) {
	return [NSString stringWithCString:BEID_PRODUCT_VERSION encoding:NSUTF8StringEncoding];
}

NSString *getMwRel(void) {
	return [NSString stringWithCString:EIDMW_REVISION_STR encoding:NSUTF8StringEncoding];
}

NSString *getMwBdate(void) {
	return [NSString stringWithCString:BEID_BUILD_DATE encoding:NSUTF8StringEncoding];
}

NSString *getJavaVers(void) {
    char line[LINE_MAX] = { 0 };
    struct stat stbuf;
    int i = 0;
	char *paths[] = {
        "/Library/Java/JavaVirtualMachines/jdk-14.jdk/Contents/Home/bin/java",
        "/Library/Java/JavaVirtualMachines/jdk-13.0.2.jdk/Contents/Home/bin/java",
        "/Library/Java/JavaVirtualMachines/jdk-11.0.6.jdk/Contents/Home/bin/java",
        "/Library/Java/JavaVirtualMachines/jdk1.8.0_241.jdk/Contents/Home/bin/java",
        "/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home/bin/java",
		"/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home/bin/java",
		"/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/bin/java",
		NULL
	};

	for(i=0; paths[i] != NULL; i++) {
		if(stat(paths[i], &stbuf)< 0) {
			switch(errno) {
				case ENOENT:
					// not here, look further
					break;

				default:
					// error
					return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
			}
		} else {
			FILE *f = NULL;
			int rv = 0;
			char *loc = NULL;

			snprintf(line, sizeof(line), "'%s' -version 2>&1", paths[i]);
			f = popen(line, "r");

			if((f == NULL) || (feof(f)) || (fgets(line, LINE_MAX, f) == NULL)) {
                if (f != NULL) {
                    pclose(f);
                }

				return @"(check failed)";
			}

            if (f != NULL)
            {
                rv = pclose(f);
            }

			if((WIFEXITED(rv) == 0) || (WEXITSTATUS(rv) == 127)) {
				return @"(check failed)";
			}

			if((loc = strrchr(line, '"')) != NULL) {
				*loc = '\0';

				return [NSString stringWithCString:strchr(line, '"')+1 encoding:NSUTF8StringEncoding];
			}
		}
	}

	return @"(not found)";
}

NSString *getPcscdStatus(void) {
    FILE *pipe = NULL;
    char line[LINE_MAX] = { 0 };
    char cmd[PATH_MAX] = { 0 };
    char *tmp = NULL;

    snprintf(cmd, PATH_MAX, "ps aux|awk '/pcscd/{print $2}'");
	pipe = popen(cmd, "r");

	if(pipe == NULL) {
		return @"(check failed)";
	}

	if(feof(pipe)) {
		return @"(not running)";
	}

	if(fgets(line, LINE_MAX, pipe) == NULL) {
		return @"(not running)";
	}

    if (pipe != NULL)
    {
        pclose(pipe);
    }

	if((tmp = strchr(line, '\n'))) {
		*tmp = '\0';
	}

	if(strlen(line)==0) {
		return @"(not running)";
	}

	return [NSString stringWithFormat:@"running; pid: %s", line];
}

NSString *getTokendStatus(void) {
    FILE *pipe = NULL;
    char line[LINE_MAX];
    char cmd[PATH_MAX];
    char *tmp = NULL;

    snprintf(cmd, PATH_MAX, "ps aux|awk '/BEID.tokend/{print $2}'");
    pipe = popen(cmd, "r");

    if(pipe == NULL) {
        return @"(check failed)";
    }

    if(feof(pipe)) {
        return @"(not running)";
    }

    if(fgets(line, LINE_MAX, pipe) == NULL) {
        return @"(not running)";
    }

    if (pipe != NULL)
    {
        pclose(pipe);
    }

    if((tmp = strchr(line, '\n'))) {
        *tmp = '\0';
    }

    if(strlen(line) == 0) {
        return @"(not running)";
    }

    return [NSString stringWithFormat:@"running; pid: %s", line];
}

NSString *getBEIDTokenStatus(void) {
    FILE *pipe = NULL;
    char line[LINE_MAX];
    char cmd[PATH_MAX];
    char *tmp = NULL;

    snprintf(cmd, PATH_MAX, "ps aux|awk '/BEIDToken/{print $2}'");
    pipe = popen(cmd, "r");

    if(pipe == NULL) {
        return getTokendStatus();
    }

    if(feof(pipe)) {
        return getTokendStatus();
    }

    if(fgets(line, PATH_MAX, pipe) == NULL) {
        return getTokendStatus();
    }

    if((tmp = strchr(line, '\n'))) {
        *tmp = '\0';
    }

    if(strlen(line) == 0) {
        return getTokendStatus();
    }

    return [NSString stringWithFormat:@"running; pid: %s", line];
}

NSString *getTokendLoc(void) {
	struct stat stbuf;

    if(stat("/Applications/BEIDToken.app", &stbuf) < 0) {
        switch(errno) {
            case ENOENT:
                break;

            default:
                return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
        }
    } else {
        return @"/Applications/BEIDToken.app";
    }

	if(stat("/System/Library/Security/tokend/BEID.tokend", &stbuf) < 0) {
		switch(errno) {
			case ENOENT:
				break;

			default:
				return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
		}
	} else {
		return @"/System/Library/Security/tokend/BEID.tokend";
	}

	if(stat("/Library/Security/tokend/BEID.tokend", &stbuf) < 0) {
		switch(errno) {
			case ENOENT:
				return @"Not found";

			default:
				return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
		}
	}
    
    return @"/Library/Security/tokend/BEID.tokend";
}

@implementation AppDelegate

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView {
	return [self.vals count];
}

- (NSView *)tableView:(NSTableView*)tableView viewForTableColumn:(NSTableColumn*)tableColumn
		  row:(NSInteger) row {
	DataItem *item = [self.vals objectAtIndex:row];
	NSTableCellView *cell = [tableView makeViewWithIdentifier:[tableColumn identifier] owner:self];
	NSTextField *field = [cell textField];
	if ([[tableColumn identifier] isEqualToString:@"Item"]) {
		[field setStringValue:[item title]];
	} else if ([[tableColumn identifier] isEqualToString:@"Value"]) {
		[field setStringValue:[item value]];
	}

	return cell;
}

- (void)awakeFromNib {
	if ([self.vals count] > 0) {
		return;
	}

	self.vals = [NSMutableArray arrayWithCapacity:1];
    DataItem *item = [DataItem alloc];

	[item setTitle: @"Middleware version"];
	[item setValue: getMwVersion()];
	[self.ctrl addObject:item];

    item = [DataItem alloc];
	[item setTitle: @"Middleware build ID"];
	[item setValue: getMwRel()];
	[self.ctrl addObject:item];

    item = [DataItem alloc];
	[item setTitle: @"Middleware build date"];
	[item setValue: getMwBdate()];
	[self.ctrl addObject:item];

	item = [DataItem alloc];
	[item setTitle: @"Java version"];
	[item setValue: getJavaVers()];
	[self.ctrl addObject:item];
	item = [DataItem alloc];
	[item setTitle: @"OS release"];
	[item setValue: getOsRel()];

	[self.ctrl addObject:item];	item = [DataItem alloc];
	[item setTitle: @"System architecture"];
	[item setValue: getOsArch()];
	[self.ctrl addObject:item];

	item = [DataItem alloc];
	[item setTitle: @"opensc pcscd autostart"];
	[item setValue: getPcscdStartStatus()];
	[self.ctrl addObject:item];

	item = [DataItem alloc];
	[item setTitle: @"pcscd status"];
	[item setValue: getPcscdStatus()];
	[self.ctrl addObject:item];

	item = [DataItem alloc];
	[item setTitle: @"BEID tokend location"];
	[item setValue: getTokendLoc()];
	[self.ctrl addObject:item];
    
	item = [DataItem alloc];
	[item setTitle: @"BEID tokend process"];
	[item setValue: getBEIDTokenStatus()];
	[self.ctrl addObject:item];
}

- (IBAction)copyData:(id)sender {
	int i = 0;
	NSMutableArray *arr = [NSMutableArray arrayWithCapacity:1];
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];

	for(i=0; i<[self.vals count]; i++) {
		if ([self.table isRowSelected:i]) {
			[arr addObject:[self.vals objectAtIndex:i]];
		}
	}

	if ([arr count] == 0) {
		return;
	}

	[pasteboard clearContents];
	[pasteboard writeObjects:arr];
}

@end
