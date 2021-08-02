#include "brexitment2conv.h"

std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <char, EID_STRING> > > BrexitMention2Convertor::conversions;

BrexitMention2Convertor::BrexitMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['C'] = TEXT("Daueraufenthalt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['C'] = TEXT("Permanent stay");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['C'] = TEXT("Séjour permanent");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['C'] = TEXT("Duurzaam verblijf");

		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_DE]['C'] = TEXT("DAUERAUFENTHALT");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_EN]['C'] = TEXT("PERMANENT STAY");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_FR]['C'] = TEXT("SEJOUR PERMANENT");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_NL]['C'] = TEXT("DUURZAAM VERBLIJF");
	}
}

EID_STRING BrexitMention2Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[graphvers_][target_][*norm];
}

std::map < EID_STRING, EID_STRING > XmlBrexitMention2Convertor::conversions;

XmlBrexitMention2Convertor::XmlBrexitMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("C")] = TEXT("Permanent stay");
		conversions[TEXT("Permanent stay")] = TEXT("C");
	}
}

EID_STRING XmlBrexitMention2Convertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
