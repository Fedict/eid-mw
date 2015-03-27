#include "hexdecode.h"
#include <cstdlib>
#include <string>

void* HexDecodeConvertor::convert(const char* original, int* len_return) {
	unsigned char* rv = (unsigned char*)malloc(len_);
	*len_return = len_;
	std::string str(original);
	for(int i=0; i<len_; i++) {
		rv[i] = strtol(str.substr(i*2, 2).c_str(), NULL, 16);
	}

	return rv;
}
