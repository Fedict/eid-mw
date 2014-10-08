//
//  beid-askpin.m
//  beid-askpin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import "beid-askpin.h"


@implementation beid_askpin

// update visual PIN field, status of ok, backspace and clear buttons
////////////////////////////////////////////////////////////////////////////////////////
- (void)pin_changed
{
	[pinField			setIntegerValue:	 [pinCode length]];
	[okButton			setEnabled:			([pinCode length]>=MIN_PIN_LENGTH?YES:NO)];
	[backSpaceButton	setEnabled:			([pinCode length]>0?YES:NO)];
	[clearButton		setEnabled:			([pinCode length]>0?YES:NO)];
}

// add one digit to the current PIN code
////////////////////////////////////////////////////////////////////////////////////////
- (void)add_digit:(NSString*)digit
{
	if([pinCode length]<MAX_PIN_LENGTH)
    {
		[pinCode appendString:digit];
		[self pin_changed];
	}
}

// remove one digit from the current PIN code
////////////////////////////////////////////////////////////////////////////////////////
- (void)backspace
{
	if([pinCode length]>0)
    {
		NSRange lastDigit={[pinCode length]-1,1};
		[pinCode deleteCharactersInRange:lastDigit];
		[self pin_changed];
	}
}

// clear the current PIN code
////////////////////////////////////////////////////////////////////////////////////////
- (void)clear
{
	NSRange allDigits={0,[pinCode length]};
	[pinCode deleteCharactersInRange:allDigits];
	[self pin_changed];
}

//-------- CONSTRUCTION/DESTRUCTION ----------//


// after the GUI is constructed, initialize some working objects
////////////////////////////////////////////////////////////////////////////////////////
- (void)awakeFromNib
{
	ok=false;
	[NSApp setDelegate:self];
	numericSet=[NSCharacterSet decimalDigitCharacterSet];
	pinCode=[[NSMutableString alloc] init];
	[pinWindow setNextResponder:self];
	[self pin_changed];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	if(!ok)
		printf("CANCEL\n");
	return NSTerminateNow;
}


//-------- EVENT ACTIONS ----------//


// one of the numeric buttons was clicked
////////////////////////////////////////////////////////////////////////////////////////
- (IBAction)button_digit:(id)sender
{
	NSString* digit = [sender title];	
	[self add_digit:digit];
}

// the ← button was clicked
////////////////////////////////////////////////////////////////////////////////////////
- (IBAction)button_backspace:(id)sender
{
	[self backspace];
}

// the ✕ button was clicked
////////////////////////////////////////////////////////////////////////////////////////
- (IBAction)button_clear:(id)sender
{
	[self clear];
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
	printf("%s\n",[pinCode UTF8String]);
	[NSApp terminate:self];
}

// keys were hit
////////////////////////////////////////////////////////////////////////////////////////
- (void)keyDown:(NSEvent *)theEvent
{
	NSString* typedString=[theEvent characters];
	UniChar typedKey=[typedString characterAtIndex:0];
	
	if(typedKey == NSDeleteCharacter)
	{
		[self backspace];
	}
	else
	{
		if([numericSet characterIsMember:typedKey])
		{
			[self add_digit:typedString];
		}
	}
}

@end
