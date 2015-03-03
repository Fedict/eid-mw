#include "specconv.h"

SpecConvertor::SpecConvertor() {
	conversions[EID_VWR_LANG_DE]['0'] = "";
	conversions[EID_VWR_LANG_DE]['1'] = "Weißer Stock";
	conversions[EID_VWR_LANG_DE]['2'] = "Verlängerte Minderjährigkeit";
	conversions[EID_VWR_LANG_DE]['3'] = "Weißer Stock\nVerlängerte Minderjährigkeit";
	conversions[EID_VWR_LANG_DE]['4'] = "Gelber Stock";
	conversions[EID_VWR_LANG_DE]['5'] = "Gelber Stock\nVerlängerte Minderjährigkeit";

	conversions[EID_VWR_LANG_EN]['0'] = "";
	conversions[EID_VWR_LANG_EN]['1'] = "White Cane";
	conversions[EID_VWR_LANG_EN]['2'] = "Extended Nonage";
	conversions[EID_VWR_LANG_EN]['3'] = "White Cane\nExtended Nonage";
	conversions[EID_VWR_LANG_EN]['4'] = "Yellow Cane";
	conversions[EID_VWR_LANG_EN]['5'] = "Yellow Cane\nExtended Nonage";

	conversions[EID_VWR_LANG_FR]['0'] = "";
	conversions[EID_VWR_LANG_FR]['1'] = "Canne blanche";
	conversions[EID_VWR_LANG_FR]['2'] = "Minorité étendue";
	conversions[EID_VWR_LANG_FR]['3'] = "Canne blanche\nMinorité étendue";
	conversions[EID_VWR_LANG_FR]['4'] = "Canne jaune";
	conversions[EID_VWR_LANG_FR]['5'] = "Canne jaune\nMinorité étendue";

	conversions[EID_VWR_LANG_NL]['0'] = "";
	conversions[EID_VWR_LANG_NL]['1'] = "Witte stok";
	conversions[EID_VWR_LANG_NL]['2'] = "Verlengde minderjarigheid";
	conversions[EID_VWR_LANG_NL]['3'] = "Witte stok\nVerlengde minderjarigheid";
	conversions[EID_VWR_LANG_NL]['4'] = "Gele stok";
	conversions[EID_VWR_LANG_NL]['5'] = "Gele stok\nVerlengde minderjarigheid";
}

std::string SpecConvertor::convert(const char* original) {
	if(conversions.count(target_) > 0) {
		if(conversions[target_].count(original[0]) > 0) {
			return conversions[target_][original[0]];
		}
	}
	return original;
}
