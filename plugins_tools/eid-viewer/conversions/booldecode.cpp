#include "booldecode.h"
#include <iostream>
#include <sstream>
#include <iomanip>

EID_STRING BoolDecodeConvertor::convert(const void *)
{
	return(TEXT(""));
}

//convert the boolean number representation back into a byte
void *BoolDecodeConvertor::convert(const void *original, int *len_return)
{
	unsigned char *rv = (unsigned char *)malloc(1);

	*len_return = 1;

	if ( (EID_STRCMP((EID_CHAR *)original, TEXT("false")) == 0) || (EID_STRCMP((EID_CHAR *)original, TEXT("0")) == 0) )
	{
		*rv = 0;
	}
	else {
		*rv = 1;
	}

	return rv;
}
