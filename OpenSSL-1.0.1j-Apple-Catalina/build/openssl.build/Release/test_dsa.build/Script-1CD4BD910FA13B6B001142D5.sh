#!/bin/sh

funcs="${SRCROOT}/bin/test_functions.sh"
if [[ -f "${funcs}" ]]
then
	source "${funcs}"
	arches="$(get_exec_arches)"
else
	printf "Unable to source in functions: \"%s\"." "${funcs}"
fi

test_binary "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}"

test_binary "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}" -app2_1

