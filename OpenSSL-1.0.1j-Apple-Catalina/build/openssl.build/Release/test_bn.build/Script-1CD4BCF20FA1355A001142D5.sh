#!/bin/sh


funcs="${SRCROOT}/bin/test_functions.sh"
if [[ -f "${funcs}" ]]
then
	source "${funcs}"
	arches="$(get_exec_arches)"
else
	printf "Unable to source in functions: \"%s\"." "${funcs}"
fi

for aa in ${arches}
do
	echo starting $aa big number library test, could take a while...
	bn_file="${TEMP_FILE_DIR}/tmp.bntest.$aa"

	arch -$aa "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}" > "${bn_file}"

	echo quit >> "${bn_file}"
	echo running bc

<$bn_file  sh -c "`sh "${SRCROOT}/src/test/bctest" ignore`" | /usr/bin/perl -e '$i=0; while (<STDIN>) {if (/^test (.*)/) {print STDERR "\nverify $1";} elsif (!/^0$/) {die "\nFailed! bc: $_";    } else {print STDERR "."; $i++;}} print STDERR "\n$i tests passed\n"'

done
