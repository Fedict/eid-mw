#ifndef EID_VWR_SPECORGCONV_H
#define EID_VWR_SPECORGCONV_H

#include "convworker.h"
#include <map>
#include <string>

class SpecOrgConvertor:public ConversionWorker
{
private:
	static std::map < eid_vwr_langs,
		std::map < EID_STRING, EID_STRING > > conversions;
public:
	SpecOrgConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
