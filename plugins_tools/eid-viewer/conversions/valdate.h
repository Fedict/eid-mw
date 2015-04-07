#ifndef EID_VWR_VALDATE_H
#define EID_VWR_VALDATE_H

#include "dateconv.h"

class ValidityDateParser : public DateParser {
public:
	virtual void input(std::string);
};

class ValidityDateWriter : public DateWriter {
public:
	ValidityDateWriter(DateParser* p) : DateWriter(p) {};
	virtual std::string output();
};

#endif
