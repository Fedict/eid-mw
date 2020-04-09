#!/bin/sh
# Change the fixed dependency path of our pkcs11 module in BeidView to a runpath

cd "${CONFIGURATION_BUILD_DIR}"/"${CONTENTS_FOLDER_PATH}"
install_name_tool -change /usr/local/lib/libbeidpkcs11.5.0.1.dylib "@rpath/libbeidpkcs11.5.0.1.dylib" "BeidView"

