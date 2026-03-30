//

import SwiftUI

struct AppNavigationView: View {
    private let model = AppNavigationViewModel()
    
    var body: some View {
        switch self.model.viewTarget {
        case .certificateRetrieval:
            CertificateRetrievalView(model: self.model.certificateRetrievalViewModel)
        case .certificateSelection:
            CertificateSelectionView(model: self.model.certificateSelectionViewModel)
        case .signing(let identity):
            SigningView(model: self.model.signingViewModel(identity: identity))
        }
    }
}

#Preview {
    AppNavigationView()
}
