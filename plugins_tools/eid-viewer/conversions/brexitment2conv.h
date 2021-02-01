#include "convworker.h"
#include <map>

class BrexitMention2Convertor :public ConversionWorker
{
private:
	static std::map < eid_vwr_langs, std::map <char, EID_STRING > > conversions;
public:
	BrexitMention2Convertor();
	virtual EID_STRING convert(const void *normal);
};

class XmlBrexitMention2Convertor :public ConversionWorker
{
private:
	static std::map < EID_STRING, EID_STRING > conversions;
public:
	XmlBrexitMention2Convertor();
	virtual EID_STRING convert(const void *original);
};
