//

import Foundation

enum AppNavigationViewTarget {
    case certificateRetrieval
    case certificateSelection
    case signing(KeychainIdentity)
}
