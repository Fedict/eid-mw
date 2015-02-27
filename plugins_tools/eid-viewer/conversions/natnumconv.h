#ifndef EID_VWR_NATNUM_CONV_H
#define EID_VWR_NATNUM_CONV_H

#include "convworker.h"

class NationalNumberConvertor : public ConversionWorker {
public:
	virtual std::string convert(const char* normal);
};

#endif
