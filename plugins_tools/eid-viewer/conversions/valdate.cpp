#include "valdate.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include "cppeidstring.h"

void ValidityDateParser::input(EID_STRING original)
{
	day = original.substr(0, 2);
	month = (int) EID_STRTOL(original.substr(3, 2).c_str(), NULL, 10);
	year = original.substr(6, 4);
}

EID_STRING ValidityDateWriter::output()
{
	EID_OSTRINGSTREAM s;

	s << parser->
		day << TEXT(".") << std::setfill(TEXT('0')) << std::
		setw(2) << parser->month << TEXT(".") << parser->year;
	return s.str();
}
