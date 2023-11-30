#include "xmldoctypeconv.h"

std::map < EID_STRING, EID_STRING > XmlDoctypeConvertor::conversions;

XmlDoctypeConvertor::XmlDoctypeConvertor()
{
	if (conversions.empty())
	{
		conversions[TEXT("1")] = TEXT("belgian_citizen");
		conversions[TEXT("6")] = TEXT("kids_card");
		conversions[TEXT("7")] = TEXT("bootstrap_card");
		conversions[TEXT("8")] = TEXT("habilitation_card");
		conversions[TEXT("11")] = TEXT("foreigner_a");
		conversions[TEXT("12")] = TEXT("foreigner_b");
		conversions[TEXT("13")] = TEXT("foreigner_c");
		conversions[TEXT("14")] = TEXT("foreigner_d");
		conversions[TEXT("15")] = TEXT("foreigner_e");
		conversions[TEXT("16")] = TEXT("foreigner_e_plus");
		conversions[TEXT("17")] = TEXT("foreigner_f");
		conversions[TEXT("18")] = TEXT("foreigner_f_plus");
		conversions[TEXT("19")] = TEXT("european_blue_card_h");
		conversions[TEXT("20")] = TEXT("ict_i");
		conversions[TEXT("21")] = TEXT("mobile_ict_j");
		conversions[TEXT("22")] = TEXT("foreigner_m");
		conversions[TEXT("23")] = TEXT("foreigner_n");
		conversions[TEXT("27")] = TEXT("foreigner_k");
		conversions[TEXT("28")] = TEXT("foreigner_l");
		conversions[TEXT("31")] = TEXT("foreigner_eu");
		conversions[TEXT("32")] = TEXT("foreigner_eu_plus");
		conversions[TEXT("33")] = TEXT("foreigner_A");//new visual (and doctype) of older a card
		conversions[TEXT("34")] = TEXT("foreigner_B");//new visual (and doctype) of older b card
		conversions[TEXT("35")] = TEXT("foreigner_F");//new visual (and doctype) of older f card
		conversions[TEXT("36")] = TEXT("foreigner_F_plus");//new visual (and doctype) of older f+ card
		conversions[TEXT("61")] = TEXT("kids_foreigner_eu");
		conversions[TEXT("62")] = TEXT("kids_foreigner_eu_plus");
		conversions[TEXT("63")] = TEXT("kids_foreigner_A");
		conversions[TEXT("64")] = TEXT("kids_foreigner_B");
		conversions[TEXT("65")] = TEXT("kids_foreigner_K");
		conversions[TEXT("66")] = TEXT("kids_foreigner_L");
		conversions[TEXT("67")] = TEXT("kids_foreigner_F");
		conversions[TEXT("68")] = TEXT("kids_foreigner_F_plus");
		conversions[TEXT("69")] = TEXT("kids_foreigner_M");
		conversions[TEXT("belgian_citizen")] = TEXT("1");
		conversions[TEXT("kids_card")] = TEXT("6");
		conversions[TEXT("bootstrap_card")] = TEXT("7");
		conversions[TEXT("habilitation_card")] = TEXT("8");
		conversions[TEXT("foreigner_a")] = TEXT("11");
		conversions[TEXT("foreigner_b")] = TEXT("12");
		conversions[TEXT("foreigner_c")] = TEXT("13");
		conversions[TEXT("foreigner_d")] = TEXT("14");
		conversions[TEXT("foreigner_e")] = TEXT("15");
		conversions[TEXT("foreigner_e_plus")] = TEXT("16");
		conversions[TEXT("foreigner_f")] = TEXT("17");
		conversions[TEXT("foreigner_f_plus")] = TEXT("18");
		conversions[TEXT("european_blue_card_h")] = TEXT("19");
		conversions[TEXT("ict_i")] = TEXT("20");
		conversions[TEXT("mobile_ict_j")] = TEXT("21");
		conversions[TEXT("foreigner_m")] = TEXT("22");
		conversions[TEXT("foreigner_n")] = TEXT("23");
		conversions[TEXT("foreigner_k")] = TEXT("27");
		conversions[TEXT("foreigner_l")] = TEXT("28");
		conversions[TEXT("foreigner_eu")] = TEXT("31");
		conversions[TEXT("foreigner_eu_plus")] = TEXT("32");
		conversions[TEXT("foreigner_A")] = TEXT("33");//new visual (and doctype) of older a card
		conversions[TEXT("foreigner_B")] = TEXT("34");//new visual (and doctype) of older b card
		conversions[TEXT("foreigner_F")] = TEXT("35");//new visual (and doctype) of older f card
		conversions[TEXT("foreigner_F_plus")] = TEXT("36");//new visual (and doctype) of older f+ card
		conversions[TEXT("kids_foreigner_eu")] = TEXT("61");
		conversions[TEXT("kids_foreigner_eu_plus")] = TEXT("62");
		conversions[TEXT("kids_foreigner_A")] = TEXT("63");
		conversions[TEXT("kids_foreigner_B")] = TEXT("64");
		conversions[TEXT("kids_foreigner_K")] = TEXT("65");
		conversions[TEXT("kids_foreigner_L")] = TEXT("66");
		conversions[TEXT("kids_foreigner_F")] = TEXT("67");
		conversions[TEXT("kids_foreigner_F_plus")] = TEXT("68");
		conversions[TEXT("kids_foreigner_M")] = TEXT("69");
	}
}

EID_STRING XmlDoctypeConvertor::convert(const void *original)
{
	EID_STRING str((EID_CHAR *) original);
	if (str.size() == 0)
	{
		return TEXT("");
	}

	if (conversions.count(str) > 0)
	{
		return conversions[str];
	}
	if (conversions.count(str.substr(1, 1)) > 0)
	{
		return conversions[str.substr(1, 1)];
	}
	return TEXT("");
}
