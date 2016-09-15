/*
 Copyright (C) 2016 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 Implements PIV token session
 */

NS_ASSUME_NONNULL_BEGIN

@interface PIVTokenSession()

typedef NS_ENUM(NSInteger, PIVAuthState) {
    PIVAuthStateUnauthorized = 0,
    PIVAuthStateFreshlyAuthorized = 1,
    PIVAuthStateAuthorizedButAlreadyUsed = 2,
};

@property PIVAuthState authState;

@end

@interface PIVAuthOperation : TKTokenSmartCardPINAuthOperation

- (instancetype)initWithSession:(PIVTokenSession *)session;
@property (readonly) PIVTokenSession *session;

@end


NS_ASSUME_NONNULL_END
