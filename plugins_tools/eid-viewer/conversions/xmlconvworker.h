#ifndef XML_CONVERTOR_H
#define XML_CONVERTOR_H

#include "convworker.h"
#include <cstring>

class XmlConversionWorker : public ConversionWorker {
public:
	virtual std::string convert(const char* original) = 0;
	virtual void* convert(const char* original, int* len_return) {
		std::string cv = convert(original);
		*len_return = cv.length();
		return strdup(cv.c_str());
	}
};

#endif
