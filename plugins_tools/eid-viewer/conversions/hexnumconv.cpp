#include "hexnumconv.h"
#include <iostream>
#include <sstream>
#include <iomanip>

EID_STRING HexNumberConvertor::convert(const void *normal)
{
	unsigned char *uc = (unsigned char *) normal;
	EID_OSTRINGSTREAM s;

	s << std::setfill(TEXT('0')) << std::hex << std::uppercase;
	for (int i = 0; i < len_; i++)
	{
		s << std::setw(2) << (unsigned int) uc[i];
	}
	return s.str();
}
