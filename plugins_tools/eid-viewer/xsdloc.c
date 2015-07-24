#ifndef HAVE_CONFIG_H
#error This file requires the autotools build system...
#endif

#include "config.h"

/* Return the XSD location on non-OSX POSIX systems */
const char* get_xsdloc(void) {
	return DATAROOTDIR "/" PACKAGE_NAME "/eidv4.xsd";
}
