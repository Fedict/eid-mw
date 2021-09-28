#include "specorgconv.h"
#include <eid-util/utftranslate.h>

#include <sstream>

//3 dimensions: graphical version, language and the actual conversion
std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map < EID_STRING, EID_STRING > > > SpecOrgConvertor::conversions;

SpecOrgConvertor::SpecOrgConvertor()
{
	if (conversions.empty())
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("4")] = TEXT("ehemaliger Inhaber der Blauen Karte EU");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("5")] = TEXT("Forscher");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("4")] = TEXT("Former EU Blue Card holder");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("5")] = TEXT("Researcher");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("2")] = TEXT("OTAN");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("4")] = TEXT("Ancien titulaire d'une carte bleue européenne");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("5")] = TEXT("Chercheur");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("4")] = TEXT("Voormalig houder van Europese blauwe kaart");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("5")] = TEXT("Onderzoeker");


		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("4")] = TEXT("EHEMALIGER INHABER DER BLAUEN KARTE EU");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("5")] = TEXT("FORSCHER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("4")] = TEXT("FORMER EU BLUE CARD HOLDER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("5")] = TEXT("RESEARCHER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("2")] = TEXT("OTAN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("4")] = TEXT("ANCIEN TITULAIRE D’UNE CARTE BLEUE UE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("5")] = TEXT("CHERCHEUR");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("4")] = TEXT("VOORMALIG HOUDER VAN EU BLAUWE KAART");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("5")] = TEXT("ONDERZOEKER");
	}
}

EID_STRING SpecOrgConvertor::convert(const void *normal)
{
	if (conversions.count(graphvers_) > 0)
	{
		if (conversions[graphvers_].count(target_) > 0)
		{
			EID_STRING type = (EID_CHAR*)normal;
			if (conversions[graphvers_][target_].count(type) > 0)
			{
				return conversions[graphvers_][target_][type];
			}
		}
	}
	return (EID_CHAR *) normal;
}
