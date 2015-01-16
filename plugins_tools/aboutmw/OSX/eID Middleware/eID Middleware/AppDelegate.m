/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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

@interface AppDelegate ()
@property (weak) IBOutlet NSTableView *table;
@property (weak) IBOutlet NSWindow *window;
@property NSMutableArray *vals;
@property (weak) IBOutlet NSArrayController *ctrl;
- (IBAction)copyData:(id)sender;

@end

struct utsname uts;
long osver;

NSString* getOsRel() {
    NSDictionary *RelMap = [NSDictionary dictionaryWithObjectsAndKeys:
                            @" (OSX 10.10, Yosemite)", @"14",
                            @" (OSX 10.9, Mavericks)", @"13",
                            @" (OSX 10.8, Mountain Lion)", @"12",
                            @" (OSX 10.7, Lion)", @"11",
                            nil];
    NSMutableString *retval = [NSMutableString stringWithCapacity:30];
    char* tmp;
    char* majrel;

    [retval appendString:@"Darwin "];

    uname(&uts);
    tmp = strdup(uts.release);
    majrel = strtok(tmp, ".");
    osver = strtol(majrel, NULL, 10);
    NSString *reldesc = [RelMap valueForKey:[NSString stringWithCString:majrel encoding:NSUTF8StringEncoding]];
    [retval appendString:[NSString stringWithCString:uts.release encoding:NSUTF8StringEncoding]];
    if (reldesc == nil) {
        [retval appendString:@" (unknown OSX release; please upgrade the eID middleware)"];
    } else {
        [retval appendString:reldesc];
    }
    free(tmp);
    return retval;
}

NSString* getOsArch() {
    return [NSString stringWithCString:uts.machine encoding:NSUTF8StringEncoding];
}

NSString* getPcscdStartStatus() {
    struct stat stbuf;
    if(stat("/System/Library/LaunchDaemons/org.opensc.pcscd.autostart.plist", &stbuf)<0) {
        switch(errno) {
            case ENOENT:
                if (osver >= 14) {
                    return @"Not found (OK on OSX 10.10)";
                }
                return @"Not OK";
            default:
                return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
        }
    }
    if (osver >= 14) {
        return @"Found, yet OS is 10.10+?";
    }
    return @"OK";
}

NSString* getPcscdStatus() {
    FILE* pipe = popen("ps aux|awk '/[p]cscd/{print $2}'", "r");
    char line[80];
    char *tmp;
    
    if(!pipe) {
        return @"(check failed)";
    }
    if(feof(pipe)) {
        return @"(not running)";
    }
    if(fgets(line, 80, pipe) == NULL) {
        return @"(not running)";
    }
    if((tmp = strchr(line, '\n'))) {
        *tmp = '\0';
    }
    if(strlen(line)==0) {
        return @"(not running)";
    }
    return [NSString stringWithFormat:@"running; pid: %s", line];
}

NSString* getTokendStatus() {
    struct stat stbuf;
    if(stat("/System/Library/Security/tokend/BEID.tokend", &stbuf) < 0) {
        switch(errno) {
            case ENOENT:
                return @"Not found";
            default:
                return [NSString stringWithFormat:@"Could not check: %s", strerror(errno)];
        }
    }
    return @"/System/Library/Security/tokend/BEID.tokend";
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
    [item setTitle: @"OS release"];
    [item setValue: getOsRel()];
    [self.ctrl addObject:item];
    item = [DataItem alloc];
    [item setTitle: @"System architecture"];
    [item setValue: getOsArch()];
    [self.ctrl addObject:item];
    item = [DataItem alloc];
    [item setTitle: @"pcscd autostart"];
    [item setValue: getPcscdStartStatus()];
    [self.ctrl addObject:item];
    item = [DataItem alloc];
    [item setTitle: @"pcscd status"];
    [item setValue: getPcscdStatus()];
    [self.ctrl addObject:item];
    item = [DataItem alloc];
    [item setTitle: @"BEID tokend"];
    [item setValue: getTokendStatus()];
    [self.ctrl addObject:item];
}

- (IBAction)copyData:(id)sender {
    int i;
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
