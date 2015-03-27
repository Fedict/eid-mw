#include "xmlspecconv.h"

std::map<std::string, std::string> XmlSpecConvertor::conversions;

XmlSpecConvertor::XmlSpecConvertor()
{
	if(conversions.empty()) {
		conversions["NO_STATUS"] = "0";
		conversions["WHITE_CANE"] = "1";
		conversions["EXTENDED_MINORITY"] = "2";
		conversions["WHITE_CANE_EXTENDED_MINORITY"] = "3";
		conversions["YELLOW_CANE"] = "4";
		conversions["YELLOW_CANE_EXTENDED_MINORITY"] = "5";
		conversions["0"] = "NO_STATUS";
		conversions["1"] = "WHITE_CANE";
		conversions["2"] = "EXTENDED_MINORITY";
		conversions["3"] = "WHITE_CANE_EXTENDED_MINORITY";
		conversions["4"] = "YELLOW_CANE";
		conversions["5"] = "YELLOW_CANE_EXTENDED_MINORITY";
	}
}

std::string XmlSpecConvertor::convert(const char* original) {
	if(conversions.count(original) > 0) {
		return conversions[original];
	}
	return "";
}
