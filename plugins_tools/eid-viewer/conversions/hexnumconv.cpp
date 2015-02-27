#include "hexnumconv.h"
#include <iostream>
#include <sstream>
#include <iomanip>

std::string HexNumberConvertor::convert(const char* normal) {
	unsigned char* uc = (unsigned char*)normal;
	std::ostringstream s;
	s << std::setfill('0') << std::hex << std::uppercase;
	for(int i=0; i<len_; i++) {
		s << std::setw(2) << (unsigned int) uc[i];
	}
	return s.str();
}
