#include "convworker.h"
#include <map>

class CardAMention2Convertor :public ConversionWorker
{
private:
	static std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <char, EID_STRING> > > conversions;
public:
	CardAMention2Convertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlCardAMention2Convertor :public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlCardAMention2Convertor();
	virtual EID_STRING convert(const void *original);
};
