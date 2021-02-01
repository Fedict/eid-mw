//
//  binhandler.h
//  eID Viewer
//
//  Created by buildslave on 29/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AppDelegate.h"

@ protocol binhandler<NSObject>
-(void) handle_bin_data:(NSData *)data forLabel:(NSString *)label withUi:(AppDelegate *)ui;
@end
