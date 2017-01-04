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
	}
}

EID_STRING XmlDoctypeConvertor::convert(const void *original)
{
	EID_STRING str((EID_CHAR *) original);

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
