#ifndef EID_VWR_BOOLDECODE_H
#define EID_VWR_BOOLDECODE_H

#include "convworker.h"

class BoolDecodeConvertor :public ConversionWorker
{
public:
	virtual EID_STRING convert(const void *original);
	virtual void *convert(const void *original, int *len_return);
};

#endif