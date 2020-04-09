#!/bin/sh

dir="${SRCROOT}/src/test/"

funcs="${SRCROOT}/bin/test_functions.sh"
if [[ -f "${funcs}" ]]
then
	source "${funcs}"
	arches="$(get_exec_arches)"
else
	printf "Unable to source in functions: \"%s\"." "${funcs}"
fi

echo "Generate and certify a test certificate"
test_binary_in_dir "${dir}" /bin/sh "${SRCROOT}/src/test/testss"

cat certCA.ss certU.ss > intP1.ss
cat certCA.ss certU.ss certP1.ss > intP2.ss

echo "test SSL protocol"
test_binary_in_dir "${dir}" "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}" -test_cipherlist

test_binary_in_dir "${dir}" /bin/sh "${dir}/testssl" keyU.ss certU.ss certCA.ss

test_binary_in_dir "${dir}" /bin/sh "${dir}/testsslproxy" keyP1.ss certP1.ss intP1.ss

test_binary_in_dir "${dir}" /bin/sh "${dir}/testsslproxy" keyP2.ss certP2.ss intP2.ss



