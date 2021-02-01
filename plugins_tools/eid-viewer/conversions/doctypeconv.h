#ifndef EID_VWR_DOCTYPE_CONV_H
#define EID_VWR_DOCTYPE_CONV_H

#include <string>
#include <map>
#include "convworker.h"

class DocTypeConvertor:public ConversionWorker
{
private:
	static std::map < eid_vwr_langs, std::map < EID_STRING, EID_STRING > >conversions;
public:
	DocTypeConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
