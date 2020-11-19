#include "art18conv.h"

std::map < eid_vwr_langs, std::map <char, EID_STRING> > Art18Convertor::conversions;

Art18Convertor::Art18Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_LANG_DE]['B'] = TEXT("Artikel 18(1) Vereinbarung");
		conversions[EID_VWR_LANG_EN]['B'] = TEXT("Article 18(1) Agreement");
		conversions[EID_VWR_LANG_FR]['B'] = TEXT("Article 18(1) Accord");
		conversions[EID_VWR_LANG_NL]['B'] = TEXT("Artikel 18(1) Akkoord");
	}
}

EID_STRING Art18Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[target_][*norm];
}

std::map < EID_STRING, EID_STRING > XmlArt18Convertor::conversions;

XmlArt18Convertor::XmlArt18Convertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("B")] = TEXT("Article 18(1) Agreement");
		conversions[TEXT("Article 18(1) Agreement")] = TEXT("B");
	}
}

EID_STRING XmlArt18Convertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
