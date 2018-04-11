#include "dateconv.h"
#include <sstream>
#include <cstdlib>
#include <iomanip>

void XmlDateParser::input(EID_STRING original)
{
	year = original.substr(0, 4);
	month = (int) EID_STRTOL(original.substr(4, 2).c_str(), NULL, 10);
	day = original.substr(6, 2);
	if (month == 0) {
		day = TEXT("  ");
	}
}

EID_STRING XmlDateWriter::output()
{
	EID_OSTRINGSTREAM s;

	s << parser->year << std::setfill(TEXT('0')) << std::setw(2)
		<< parser->month << (parser->day == TEXT("  ") ? TEXT("00") : parser->day);
	return s.str();
}
