#ifndef EID_VWR_DATE_CONV_H
#define EID_VWR_DATE_CONV_H

#include "convworker.h"
#include <map>

class DateConvertor : public ConversionWorker {
private:
	std::map<eid_vwr_langs, std::map<int, std::string> > tomonth;
	std::map<std::string, int> frommonth;
public:
	DateConvertor();
	virtual std::string convert(const char* original);
};

#endif
