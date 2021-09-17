#include "brexitment1conv.h"

//3 dimensions: graphical version, language and the actual conversion
std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <char, EID_STRING> > >BrexitMention1Convertor::conversions;

BrexitMention1Convertor::BrexitMention1Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['B'] = TEXT("Artikel 18(1) Vereinbarung");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['B'] = TEXT("Article 18(1) Agreement");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['B'] = TEXT("Article 18(1) Accord");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['B'] = TEXT("Artikel 18(1) Akkoord");

		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_DE]['B'] = TEXT("ARTIKEL 18(1) VEREINBARUNG");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_EN]['B'] = TEXT("ARTICLE 18(1) AGREEMENT");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_FR]['B'] = TEXT("ARTICLE 18(1) ACCORD");
		conversions[EID_VWR_GRAPH_VERSION_NINE][EID_VWR_LANG_NL]['B'] = TEXT("ARTIKEL 18(1) AKKOORD");
	}
}

EID_STRING BrexitMention1Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[graphvers_][target_][*norm];
}


std::map < EID_STRING, EID_STRING > XmlBrexitMention1Convertor::conversions;

XmlBrexitMention1Convertor::XmlBrexitMention1Convertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("B")] = TEXT("Article 18(1) Agreement");
		conversions[TEXT("Article 18(1) Agreement")] = TEXT("B");
	}
}

EID_STRING XmlBrexitMention1Convertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
