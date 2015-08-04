#include "dob.h"

std::map<eid_vwr_langs, std::map<int, std::string> > DobWriter::tomonth;
std::map<std::string, int> DobParser::frommonth;

/*
The date of birth is written to the card in a localized format. This
means we need to read it from the card, parse it, and then convert it
to the same format in the language of the current user.

Clearly someone never heard of ISO 8601
*/

#define domap(l,n,s) frommonth[s] = n;
DobParser::DobParser() {
	if(frommonth.size() == 0) {
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
}
#undef domap
#define domap(l,n,s) tomonth[l][n] = s
DobWriter::DobWriter(DateParser* p) : DateWriter(p) {
	if(tomonth.size() == 0) {
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
}

void DobParser::input(std::string original) {
	day = original.substr(0,2);
	month = frommonth[original.substr(3, original.find_last_of(" .") - 3)];
	year = original.substr(original.find_last_of(" .") + 1);
}

std::string DobWriter::output() {
	char sep = target_ == EID_VWR_LANG_DE ? '.' : ' ';
	return parser->day + sep + tomonth[target_][parser->month] + sep + parser->year;
}
