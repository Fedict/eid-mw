#!/bin/bash

#-----------------------------------------
# set the Debug build definitions
# or on the console with: export MAC_BUILD_CONFIG=Debug
#-----------------------------------------

MAC_BUILD_CONFIG=Debug
SIGN_BUILD=0

SIGN_BUILD=$SIGN_BUILD MAC_BUILD_CONFIG=$MAC_BUILD_CONFIG ./make-mac.sh


