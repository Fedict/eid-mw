# BEIDToken: Using CryptoTokenKit to add support for BEID tokens

To disable the built-in PIV token this command can be used: sudo defaults write /Library/Preferences/com.apple.security.smartcard DisabledTokens -array com.apple.CryptoTokenKit.pivtoken 

The new extension will be automatically installed when the hosting application is installed.

## Requirements

This sample requires SmartCard reader, BEID SmartCard and OS X 10.12 or later

### Build

Xcode 8.0 or later; OS X 10.12 or later

### Runtime

OS X 10.12 or later

