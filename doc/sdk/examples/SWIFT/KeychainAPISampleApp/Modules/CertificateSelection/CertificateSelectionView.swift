//

import SwiftUI

struct CertificateSelectionView: View {
    private var model: CertificateSelectionViewModel
    
    init(model: CertificateSelectionViewModel) {
        self.model = model
    }
    
    var body: some View {
        VStack {
            ScrollView {
                VStack(alignment: .leading, spacing: 0) {
                    ForEach(Array(self.model.identities.enumerated()), id: \.offset) { _, identity in
                        VStack(alignment: .leading, spacing: 0) {
                            HStack(alignment: .center, spacing: 12) {
                                VStack(alignment: .leading, spacing: 4) {
                                    Text(identity.label)
                                    if let notValidAfter = identity.identity.certificate?.notValidAfter {
                                        Text("Expires after \(DateFormatter.localizedString(from: notValidAfter, dateStyle: .short, timeStyle: .short))")
                                            .foregroundStyle(.secondary)
                                            .font(.footnote)
                                    }
                                }
                                .frame(maxWidth: .infinity, alignment: .leading)
                                
                                Button("Select") {
                                    self.model.identityRowClicked(identity)
                                }
                                .buttonStyle(.borderedProminent)
                            }
                            .padding(.vertical, 8)
                            
                            Rectangle()
                                .frame(height: 1)
                                .opacity(0.3)
                        }
                    }
                }
            }
            Button("Back") {
                self.model.backButtonUsed()
            }
            .buttonStyle(.borderedProminent)
            .padding(.top, 20)
        }
    }
}

#Preview {
    CertificateSelectionView(model: .init(
        backRequested: {},
        identitySelected: { _ in }
    ))
        .frame(width: 600, height: 400)
}
