#ifndef EID_VWR_XML_SPECCONV_H
#define EID_VWR_XML_SPECCONV_H

#include "convworker.h"
#include <map>
#include <string>

class XmlSpecConvertor:public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	       XmlSpecConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
