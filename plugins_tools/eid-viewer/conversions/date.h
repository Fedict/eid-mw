#ifndef EID_VWR_DATE_H
#define EID_VWR_DATE_H

#include <string>

class Date {
protected:
	std::string year;
	int month;
	std::string day;
public:
	Date() : year("0"), month(0), day("0") {};
	Date(std::string y, int m, std::string d) : year(y), month(m), day(d) {};
	virtual void input(std::string) = 0;
	virtual std::string output(void) = 0;
};

#endif
