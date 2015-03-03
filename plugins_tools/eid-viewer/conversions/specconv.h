#ifndef EID_VWR_SPEC_CONV_H
#define EID_VWR_SPEC_CONV_H

#include "convworker.h"
#include <conversions.h>
#include <map>

class SpecConvertor : public ConversionWorker {
private:
	std::map<eid_vwr_langs, std::map<char, std::string> > conversions;
public:
	SpecConvertor();
	virtual std::string convert(const char* original);
};

#endif
