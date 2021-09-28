#include "convworker.h"
#include <map>

class CardAMention1Convertor :public ConversionWorker
{
private:
	static std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <char, EID_STRING> > > conversions;
public:
	CardAMention1Convertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlCardAMention1Convertor :public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlCardAMention1Convertor();
	virtual EID_STRING convert(const void *original);
};
