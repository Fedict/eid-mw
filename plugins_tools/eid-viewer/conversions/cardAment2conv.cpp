#include "cardAment2conv.h"

std::map <eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > > CardAMention2Convertor::conversions;

CardAMention2Convertor::CardAMention2Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("K")] = TEXT("Programm mit Mobilit\u00E4tsma\u00DFnahmen");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("K")] = TEXT("Mobility program");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("K")] = TEXT("programme de mobilit\u00E9");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("K")] = TEXT("mobiliteitsprogramma");


		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("K")] = TEXT("PROGRAMM MIT MOBILITATSMAβNAHMEN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("K")] = TEXT("MOBILITY PROGRAM");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("K")] = TEXT("PROGRAMME DE MOBILITE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("K")] = TEXT("MOBILITEITSPROGRAMMA");
	}
}

EID_STRING CardAMention2Convertor::convert(const void *normal) {
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
