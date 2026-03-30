//

import Foundation

enum SigningViewModelState {
    case waitingForInput
    case waitingForSignature
    case signatureCompleted(String)
    case error(String)
}
