//

import SwiftUI

struct SigningView: View {
    @Bindable private var model: SigningViewModel
    
    init(model: SigningViewModel) {
        self.model = model
    }
    
    var body: some View {
        VStack(spacing: 20) {
            switch self.model.viewState {
            case .waitingForInput:
                TextField("", text: self.$model.inputText)
                Button("Continue") {
                    self.model.continueButtonUsed()
                }
                .buttonStyle(.borderedProminent)
            case .waitingForSignature:
                Text("Waiting for signature...")
                ProgressView()
            case .signatureCompleted(let signatureHex):
                Text("Signature completed! Hex value below.")
                Text(signatureHex)
                Button("Back") {
                    self.model.backButtonUsed()
                }
                .buttonStyle(.borderedProminent)
            case .error(let errorText):
                Text("Signature failed: \(errorText)")
                Button("Back") {
                    self.model.backButtonUsed()
                }
                .buttonStyle(.borderedProminent)
            }
        }
    }
}
