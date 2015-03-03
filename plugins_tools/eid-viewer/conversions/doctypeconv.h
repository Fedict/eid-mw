#ifndef EID_VWR_DOCTYPE_CONV_H
#define EID_VWR_DOCTYPE_CONV_H

#include <string>
#include <map>
#include "convworker.h"

class DocTypeConvertor : public ConversionWorker {
private:
	std::map<eid_vwr_langs, std::map<std::string, std::string> > conversions;
public:
	DocTypeConvertor();
	virtual std::string convert(const char* original);
};

#endif
