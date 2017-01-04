
NS_ASSUME_NONNULL_BEGIN

@interface BEIDTokenSession()

typedef NS_ENUM(NSInteger, BEIDAuthState) {
    BEIDAuthStateUnauthorized = 0,
    BEIDAuthStateFreshlyAuthorized = 1,
    BEIDAuthStateAuthorizedButAlreadyUsed = 2,
};

@property BEIDAuthState authState;

@end

@interface BEIDAuthOperation : TKTokenSmartCardPINAuthOperation

- (instancetype)initWithSession:(BEIDTokenSession *)session;
@property (readonly) BEIDTokenSession *session;

@end


NS_ASSUME_NONNULL_END
