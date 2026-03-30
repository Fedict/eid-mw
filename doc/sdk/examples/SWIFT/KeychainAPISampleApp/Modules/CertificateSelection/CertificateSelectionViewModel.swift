//

import Foundation
import Security

@Observable
class CertificateSelectionViewModel {
    // MARK: - Observable properties
    private(set) var identities: [KeychainIdentity]
    
    // MARK: - Callbacks
    private let backRequested: () -> Void
    private let identitySelected: (KeychainIdentity) -> Void
    
    // MARK: - Lifecycle
    init(
        backRequested: @escaping () -> Void,
        identitySelected: @escaping (KeychainIdentity) -> Void
    ) {
        self.backRequested = backRequested
        self.identitySelected = identitySelected
        self.identities = (try? Self.fetchSigningIdentities()) ?? []
    }
    
    // MARK: - Actions
    func backButtonUsed() {
        self.backRequested()
    }
    
    func identityRowClicked(_ identity: KeychainIdentity) {
        self.identitySelected(identity)
    }
    
    // MARK: - Internals
    private static func fetchSigningIdentities() throws -> [KeychainIdentity] {
        let query: [CFString: Any] = [
            kSecClass: kSecClassIdentity,
            kSecReturnRef: true,
            kSecMatchLimit: kSecMatchLimitAll,
            // Optional: restrict to a keychain, access group, etc.
        ]

        var result: CFTypeRef?
        let status = SecItemCopyMatching(query as CFDictionary, &result)

        guard status == errSecSuccess else {
            throw NSError(domain: NSOSStatusErrorDomain, code: Int(status))
        }

        let identities = (result as? [SecIdentity]) ?? []
        return identities.compactMap { identity in
            // Make sure the identity's certificate is valid
            guard identity.certificate?.isValid == true
            else { return nil }
            
            // Get private key from the certificate, if possible
            guard let privateKey = identity.privateKey
            else { return nil }

            return KeychainIdentity(
                identity: identity,
                label: identity.certificate?.summary ?? "Certificate",
                privateKey: privateKey
            )
        }
    }
}
