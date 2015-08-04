#include "bbannumconv.h"
#include "backend.h"
#include <string>
#include <stdexcept>

/* BBAN numbers.
   On card: 123123456712
   Display: 123-1234567-12
 */
std::string BBANNumberConvertor::convert(const char* original) {
	const std::string normal(original);

	try {
		return normal.substr(0, 3) + "-" + normal.substr(3, 7) + "-" + normal.substr(10, 2);
	} catch (const std::out_of_range& oor) {
		be_log(EID_VWR_LOG_NORMAL, "Could not convert '%s' to display format: number too short", original);
		return normal;
	}
}
