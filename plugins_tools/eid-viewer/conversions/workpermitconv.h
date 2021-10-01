#include "convworker.h"
#include <map>

class WorkPermitConvertor:public ConversionWorker
{
private:
        static std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > > conversions;
public:
	WorkPermitConvertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlWorkPermitConvertor:public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlWorkPermitConvertor();
	virtual EID_STRING convert(const void *original);
};
