#ifndef EID_VWR_BBAN_CONV_H
#define EID_VWR_BBAN_CONV_H

#include "convworker.h"

class BBANNumberConvertor:public ConversionWorker
{
public:
	virtual EID_STRING convert(const void *normal);
};

#endif
