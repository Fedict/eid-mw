#include "xmldoctypeconv.h"

std::map<std::string, std::string> XmlDoctypeConvertor::conversions;

XmlDoctypeConvertor::XmlDoctypeConvertor() {
	if(conversions.empty()) {
		conversions["1"] = "belgian_citizen";
		conversions["6"] = "kids_card";
		conversions["7"] = "bootstrap_card";
		conversions["8"] = "habilitation_card";
		conversions["11"] = "foreigner_a";
		conversions["12"] = "foreigner_b";
		conversions["13"] = "foreigner_c";
		conversions["14"] = "foreigner_d";
		conversions["15"] = "foreigner_e";
		conversions["16"] = "foreigner_e_plus";
		conversions["17"] = "foreigner_f";
		conversions["18"] = "foreigner_f_plus";
		conversions["19"] = "european_blue_card_h";
		conversions["belgian_citizen"] = "1";
		conversions["kids_card"] = "6";
		conversions["bootstrap_card"] = "7";
		conversions["habilitation_card"] = "8";
		conversions["foreigner_a"] = "11";
		conversions["foreigner_b"] = "12";
		conversions["foreigner_c"] = "13";
		conversions["foreigner_d"] = "14";
		conversions["foreigner_e"] = "15";
		conversions["foreigner_e_plus"]	= "16";
		conversions["foreigner_f"] = "17";
		conversions["foreigner_f_plus"] = "18";
		conversions["european_blue_card_h"] = "19";
	}
}

std::string XmlDoctypeConvertor::convert(const char* original) {
	std::string str(original);
	if(conversions.count(str) > 0) {
		return conversions[str];
	}
	if(conversions.count(str.substr(1, 1)) > 0) {
		return conversions[str.substr(1, 1)];
	}
	return "";
}
