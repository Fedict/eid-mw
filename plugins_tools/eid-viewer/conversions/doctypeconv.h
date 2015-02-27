#ifndef EID_VWR_CONV_WORKER_H
#define EID_VWR_CONV_WORKER_H

#include <string>

class DocTypeConvertor : public ConversionWorker {
public:
	virtual std::string convert(const char* original);
};

#endif
