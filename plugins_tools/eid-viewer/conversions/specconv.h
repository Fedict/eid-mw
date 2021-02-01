#ifndef EID_VWR_SPEC_CONV_H
#define EID_VWR_SPEC_CONV_H

#include "convworker.h"
#include <conversions.h>
#include <map>

class SpecConvertor:public ConversionWorker
{
private:
	static std::map < eid_vwr_langs, std::map < EID_CHAR, EID_STRING > >conversions;
public:
	SpecConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
