//
//  AppDelegate.m
//  eID Middleware tool
//
//  Created by buildslave on 14/01/15.
//  Copyright (c) 2015 FedICT. All rights reserved.
//

#import "AppDelegate.h"
#import "DataItem.h"
#include <sys/utsname.h>
#include <sys/stat.h>

@interface AppDelegate ()
@property (weak) IBOutlet NSTableView *table;
@property (weak) IBOutlet NSWindow *window;
@property NSMutableArray *vals;
@property (weak) IBOutlet NSArrayController *ctrl;

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

NSString* getPcscdStatus() {
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
    [item setValue: getPcscdStatus()];
    [self.ctrl addObject:item];
}

@end
