#include "convworker.h"
#include <map>

class Art18Convertor:public ConversionWorker
{
private:
	static std::map < eid_vwr_langs, std::map <char, EID_STRING > > conversions;
public:
	Art18Convertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlArt18Convertor:public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlArt18Convertor();
	virtual EID_STRING convert(const void *original);
};
