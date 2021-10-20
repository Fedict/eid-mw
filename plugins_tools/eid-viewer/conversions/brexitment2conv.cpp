#include "brexitment2conv.h"

std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > > BrexitMention2Convertor::conversions;

BrexitMention2Convertor::BrexitMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("C")] = TEXT("Daueraufenthalt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("C")] = TEXT("Permanent stay");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("C")] = TEXT("Séjour permanent");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("C")] = TEXT("Duurzaam verblijf");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("C")] = TEXT("DAUERAUFENTHALT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("C")] = TEXT("PERMANENT STAY");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("C")] = TEXT("SEJOUR PERMANENT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("C")] = TEXT("DUURZAAM VERBLIJF");
	}
}

EID_STRING BrexitMention2Convertor::convert(const void *normal) {
	if (conversions.count(graphvers_) > 0)
	{
		if (conversions[graphvers_].count(target_) > 0)
		{
			EID_STRING norm = (EID_CHAR*)normal;
			if (conversions[graphvers_][target_].count(norm) > 0)
			{
				return conversions[graphvers_][target_][norm];
			}
		}
	}
	return (EID_CHAR*)normal;
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
