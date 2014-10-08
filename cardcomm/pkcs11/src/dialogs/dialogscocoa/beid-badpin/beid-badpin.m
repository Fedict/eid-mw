//
//  beid-badpin.m
//  beid-badpin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "beid-badpin.h"


@implementation beid_badpin

- (void)awakeFromNib
{
	[NSApp setDelegate:self];
	
	NSRunningApplication* me=[NSRunningApplication currentApplication];
	NSArray* peers=[NSRunningApplication runningApplicationsWithBundleIdentifier:@"be.fedict.eid.cryptomodules.pkcs#11.beid-badpin"];
	for(id peer in peers)
	{
		if(![peer isEqual:me])
			[peer terminate:self];
	}
	
	NSArray* args=[[NSProcessInfo processInfo] arguments]; 
	if([args count]==2)
	{
		int attemptsLeft=[[args objectAtIndex:1] intValue];
		NSString* templateLabel=(attemptsLeft<=1?[lastAttemptTextField stringValue]:[textField stringValue]);
		NSString* labelText=[NSString stringWithFormat:templateLabel,attemptsLeft];
		[textField setObjectValue:labelText];
		[window makeKeyAndOrderFront:nil];
		[me activateWithOptions:NSApplicationActivateIgnoringOtherApps];
	}
	else
	{
		[NSApp terminate:self];
	}
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	printf("OK\n");
	return NSTerminateNow;
}

// the ok button was clicked
////////////////////////////////////////////////////////////////////////////////////////
-(IBAction)button_ok:(id)sender
{
	[NSApp terminate:self];
}

@end
