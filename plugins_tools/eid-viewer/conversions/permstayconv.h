#include "convworker.h"
#include <map>

class PermStayConvertor:public ConversionWorker
{
private:
	static std::map < eid_vwr_langs, std::map <char, EID_STRING > > conversions;
public:
	PermStayConvertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlPermStayConvertor :public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlPermStayConvertor();
	virtual EID_STRING convert(const void *original);
};
