//

import Foundation
import CryptoKit

@Observable
class SigningViewModel {
    // MARK: - Observable properties
    private(set) var viewState = SigningViewModelState.waitingForInput
    var inputText = "Hello! Hit continue to sign the content of this text field."
    
    // MARK: - Internal properties
    private let identity: KeychainIdentity
    private let completed: () -> Void
    
    // MARK: - Lifecycle
    init(
        identity: KeychainIdentity,
        completed: @escaping () -> Void
    ) {
        self.identity = identity
        self.completed = completed
    }
    
    // MARK: - Actions
    func continueButtonUsed() {
        self.viewState = .waitingForSignature
        Task {
            self.doSign()
        }
    }
    
    func backButtonUsed() {
        self.completed()
    }
    
    // MARK: - Internals
    private func doSign() {
        // Make hash to sign
        let hashToSign = Data(SHA256.hash(data: self.inputText.data(using: .utf8)!))
        
        // Do sign
        if let sig = SecKeyCreateSignature(self.identity.privateKey, .ecdsaSignatureDigestX962SHA256, hashToSign as CFData, nil) as Data? {
            self.viewState = .signatureCompleted(sig.hexEncodedString)
        } else {
            self.viewState = .error("Could not sign")
        }
    }
}
