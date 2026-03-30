//

import Foundation
import AppKit

@Observable
class CertificateRetrievalViewModel {
    // MARK: - Observable properties
    private(set) var isRetrievingCertificates = false
    
    // MARK: - Callbacks
    private let retrievalCompleted: () -> Void
    
    // MARK: - Lifecycle
    init(retrievalCompleted: @escaping () -> Void) {
        self.retrievalCompleted = retrievalCompleted
    }
    
    // MARK: - Buttons
    func retrieveCertificatesButtonUsed() {
        self.isRetrievingCertificates = true
        self.startCertificateRetrieval()
    }
    
    // MARK: - Internals
    private func startCertificateRetrieval() {
        DarwinListener.shared.certificateRetrieved = { [weak self] in
            self?.retrievalCompleted()
            self?.isRetrievingCertificates = false
        }
        NSWorkspace.shared.open(URL(string: "beidsign://certificate")!)
    }
}
