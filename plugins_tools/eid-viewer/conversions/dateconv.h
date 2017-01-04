#ifndef EID_VWR_DATECONV_H
#define EID_VWR_DATECONV_H

#include <string>
#include "convworker.h"

struct Date
{
	EID_STRING year;
	int month;
	EID_STRING day;

	           Date():year(TEXT("0")), month(0), day(TEXT("0"))
	{
	};
	Date(EID_STRING y, int m, EID_STRING d):year(y), month(m), day(d)
	{
	};
};

class DateParser:public Date
{
public:
	virtual void input(EID_STRING) = 0;
};

class DateWriter:public ConversionWorker
{
private:
	DateWriter()
	{
	};

protected:
	DateParser * parser;
public:
DateWriter(DateParser * p):parser(p)
	{
	};
	virtual EID_STRING output() = 0;
	virtual EID_STRING convert(const void *original)
	{
		parser->input((EID_CHAR *) original);
		return output();
	}
};

class XmlDateParser:public DateParser
{
public:
	virtual void input(EID_STRING);
};

class XmlDateWriter:public DateWriter
{
public:
	XmlDateWriter(DateParser * p):DateWriter(p)
	{
	};
	virtual EID_STRING output();
};

#endif
