/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
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
