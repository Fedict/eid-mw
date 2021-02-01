#include "specorgconv.h"
#include <eid-util/utftranslate.h>

#include <sstream>

std::map < eid_vwr_langs, std::map < EID_STRING, EID_STRING > > SpecOrgConvertor::conversions;

SpecOrgConvertor::SpecOrgConvertor()
{
	if (conversions.empty())
	{
		conversions[EID_VWR_LANG_DE][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_LANG_DE][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_LANG_DE][TEXT("4")] = TEXT("ehemaliger Inhaber der Blauen Karte EU");
		conversions[EID_VWR_LANG_DE][TEXT("5")] = TEXT("Forscher");

		conversions[EID_VWR_LANG_EN][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_LANG_EN][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_LANG_EN][TEXT("4")] = TEXT("Former EU Blue Card holder");
		conversions[EID_VWR_LANG_EN][TEXT("5")] = TEXT("Researcher");

		conversions[EID_VWR_LANG_FR][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_LANG_FR][TEXT("2")] = TEXT("OTAN");
		conversions[EID_VWR_LANG_FR][TEXT("4")] = TEXT("Ancien titulaire d'une carte bleue européenne");
		conversions[EID_VWR_LANG_FR][TEXT("5")] = TEXT("Chercheur");

		conversions[EID_VWR_LANG_NL][TEXT("1")] = TEXT("SHAPE");
		conversions[EID_VWR_LANG_NL][TEXT("2")] = TEXT("NATO");
		conversions[EID_VWR_LANG_NL][TEXT("4")] = TEXT("Voormalig houder van Europese blauwe kaart");
		conversions[EID_VWR_LANG_NL][TEXT("5")] = TEXT("Onderzoeker");
	}
}

EID_STRING SpecOrgConvertor::convert(const void *normal)
{
	if(conversions.count(target_) > 0)
	{
		EID_STRING type = (EID_CHAR*)normal;
		if(conversions[target_].count(type) > 0)
		{
			return conversions[target_][type];
		}
	}
	return (EID_CHAR *) normal;
}
