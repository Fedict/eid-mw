#include "dateconv.h"

#define domap(l,n,s) tomonth[l][n] = s; frommonth[s] = n;

DateConvertor::DateConvertor() {
	domap(EID_VWR_LANG_DE, 1, "JAN");
	domap(EID_VWR_LANG_DE, 2, "FEB");
	domap(EID_VWR_LANG_DE, 3, "MÄR");
	domap(EID_VWR_LANG_DE, 4, "APR");
	domap(EID_VWR_LANG_DE, 5, "MAI");
	domap(EID_VWR_LANG_DE, 6, "JUN");
	domap(EID_VWR_LANG_DE, 7, "JUL");
	domap(EID_VWR_LANG_DE, 8, "AUG");
	domap(EID_VWR_LANG_DE, 9, "SEP");
	domap(EID_VWR_LANG_DE, 10, "OKT");
	domap(EID_VWR_LANG_DE, 11, "NOV");
	domap(EID_VWR_LANG_DE, 12, "DEZ");

	domap(EID_VWR_LANG_FR, 1, "JAN ");
	domap(EID_VWR_LANG_FR, 2, "FEV ");
	domap(EID_VWR_LANG_FR, 3, "MARS");
	domap(EID_VWR_LANG_FR, 4, "AVR ");
	domap(EID_VWR_LANG_FR, 5, "MAI ");
	domap(EID_VWR_LANG_FR, 6, "JUIN");
	domap(EID_VWR_LANG_FR, 7, "JUIL");
	domap(EID_VWR_LANG_FR, 8, "AOUT");
	domap(EID_VWR_LANG_FR, 9, "SEPT");
	domap(EID_VWR_LANG_FR, 10, "OCT ");
	domap(EID_VWR_LANG_FR, 11, "NOV ");
	domap(EID_VWR_LANG_FR, 12, "DEC ");

	domap(EID_VWR_LANG_NL, 1, "JAN ");
	domap(EID_VWR_LANG_NL, 2, "FEB ");
	domap(EID_VWR_LANG_NL, 3, "MAAR");
	domap(EID_VWR_LANG_NL, 4, "APR ");
	domap(EID_VWR_LANG_NL, 5, "MEI ");
	domap(EID_VWR_LANG_NL, 6, "JUN ");
	domap(EID_VWR_LANG_NL, 7, "JUL ");
	domap(EID_VWR_LANG_NL, 8, "AUG ");
	domap(EID_VWR_LANG_NL, 9, "SEP ");
	domap(EID_VWR_LANG_NL, 10, "OKT ");
	domap(EID_VWR_LANG_NL, 11, "NOV ");
	domap(EID_VWR_LANG_NL, 12, "DEC ");

	domap(EID_VWR_LANG_EN, 1, "JAN");
	domap(EID_VWR_LANG_EN, 2, "FEB");
	domap(EID_VWR_LANG_EN, 3, "MAR");
	domap(EID_VWR_LANG_EN, 4, "APR");
	domap(EID_VWR_LANG_EN, 5, "MAY");
	domap(EID_VWR_LANG_EN, 6, "JUN");
	domap(EID_VWR_LANG_EN, 7, "JUL");
	domap(EID_VWR_LANG_EN, 8, "AUG");
	domap(EID_VWR_LANG_EN, 9, "SEP");
	domap(EID_VWR_LANG_EN, 10, "OCT");
	domap(EID_VWR_LANG_EN, 11, "NOV");
	domap(EID_VWR_LANG_EN, 12, "DEC");
}

std::string DateConvertor::convert(const char* original) {
	std::string orig = original;
	std::string daypart = orig.substr(0,2);
	std::string monthpart = orig.substr(3, orig.find_last_of(" .") - 3);
	std::string yearpart = orig.substr(orig.find_last_of(" .") + 1);

	char sep = target_ == EID_VWR_LANG_DE ? '.' : ' ';

	return daypart + sep + tomonth[target_][frommonth[monthpart]] + sep + yearpart;
}
