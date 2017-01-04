#ifndef EID_VWR_GENDERCONV_H
#define EID_VWR_GENDERCONV_H

#include "convworker.h"

class GenderConvertor:public ConversionWorker
{
public:
	EID_STRING convert(const void *original);
};

class XmlGenderConvertor:public GenderConvertor
{
public:
	EID_STRING convert(const void *original);
};

#endif
