#ifndef XML_DOCTYPECONV_H
#define XML_DOCTYPECONV_H

#include "xmlconvworker.h"
#include <map>
#include <string>

class XmlDoctypeConvertor : public XmlConversionWorker {
private:
	static std::map<std::string, std::string> conversions;
public:
	XmlDoctypeConvertor();
	virtual std::string convert(const char* original);
};

#endif
