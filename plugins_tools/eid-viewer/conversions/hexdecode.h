#ifndef EID_VWR_HEXDECODE_H
#define EID_VWR_HEXDECODE_H

#include "convworker.h"

class HexDecodeConvertor : public ConversionWorker {
private:
	int len_;
public:
	HexDecodeConvertor(int len) : len_(len) {};
	virtual std::string convert(const char* original) { return ""; };
	virtual void* convert(const char* original, int* len_return);
};

#endif
