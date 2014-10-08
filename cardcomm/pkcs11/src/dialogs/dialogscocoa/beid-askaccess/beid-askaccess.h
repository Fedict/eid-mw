//
//  beid-askaccess.h
//  beid-askaccess
//
//  Created by Frank Mariën on 22/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface beid_askaccess : NSObject
{
	IBOutlet NSTextField* textField;
	IBOutlet NSWindow*	  window;
			 Boolean	  allow;
}

-(IBAction)button_allow:(id)sender;
-(IBAction)button_deny:(id)sender;

@end