//
//  dataroot-osx.m
//  eID Viewer
//
//  Created by buildslave on 24/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <xsdloc.h>

const char* get_xsdloc(void) {
    static char path[PATH_MAX] = "";
    if(path[0] == 0) {
        NSBundle* bundle = [NSBundle mainBundle];
        NSURL* url = [bundle URLForResource:@"eidv4.xsd" withExtension:nil];
        [url getFileSystemRepresentation:path maxLength:PATH_MAX];
    }
    return path;
}
