﻿#include "brexitment1conv.h"

std::map < eid_vwr_langs, std::map <char, EID_STRING> > BrexitMention1Convertor::conversions;

BrexitMention1Convertor::BrexitMention1Convertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_LANG_DE]['B'] = TEXT("Artikel 18(1) Vereinbarung");
		conversions[EID_VWR_LANG_EN]['B'] = TEXT("Article 18(1) Agreement");
		conversions[EID_VWR_LANG_FR]['B'] = TEXT("Article 18(1) Accord");
		conversions[EID_VWR_LANG_NL]['B'] = TEXT("Artikel 18(1) Akkoord");
	}
}

EID_STRING BrexitMention1Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[target_][*norm];
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
