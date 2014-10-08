//
//  beid-askpin.h
//  beid-askpin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define MIN_PIN_LENGTH 4
#define MAX_PIN_LENGTH 12

@interface beid_askpin : NSResponder
{
	IBOutlet NSLevelIndicator*   pinField;
	IBOutlet NSWindow*			 pinWindow;
	IBOutlet NSButton*			 okButton;
	IBOutlet NSButton*			 backSpaceButton;
	IBOutlet NSButton*			 clearButton;
			 NSMutableString*    pinCode;
			 NSCharacterSet*	 numericSet;
			 Boolean			 ok;
}

-(void)keyDown:(NSEvent *)theEvent;
-(void)update_pin_label;
-(void)pin_changed;
-(void)backspace;
-(void)add_digit:(NSString*)digit;
-(void)clear;

-(IBAction)button_digit:(id)sender;
-(IBAction)button_backspace:(id)sender;
-(IBAction)button_clear:(id)sender;
-(IBAction)button_cancel:(id)sender;
-(IBAction)button_ok:(id)sender;


@end
