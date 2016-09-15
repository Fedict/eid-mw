# PIVToken: Using CryptoTokenKit to add support for new types of tokens

This sample demonstrates how to write an extension for CryptoTokenKit framework to support new types of SmartCards or any other cryptographic token.

The implementation is sample PIV standard token similar to the one shipped with OS X. 

To disable the built-in PIV token this command can be used: sudo defaults write /Library/Preferences/com.apple.security.smartcard DisabledTokens -array com.apple.CryptoTokenKit.pivtoken 

The new extension will be automatically installed when the hosting application is installed.

## Requirements

This sample requires SmartCard reader, PIV SmartCard and OS X 10.12 or later

### Build

Xcode 8.0 or later; iOS 10.0 SDK or later

### Runtime

iOS 10.0 or later

Copyright (C) 2016 Apple Inc. All rights reserved.
