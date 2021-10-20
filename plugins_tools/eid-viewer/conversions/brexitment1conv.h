#include "convworker.h"
#include <map>

class BrexitMention1Convertor :public ConversionWorker
{
private:
	//3 dimensions: graphical version, language and the actual conversion
	static std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > > conversions;
public:
	BrexitMention1Convertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlBrexitMention1Convertor :public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlBrexitMention1Convertor();
	virtual EID_STRING convert(const void *original);
};
