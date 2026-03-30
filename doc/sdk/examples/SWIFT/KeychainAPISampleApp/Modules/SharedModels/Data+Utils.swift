//

import Foundation

extension Data {
    var hexEncodedString: String {
        return map { String(format: "%02x", $0) }.joined()
    }
}
