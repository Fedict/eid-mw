#include "bbannumconv.h"
#include "backend.h"
#include <string>
#include <stdexcept>
#include "cppeidstring.h"

/* BBAN numbers.
   On card: 123123456712
   Display: 123-1234567-12
 */
EID_STRING BBANNumberConvertor::convert(const void *original)
{
	const EID_STRING normal((const EID_CHAR *) original);

	try
	{
		return normal.substr(0, 3) + TEXT("-") + normal.substr(3, 7) + TEXT("-") + normal.substr(10, 2);
	}
	catch(const std::out_of_range & oor)
	{
		be_log(EID_VWR_LOG_NORMAL, TEXT("Could not convert card number %s to display format: number too short"), (const EID_CHAR *) original);
		(void)oor;//unused variable
		return normal;
	}
}
