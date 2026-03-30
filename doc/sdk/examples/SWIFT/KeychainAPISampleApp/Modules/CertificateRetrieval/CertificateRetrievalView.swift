//

import SwiftUI

struct CertificateRetrievalView: View {
    private var model: CertificateRetrievalViewModel
    
    init(model: CertificateRetrievalViewModel) {
        self.model = model
    }
    
    var body: some View {
        if self.model.isRetrievingCertificates {
            VStack(spacing: 20) {
                Text("Waiting for certificate retrieval...")
                ProgressView()
            }
        } else {
            Button("Retrieve certificates") {
                self.model.retrieveCertificatesButtonUsed()
            }
            .buttonStyle(.borderedProminent)
        }
    }
}

#Preview {
    CertificateRetrievalView(model: .init(retrievalCompleted: {}))
        .frame(width: 600, height: 600)
}
