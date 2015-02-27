#include "natnumconv.h"

std::string NationalNumberConvertor::convert(const char* original) {
	const std::string normal(original);
	std::string retval = normal.substr(0, 2) + "." + normal.substr(2, 2) +
		"." + normal.substr(4, 2) + "-" + normal.substr(6, 3) + "." +
		normal.substr(9, 2);
	return retval;
}
