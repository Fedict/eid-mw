#ifndef EID_VWR_XML_DATECONV_H
#define EID_VWR_XML_DATECONV_H

#include "xmlconvworker.h"

class XmlDateConvertor : public XmlConversionWorker {
public:
	XmlDateConvertor();
	virtual std::string convert(const char* original);
};
#endif
