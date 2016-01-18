#include "hexdecode.h"
#include <cstdlib>
#include <string>
#include "cppeidstring.h"

void* HexDecodeConvertor::convert(const void* original, int* len_return) {
	unsigned char* rv = (unsigned char*)malloc(len_);
	*len_return = len_;
	EID_STRING str((EID_CHAR*)original);
	for(int i=0; i<len_; i++) {
		rv[i] = EID_STRTOL(str.substr(i*2, 2).c_str(), NULL, 16);
	}

	return rv;
}
