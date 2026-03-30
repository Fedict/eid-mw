//

import Foundation

@Observable
class AppNavigationViewModel {
    // MARK: - Observable properties
    private(set) var viewTarget: AppNavigationViewTarget = .certificateRetrieval
}

// MARK: - Derived models
extension AppNavigationViewModel {
    var certificateRetrievalViewModel: CertificateRetrievalViewModel {
        .init(retrievalCompleted: {
            self.viewTarget = .certificateSelection
        })
    }
    var certificateSelectionViewModel: CertificateSelectionViewModel {
        .init(
            backRequested: {
                self.viewTarget = .certificateRetrieval
            },
            identitySelected: { identity in
                self.viewTarget = .signing(identity)
            }
        )
    }
    
    func signingViewModel(identity: KeychainIdentity) -> SigningViewModel {
        .init(
            identity: identity,
            completed: {
                self.viewTarget = .certificateSelection
            }
        )
    }
}
