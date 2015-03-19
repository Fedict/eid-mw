#include "bbannumconv.h"

std::string BBANNumberConvertor::convert(const char* original) {
	const std::string normal(original);

	std::string retval = normal.substr(0, 3) + "-" + normal.substr(3, 7) + "-" + normal.substr(10, 2);
	return retval;
}
