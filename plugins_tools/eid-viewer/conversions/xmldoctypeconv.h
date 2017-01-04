#ifndef XML_DOCTYPECONV_H
#define XML_DOCTYPECONV_H

#include "convworker.h"
#include <map>
#include <string>

class XmlDoctypeConvertor:public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	       XmlDoctypeConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
