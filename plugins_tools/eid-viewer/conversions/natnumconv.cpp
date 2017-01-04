#include "natnumconv.h"

EID_STRING NationalNumberConvertor::convert(const void *original)
{
	const EID_STRING normal((EID_CHAR *) original);
	EID_STRING retval =
		normal.substr(0, 2) + TEXT(".") + normal.substr(2,
								2) +
		TEXT(".") + normal.substr(4, 2) + TEXT("-") + normal.substr(6,
									    3)
		+ TEXT(".") + normal.substr(9, 2);
	return retval;
}
