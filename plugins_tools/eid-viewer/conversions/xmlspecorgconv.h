#ifndef EID_VWR_XMLSPECORGCONV_H
#define EID_VWR_XMLSPECORGCONV_H

#include <string>
#include <map>
#include "convworker.h"

class XmlSpecOrgConvertor:public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING >conversions;
public:
	XmlSpecOrgConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
