//
//  beid-changepass.m
//  beid-changepin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import "beid-changepin.h"


@implementation beid_changepin

// after the GUI is constructed, initialize some working objects
////////////////////////////////////////////////////////////////////////////////////////
- (void)awakeFromNib
{
	ok=false;
	[NSApp setDelegate:self];
	
	NSRunningApplication* me=[NSRunningApplication currentApplication];
	NSArray* peers=[NSRunningApplication runningApplicationsWithBundleIdentifier:@"be.fedict.eid.cryptomodules.pkcs#11.beid-changepin"];
	for(id peer in peers)
	{
		if(![peer isEqual:me])
			[peer terminate:self];
	}
	
	[originalPinEntry setDelegate:self];
	[newPin0Entry setDelegate:self];
	[newPin1Entry setDelegate:self];
	decimalDigits=[NSCharacterSet decimalDigitCharacterSet];
	[self updateOKButton];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	if(!ok)
		printf("CANCEL\n");
	return NSTerminateNow;
}

-(Boolean)entriesAreValid
{
	NSString* originalPIN	=[originalPinEntry stringValue];
	NSString* newPIN0		=[newPin0Entry stringValue];
	NSString* newPIN1		=[newPin1Entry stringValue];
	
	if([originalPIN length]<MIN_PIN_LENGTH || [newPIN0 length]<MIN_PIN_LENGTH || [newPIN1 length]<MIN_PIN_LENGTH)
		return false;
	if([originalPIN length]>MAX_PIN_LENGTH || [newPIN0 length]>MAX_PIN_LENGTH || [newPIN1 length]>MAX_PIN_LENGTH)
		return false;
	if(![newPIN0 isEqualToString:newPIN1])
		return false;
	if([newPIN0 isEqualToString:originalPIN])
		return false;
	return true;
}

-(void)updateOKButton
{
	[okButton setEnabled: [self entriesAreValid]];
}


-(void)controlTextDidChange:(NSNotification*)notification
{
	NSSecureTextField*	target=[notification object];
	NSString*			typed=[target stringValue];
	NSString*			numericOnly=[[NSString alloc] init];
	for(int i=0;i<[typed length] && i<MAX_PIN_LENGTH;i++)
		if([decimalDigits characterIsMember:[typed characterAtIndex:i]])
			numericOnly=[numericOnly stringByAppendingFormat:@"%c",[typed characterAtIndex:i]];
	[target setStringValue:numericOnly];
	[self updateOKButton];
}

// the Cancel button was clicked
////////////////////////////////////////////////////////////////////////////////////////
-(IBAction)button_cancel:(id)sender
{
	ok=false;
	[NSApp terminate:self];
}

// the OK button was clicked
////////////////////////////////////////////////////////////////////////////////////////
-(IBAction)button_ok:(id)sender
{
	ok=true;
	printf("%s:%s\n",[[originalPinEntry stringValue] UTF8String],[[newPin0Entry stringValue] UTF8String]);
	[NSApp terminate:self];
}


@end
