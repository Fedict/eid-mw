#ifndef EID_VWR_DOB_H
#define EID_VWR_DOB_H

#include "dateconv.h"
#include <map>
#include "cppeidstring.h"

class DobParser:public DateParser
{
private:
	static std::map < EID_STRING, int >frommonth;
public:
	    DobParser();
	virtual void input(EID_STRING);
};

class DobWriter:public DateWriter
{
private:
	static std::map < eid_vwr_langs, std::map < int,
		EID_STRING > >tomonth;
public:
	    DobWriter(DateParser * p);
	virtual EID_STRING output();
};

#endif
