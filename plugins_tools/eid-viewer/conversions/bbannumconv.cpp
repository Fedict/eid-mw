#include "bbannumconv.h"
#include "backend.h"
#include <string>
#include <stdexcept>
#include "cppeidstring.h"

/* belgian eID card numbers. (12)
   On card: 123123456712
   Display: 123-1234567-12

   foreigner eID card numbers (10)
   On card: B123456789
   Display: B 1234567 89

   older foreigner eID card number (9)
   On card: 123456789
   Display: 123456789
 */


EID_STRING BBANNumberConvertor::convert(const void *original)
{
	const EID_STRING normal((const EID_CHAR *) original);

	try
	{
		switch (normal.length())
		{
		case 9:
			return normal;
		case 10:
			return normal.substr(0, 1) + TEXT(" ") + normal.substr(1, 7) + TEXT(" ") + normal.substr(8, 2);
		case 12:
		default:
			return normal.substr(0, 3) + TEXT("-") + normal.substr(3, 7) + TEXT("-") + normal.substr(10, 2);
		}
	}
	catch(const std::out_of_range & oor)
	{
		be_log(EID_VWR_LOG_NORMAL, TEXT("Could not convert card number %s to display format: number too short"), (const EID_CHAR *) original);
		(void)oor;//unused variable
		return normal;
	}
}
