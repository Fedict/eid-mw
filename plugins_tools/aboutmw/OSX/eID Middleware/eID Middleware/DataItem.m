//
//  DataItem.m
//  eID Middleware tool
//
//  Created by buildslave on 14/01/15.
//  Copyright (c) 2015 FedICT. All rights reserved.
//

#import "DataItem.h"

@implementation DataItem
-(NSString*) description {
    return [NSString stringWithFormat:@"%@: %@", _title, _value];
}
-(id)pasteboardPropertyListForType:(NSString *)type {
    return [self description];
}
-(NSArray*) writableTypesForPasteboard:(NSPasteboard *)pasteboard {
    return [[NSArray alloc] initWithObjects:@"public.utf8-plain-text", nil];
}
@end
