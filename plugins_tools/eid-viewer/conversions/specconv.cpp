#include "specconv.h"

std::map < eid_vwr_langs, std::map < EID_CHAR, EID_STRING > >SpecConvertor::conversions;

SpecConvertor::SpecConvertor()
{
	conversions[EID_VWR_LANG_DE]['0'] = TEXT("-");
	conversions[EID_VWR_LANG_DE]['1'] = TEXT("Weißer Stock");
	conversions[EID_VWR_LANG_DE]['2'] = TEXT("Verlängerte Minderjährigkeit");
	conversions[EID_VWR_LANG_DE]['3'] = TEXT("Weißer Stock\nVerlängerte Minderjährigkeit");
	conversions[EID_VWR_LANG_DE]['4'] = TEXT("Gelber Stock");
	conversions[EID_VWR_LANG_DE]['5'] = TEXT("Gelber Stock\nVerlängerte Minderjährigkeit");

	conversions[EID_VWR_LANG_EN]['0'] = TEXT("-");
	conversions[EID_VWR_LANG_EN]['1'] = TEXT("White Cane");
	conversions[EID_VWR_LANG_EN]['2'] = TEXT("Extended Nonage");
	conversions[EID_VWR_LANG_EN]['3'] = TEXT("White Cane\nExtended Nonage");
	conversions[EID_VWR_LANG_EN]['4'] = TEXT("Yellow Cane");
	conversions[EID_VWR_LANG_EN]['5'] = TEXT("Yellow Cane\nExtended Nonage");

	conversions[EID_VWR_LANG_FR]['0'] = TEXT("-");
	conversions[EID_VWR_LANG_FR]['1'] = TEXT("Canne blanche");
	conversions[EID_VWR_LANG_FR]['2'] = TEXT("Minorité étendue");
	conversions[EID_VWR_LANG_FR]['3'] = TEXT("Canne blanche\nMinorité étendue");
	conversions[EID_VWR_LANG_FR]['4'] = TEXT("Canne jaune");
	conversions[EID_VWR_LANG_FR]['5'] = TEXT("Canne jaune\nMinorité étendue");

	conversions[EID_VWR_LANG_NL]['0'] = TEXT("-");
	conversions[EID_VWR_LANG_NL]['1'] = TEXT("Witte stok");
	conversions[EID_VWR_LANG_NL]['2'] = TEXT("Verlengde minderjarigheid");
	conversions[EID_VWR_LANG_NL]['3'] = TEXT("Witte stok\nVerlengde minderjarigheid");
	conversions[EID_VWR_LANG_NL]['4'] = TEXT("Gele stok");
	conversions[EID_VWR_LANG_NL]['5'] = TEXT("Gele stok\nVerlengde minderjarigheid");
}

EID_STRING SpecConvertor::convert(const void *original)
{
	const EID_CHAR *eid_original = (const EID_CHAR *) original;

	if (conversions.count(target_) > 0)
	{
		if (conversions[target_].count(eid_original[0]) > 0)
		{
			return conversions[target_][eid_original[0]];
		}
	}
	return eid_original;
}
