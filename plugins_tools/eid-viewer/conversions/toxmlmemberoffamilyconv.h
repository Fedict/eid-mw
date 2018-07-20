#ifndef EID_VWR_MEMOFAM_CONV_H
#define EID_VWR_MEMOFAM_CONV_H

#include "convworker.h"

class ToXmlMemberOfFamilyConvertor :public ConversionWorker
{
public:
	virtual EID_STRING convert(const void *normal);
};

#endif
