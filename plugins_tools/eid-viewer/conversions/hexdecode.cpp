#include "hexdecode.h"
#include <cstdlib>
#include <string>
#include "cppeidstring.h"

//convert the chip number representation back into a 16 bytes long array
void *HexDecodeConvertor::convert(const void *original, int *len_return)
{
	unsigned char *rv = (unsigned char *) malloc(len_);

	*len_return = len_;
	EID_STRING str((EID_CHAR *) original);

	for (int i = 0; i < len_; i++)
	{
		//2 characters with base 16 will fit into 1 byte
		rv[i] = (unsigned char) EID_STRTOL(str.substr(i * 2, 2).
						   c_str(), NULL, 16);
	}

	return rv;
}
