#include "valdate.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

void ValidityDateParser::input(std::string original) {
	day = original.substr(0, 2);
	month = strtol(original.substr(3, 2).c_str(), NULL, 10);
	year = original.substr(6, 4);
}

std::string ValidityDateWriter::output() {
	std::ostringstream s;
	s << parser->day << "." << std::setfill('0') << std::setw(2) << parser->month << parser->year;
	return s.str();
}
