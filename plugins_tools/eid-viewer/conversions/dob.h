#ifndef EID_VWR_DOB_H
#define EID_VWR_DOB_H

#include "dateconv.h"
#include <map>

class DobParser : public DateParser {
private:
	static std::map<std::string, int> frommonth;
public:
	DobParser();
	virtual void input(std::string);
};

class DobWriter : public DateWriter {
private:
	static std::map<eid_vwr_langs, std::map<int, std::string> > tomonth;
public:
	DobWriter(DateParser* p);
	virtual std::string output();
};

#endif
