#include "workpermitconv.h"

std::map < eid_vwr_langs, std::map <char, EID_STRING> > WorkPermitConvertor::conversions;

WorkPermitConvertor::WorkPermitConvertor() {
	if(conversions.size() == 0)
	{
		conversions[EID_VWR_LANG_DE]['7'] = TEXT("unlimited");
		conversions[EID_VWR_LANG_DE]['8'] = TEXT("limited");
		conversions[EID_VWR_LANG_DE]['9'] = TEXT("none");
		conversions[EID_VWR_LANG_DE]['A'] = TEXT("seasonal worker");

		conversions[EID_VWR_LANG_EN]['7'] = TEXT("unlimited");
		conversions[EID_VWR_LANG_EN]['8'] = TEXT("limited");
		conversions[EID_VWR_LANG_EN]['9'] = TEXT("none");
		conversions[EID_VWR_LANG_EN]['A'] = TEXT("seasonal worker");

		conversions[EID_VWR_LANG_FR]['7'] = TEXT("illimité");
		conversions[EID_VWR_LANG_FR]['8'] = TEXT("limité");
		conversions[EID_VWR_LANG_FR]['9'] = TEXT("non");
		conversions[EID_VWR_LANG_FR]['A'] = TEXT("saisonnier");

		conversions[EID_VWR_LANG_NL]['7'] = TEXT("onbeperkt");
		conversions[EID_VWR_LANG_NL]['8'] = TEXT("beperkt");
		conversions[EID_VWR_LANG_NL]['9'] = TEXT("geen");
		conversions[EID_VWR_LANG_NL]['A'] = TEXT("seizoensarbeider");
	}
}

EID_STRING WorkPermitConvertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[target_][*norm];
}
