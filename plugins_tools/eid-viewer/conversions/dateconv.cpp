#include "dateconv.h"
#include <sstream>
#include <cstdlib>
#include <iomanip>

void XmlDateParser::input(std::string original) {
	year = original.substr(0,4);
	month = strtol(original.substr(4,2).c_str(), NULL, 10);
	day = original.substr(6,2);
}

std::string XmlDateWriter::output() {
	std::ostringstream s;
	s << parser->year << std::setfill('0') << std::setw(2) << parser->month << parser->day;
	return s.str();
}
