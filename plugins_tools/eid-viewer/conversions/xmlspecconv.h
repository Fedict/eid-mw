#ifndef EID_VWR_XML_SPECCONV_H
#define EID_VWR_XML_SPECCONV_H

#include "convworker.h"
#include <map>
#include <string>

class XmlSpecConvertor : public ConversionWorker {
private:
	static std::map<std::string, std::string> conversions;
public:
	XmlSpecConvertor();
	virtual std::string convert(const char* original);
};

#endif
