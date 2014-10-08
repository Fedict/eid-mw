//
//  beid-askaccess.m
//  beid-askaccess
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import "beid-askaccess.h"


@implementation beid_askaccess

- (void)awakeFromNib
{
	allow=false;
	[NSApp setDelegate:self];
	
	NSRunningApplication* me=[NSRunningApplication currentApplication];
	NSArray* peers=[NSRunningApplication runningApplicationsWithBundleIdentifier:@"be.fedict.eid.cryptomodules.pkcs#11.beid-askaccess"];
	for(id peer in peers)
	{
		if(![peer isEqual:me])
			[peer terminate:self];
	}
		   
	NSArray* args=[[NSProcessInfo processInfo] arguments]; 
	if([args count]==2)
	{
		NSString* basicLabel=[textField stringValue];
		NSString* appName=[args objectAtIndex:1];
		NSString* labelText=[NSString stringWithFormat:basicLabel,appName];
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
	printf("%s\n",(allow?"ALLOW":"DENY"));
	return NSTerminateNow;
}

// the deny button was clicked
////////////////////////////////////////////////////////////////////////////////////////
-(IBAction)button_deny:(id)sender
{
	allow=false;
	[NSApp terminate:self];
}

// the allow button was clicked
////////////////////////////////////////////////////////////////////////////////////////
-(IBAction)button_allow:(id)sender
{
	allow=true;
	[NSApp terminate:self];
}
		   

@end
