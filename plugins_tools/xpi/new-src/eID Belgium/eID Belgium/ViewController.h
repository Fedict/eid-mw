//
//  ViewController.h
//  eID Belgium
//
//  Created by Vampire Cat on 28/02/2021.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController

@property (weak, nonatomic) IBOutlet NSTextField *appNameLabel;

- (IBAction)openSafariExtensionPreferences:(id)sender;

@end

