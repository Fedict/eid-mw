#include "cardAment2conv.h"

std::map < eid_vwr_langs, std::map <char, EID_STRING> > CardAMention2Convertor::conversions;

CardAMention2Convertor::CardAMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_LANG_DE]['K'] = TEXT("Programm mit Mobilit\u00E4tsma\u00DFnahmen");
		conversions[EID_VWR_LANG_EN]['K'] = TEXT("Mobility program");
		conversions[EID_VWR_LANG_FR]['K'] = TEXT("programme de mobilit\u00E9");
		conversions[EID_VWR_LANG_NL]['K'] = TEXT("mobiliteitsprogramma");
	}
}

EID_STRING CardAMention2Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[target_][*norm];
}

std::map < EID_STRING, EID_STRING > XmlCardAMention2Convertor::conversions;

XmlCardAMention2Convertor::XmlCardAMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("K")] = TEXT("Mobility program");
		conversions[TEXT("Mobility program")] = TEXT("K");
	}
}

EID_STRING XmlCardAMention2Convertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
