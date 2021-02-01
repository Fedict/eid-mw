#ifndef EID_VWR_VALDATE_H
#define EID_VWR_VALDATE_H

#include "dateconv.h"

class ValidityDateParser:public DateParser
{
public:
	virtual void input(EID_STRING);
};

class ValidityDateWriter:public DateWriter
{
public:
	ValidityDateWriter(DateParser * p):DateWriter(p)
	{
	};
	virtual EID_STRING output();
};

#endif
