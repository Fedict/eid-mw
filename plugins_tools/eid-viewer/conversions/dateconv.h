#ifndef EID_VWR_DATECONV_H
#define EID_VWR_DATECONV_H

#include <string>
#include "convworker.h"

struct Date {
	std::string year;
	int month;
	std::string day;

	Date() : year("0"), month(0), day("0") {};
	Date(std::string y, int m, std::string d) : year(y), month(m), day(d) {};
};

class DateParser : public Date {
public:
	virtual void input(std::string) = 0;
};

class DateWriter : public ConversionWorker {
private:
	DateWriter() {};
protected:
	DateParser* parser;
public:
	DateWriter(DateParser* p) : parser(p) {};
	virtual std::string output() = 0;
	virtual std::string convert(const char* original) {
		parser->input(original);
		return output();
	}
};

class XmlDateParser : public DateParser {
public:
	virtual void input(std::string);
};

class XmlDateWriter : public DateWriter {
public:
	XmlDateWriter(DateParser* p) : DateWriter(p) {};
	virtual std::string output();
};

#endif
