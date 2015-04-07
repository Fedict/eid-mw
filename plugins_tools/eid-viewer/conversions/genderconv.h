#ifndef EID_VWR_GENDERCONV_H
#define EID_VWR_GENDERCONV_H

#include "convworker.h"

class GenderConvertor : public ConversionWorker {
public:
	std::string convert(const char* original);
};

class XmlGenderConvertor : public GenderConvertor {
public:
	std::string convert(const char* original);
};

#endif
