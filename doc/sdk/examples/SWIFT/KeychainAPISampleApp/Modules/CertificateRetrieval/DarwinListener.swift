//

import Foundation

class DarwinListener {
    // MARK: - Constants
    private static let certificateRetrievalStartedNotificationName = "com.zetes.pc-signing.certificate-requested" as CFString
    private static let certificateRetrievalCompletedNotificationName = "com.zetes.pc-signing.certificate-retrieved" as CFString
    
    // MARK: - Singleton
    static let shared = DarwinListener()
    
    // MARK: - Internal properties
    private lazy var observerPointer: UnsafeMutableRawPointer = {
        UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
    }()
    
    // MARK: - Exposed properties
    var certificateRetrieved: (() -> Void)?
    
    // MARK: - Lifecycle
    private init() {
        self.listenToDarwinNotifications()
    }
    
    // MARK: - Internals
    private func listenToDarwinNotifications() {
        let center = CFNotificationCenterGetDarwinNotifyCenter()
        
        CFNotificationCenterAddObserver(
            center,
            self.observerPointer,
            myCallback,
            Self.certificateRetrievalStartedNotificationName,
            nil,
            .deliverImmediately
        )
        CFNotificationCenterAddObserver(
            center,
            self.observerPointer,
            myCallback,
            Self.certificateRetrievalCompletedNotificationName,
            nil,
            .deliverImmediately
        )
    }
    
    fileprivate func handleNotification(name: CFString) {
        // Add handling logic for different notifications
        if name == Self.certificateRetrievalStartedNotificationName {
            // Do anything to react to process start here
            print("Certificate retrieval app has launched!")
        } else if name == Self.certificateRetrievalCompletedNotificationName {
            self.certificateRetrieved?()
        }
    }
}

fileprivate func myCallback(
    _ center: CFNotificationCenter?,
    _ observer: UnsafeMutableRawPointer?,
    _ name: CFNotificationName?,
    _ object: UnsafeRawPointer?,
    _ userInfo: CFDictionary?
) {
    guard let observer = observer,
          let notificationName = name?.rawValue
    else { return }
    
    let instance = Unmanaged<DarwinListener>.fromOpaque(observer).takeUnretainedValue()
    instance.handleNotification(name: notificationName)
}
