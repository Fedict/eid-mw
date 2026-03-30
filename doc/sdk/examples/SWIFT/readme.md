# Triggering certificate retrieval process

Certificate retrieval is triggered by opening the following simple deeplink: beidsign://certificate

# Observing process completion

The BeIDSign app uses macOS built-in Darwin notifications to inform about the state of the certificate retrieval process. This notification system can be natively used from Swift, Objective-C, C++ and C, as well as Java with a JNI wrapper.

2 notification names can be listened to:

 - com.zetes.pc-signing.certificate-requested: this is sent by the BeIDSign app as soon as the certificate retrieval request has been received, so almost immediately after the deeplink as been opened. It can be used by a 3rd party application to know that the request has been correctly received, and to display an error message after a few seconds if it hasn’t.
    
- com.zetes.pc-signing.certificate-retrieved: this is sent by the BeIDSign app when the certificate retrieval process is completed (whether it was successful or not, e.g. in case of an error or user cancellation). Once received, a 3rd party application can start querying the macOS keychain to read the retrieved remote signing certificates.