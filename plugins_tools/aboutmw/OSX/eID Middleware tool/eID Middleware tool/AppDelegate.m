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

@interface AppDelegate ()
@property (weak) IBOutlet NSTableView *table;
@property (weak) IBOutlet NSWindow *window;
@property NSMutableArray *vals;
@property (weak) IBOutlet NSArrayController *ctrl;

@end

char* getOsRel() {
    struct utsname uts;
    uname(&uts);
    return strdup(uts.release);
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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (void)awakeFromNib {
    if ([self.vals count] > 0) {
        return;
    }
    self.vals = [NSMutableArray arrayWithCapacity:1];
    DataItem *item = [DataItem alloc];
    [item setTitle: @"OS release"];
    [item setValue: [NSString stringWithCString:getOsRel() encoding:NSUTF8StringEncoding]];
    [self.ctrl addObject:item];
}

@end
