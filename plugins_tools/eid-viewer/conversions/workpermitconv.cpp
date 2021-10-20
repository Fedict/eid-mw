#include "workpermitconv.h"

//3 dimensions: graphical version, language and the actual conversion
std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > >WorkPermitConvertor::conversions;

WorkPermitConvertor::WorkPermitConvertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("7")] = TEXT("unbegrenzt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("8")] = TEXT("begrenzt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("9")] = TEXT("nein");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("A")] = TEXT("Saisonarbeitnehmer");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("7")] = TEXT("unlimited");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("8")] = TEXT("limited");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("9")] = TEXT("none");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("A")] = TEXT("seasonal worker");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("7")] = TEXT("illimité");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("8")] = TEXT("limité");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("9")] = TEXT("non");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("A")] = TEXT("travailleur saisonnier");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("7")] = TEXT("onbeperkt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("8")] = TEXT("beperkt");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("9")] = TEXT("geen");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("A")] = TEXT("seizoensarbeider");


		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("7")] = TEXT("UNBEGRENZT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("8")] = TEXT("BEGRENZT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("9")] = TEXT("NEIN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("A")] = TEXT("SAISONARBEITER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("7")] = TEXT("UNLIMITED");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("8")] = TEXT("LIMITED");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("9")] = TEXT("NONE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("A")] = TEXT("SEASONAL WORKER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("7")] = TEXT("ILLIMITE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("8")] = TEXT("LIMITE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("9")] = TEXT("NON");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("A")] = TEXT("SAISONNIER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("7")] = TEXT("ONBEPERKT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("8")] = TEXT("BEPERKT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("9")] = TEXT("NEEN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("A")] = TEXT("SEIZOENARBEIDER");
	}
}


EID_STRING WorkPermitConvertor::convert(const void *normal) {
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


std::map < EID_STRING, EID_STRING > XmlWorkPermitConvertor::conversions;

XmlWorkPermitConvertor::XmlWorkPermitConvertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("7")] = TEXT("unlimited");
		conversions[TEXT("8")] = TEXT("limited");
		conversions[TEXT("9")] = TEXT("no");
		conversions[TEXT("A")] = TEXT("seasonal worker");
		conversions[TEXT("unlimited")] = TEXT("7");
		conversions[TEXT("limited")] = TEXT("8");
		conversions[TEXT("no")] = TEXT("9");
		conversions[TEXT("seasonal worker")] = TEXT("A");
	}
}

EID_STRING XmlWorkPermitConvertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
