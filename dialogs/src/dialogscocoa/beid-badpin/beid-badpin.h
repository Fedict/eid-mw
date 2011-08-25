//
//  beid-badpin.h
//  beid-badpin
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface beid_badpin : NSObject
{
	IBOutlet NSWindow*    window;
	IBOutlet NSTextField* textField;
	IBOutlet NSTextField* lastAttemptTextField;
}

-(IBAction)button_ok:(id)sender;

@end