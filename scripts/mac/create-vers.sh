#!/bin/bash

source $(dirname $0)/set_eidmw_version.sh

gitrev=$(git describe --dirty)
NOW=$(date +%F)

cat > "$1" <<EOF
#ifndef BEID_VERSION_H
#define BEID_VERSION_H

#define BEID_COMPANY_NAME "Belgian Government"
#define BEID_COPYRIGHT "Copyright (c) 2021"
#define BEID_PRODUCT_NAME "Belgium eID Middleware"
#define BEID_PRODUCT_VERSION "$BASE_VERSION1.$BASE_VERSION2.$BASE_VERSION3"
#define BEID_VERSION_STRING "$BASE_VERSION1, $BASE_VERSION2, $BASE_VERSION3"
#define BEID_BUILD_DATE "$NOW"
#define BASE_VERSION1 $BASE_VERSION1
#define BASE_VERSION2 $BASE_VERSION2
#define BASE_VERSION3 $BASE_VERSION3
#define EIDMW_REVISION_STR "$gitrev"

#endif
EOF
