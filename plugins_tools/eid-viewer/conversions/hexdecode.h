#ifndef EID_VWR_HEXDECODE_H
#define EID_VWR_HEXDECODE_H

#include "convworker.h"

class HexDecodeConvertor:public ConversionWorker
{
private:
	int len_;
public:
	    HexDecodeConvertor(int len):len_(len)
	{
	};
	virtual EID_STRING convert(const void *)
	{
		return TEXT("");
	};
	virtual void *convert(const void *original, int *len_return);
};

#endif
