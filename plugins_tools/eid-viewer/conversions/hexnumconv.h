#ifndef EID_VWR_CHIPNUM_CONV_H
#define EID_VWR_CHIPNUM_CONV_H

#include "convworker.h"

class HexNumberConvertor : public ConversionWorker {
private:
	HexNumberConvertor() {};
	int len_;
public:
	HexNumberConvertor(int len) : len_(len) { };
	virtual std::string convert(const char* normal);
};

#endif
