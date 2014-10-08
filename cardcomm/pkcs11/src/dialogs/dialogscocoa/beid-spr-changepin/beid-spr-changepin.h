//
//  beid-spr-changepin.h
//  beid-spr-changepin
//
//  Created by Frank Mariën on 08/08/11.
//  Copyright 2011 FedICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface beid_spr_changepin : NSObject
{
	IBOutlet NSProgressIndicator* progress;
	IBOutlet NSTextField*		  textField;
	IBOutlet NSWindow*			  window;
}

@end
