//
//  beid-changepass.h
//  beid-changepin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define MIN_PIN_LENGTH 4
#define MAX_PIN_LENGTH 12


@interface beid_changepin : NSObject <NSTextFieldDelegate>
{
	IBOutlet NSSecureTextField*		originalPinEntry;
	IBOutlet NSSecureTextField*		newPin0Entry;
	IBOutlet NSSecureTextField*		newPin1Entry;
	IBOutlet NSButton*				okButton;
	IBOutlet NSButton*				cancelButton;
			 NSCharacterSet*		decimalDigits;
			 Boolean				ok;
}

-(void)controlTextDidChange:(NSNotification*)notification;
-(void)updateOKButton;
-(IBAction)button_cancel:(id)sender;
-(IBAction)button_ok:(id)sender;


@end
