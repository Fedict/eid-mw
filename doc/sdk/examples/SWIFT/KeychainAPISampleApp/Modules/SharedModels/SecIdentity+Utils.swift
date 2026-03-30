//

import Foundation
import Security

public extension SecIdentity {    
    var certificate: SecCertificate? {
        var cert: SecCertificate?
        let status = SecIdentityCopyCertificate(self, &cert)
        if status == errSecSuccess {
            return cert
        } else {
            return nil
        }
    }
    
    var privateKey: SecKey? {
        var key: SecKey?
        let status = SecIdentityCopyPrivateKey(self, &key)
        if status == errSecSuccess {
            return key
        } else {
            print("Failed to get private key: \(status)")
            return nil
        }
    }
}
