//
//  ValidityTree.h
//  eID Viewer
//
//  Created by Wouter Verhelst on 20/11/2020.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface ValidityTree : NSObject<NSOutlineViewDataSource>
-(instancetype)initWithOutlineView:(NSOutlineView *)view;

@property NSMutableDictionary *tree;
@property NSDictionary *index;
@property NSArray *images;
@property NSOutlineView *ov;
@end

NS_ASSUME_NONNULL_END
