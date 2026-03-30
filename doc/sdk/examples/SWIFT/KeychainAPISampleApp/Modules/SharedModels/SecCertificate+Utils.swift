//

import Foundation
import CryptoKit
import Security

public extension SecCertificate {
    var summary: String {
        return (SecCertificateCopySubjectSummary(self) as String?) ?? "Certificate"
    }
    
    var notValidBefore: Date? {
        guard
            let values = SecCertificateCopyValues(
                self,
                [kSecOIDX509V1ValidityNotBefore] as CFArray,
                nil
            ) as? [CFString: Any],
            let validityDict = values[kSecOIDX509V1ValidityNotBefore] as? [CFString: Any],
            let dateValue = validityDict[kSecPropertyKeyValue] as? Double
        else {
            return nil
        }
        
        return Date(timeIntervalSinceReferenceDate: dateValue)
    }
    
    var notValidAfter: Date? {
        guard
            let values = SecCertificateCopyValues(
                self,
                [kSecOIDX509V1ValidityNotAfter] as CFArray,
                nil
            ) as? [CFString: Any],
            let validityDict = values[kSecOIDX509V1ValidityNotAfter] as? [CFString: Any],
            let dateValue = validityDict[kSecPropertyKeyValue] as? Double
        else {
            return nil
        }
        
        return Date(timeIntervalSinceReferenceDate: dateValue)
    }
    
    var isValid: Bool {
        guard let notValidBefore = self.notValidBefore,
              let notValidAfter = self.notValidAfter
        else { return false }
        
        return notValidBefore < Date() && notValidAfter > Date()
    }
}
