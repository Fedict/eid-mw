#include "dob.h"
#include "cppeidstring.h"

std::map < eid_vwr_langs, std::map < int, EID_STRING > >DobWriter::tomonth;
std::map < EID_STRING, int >DobParser::frommonth;

/*
The date of birth is written to the card in a localized format. This
means we need to read it from the card, parse it, and then convert it
to the same format in the language of the current user.

Clearly someone never heard of ISO 8601
*/

#define domap(l,n,s) frommonth[s] = n;
DobParser::DobParser()
{
	if (frommonth.size() == 0)
	{
		domap(EID_VWR_LANG_DE, 0, TEXT("   "));
		domap(EID_VWR_LANG_DE, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_DE, 2, TEXT("FEB"));
		domap(EID_VWR_LANG_DE, 3, TEXT("MÄR"));
		domap(EID_VWR_LANG_DE, 4, TEXT("APR"));
		domap(EID_VWR_LANG_DE, 5, TEXT("MAI"));
		domap(EID_VWR_LANG_DE, 6, TEXT("JUN"));
		domap(EID_VWR_LANG_DE, 7, TEXT("JUL"));
		domap(EID_VWR_LANG_DE, 8, TEXT("AUG"));
		domap(EID_VWR_LANG_DE, 9, TEXT("SEP"));
		domap(EID_VWR_LANG_DE, 10, TEXT("OKT"));
		domap(EID_VWR_LANG_DE, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_DE, 12, TEXT("DEZ"));

		domap(EID_VWR_LANG_FR, 0, TEXT("   "));
		domap(EID_VWR_LANG_FR, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_FR, 2, TEXT("FEV"));
		domap(EID_VWR_LANG_FR, 3, TEXT("MARS"));
		domap(EID_VWR_LANG_FR, 4, TEXT("AVR"));
		domap(EID_VWR_LANG_FR, 5, TEXT("MAI"));
		domap(EID_VWR_LANG_FR, 6, TEXT("JUIN"));
		domap(EID_VWR_LANG_FR, 7, TEXT("JUIL"));
		domap(EID_VWR_LANG_FR, 8, TEXT("AOUT"));
		domap(EID_VWR_LANG_FR, 9, TEXT("SEPT"));
		domap(EID_VWR_LANG_FR, 10, TEXT("OCT"));
		domap(EID_VWR_LANG_FR, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_FR, 12, TEXT("DEC"));

		domap(EID_VWR_LANG_NL, 0, TEXT("   "));
		domap(EID_VWR_LANG_NL, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_NL, 2, TEXT("FEB"));
		domap(EID_VWR_LANG_NL, 3, TEXT("MAAR"));
		domap(EID_VWR_LANG_NL, 4, TEXT("APR"));
		domap(EID_VWR_LANG_NL, 5, TEXT("MEI"));
		domap(EID_VWR_LANG_NL, 6, TEXT("JUN"));
		domap(EID_VWR_LANG_NL, 7, TEXT("JUL"));
		domap(EID_VWR_LANG_NL, 8, TEXT("AUG"));
		domap(EID_VWR_LANG_NL, 9, TEXT("SEP"));
		domap(EID_VWR_LANG_NL, 10, TEXT("OKT"));
		domap(EID_VWR_LANG_NL, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_NL, 12, TEXT("DEC"));

		domap(EID_VWR_LANG_EN, 0, TEXT("   "));
		domap(EID_VWR_LANG_EN, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_EN, 2, TEXT("FEB"));
		domap(EID_VWR_LANG_EN, 3, TEXT("MAR"));
		domap(EID_VWR_LANG_EN, 4, TEXT("APR"));
		domap(EID_VWR_LANG_EN, 5, TEXT("MAY"));
		domap(EID_VWR_LANG_EN, 6, TEXT("JUN"));
		domap(EID_VWR_LANG_EN, 7, TEXT("JUL"));
		domap(EID_VWR_LANG_EN, 8, TEXT("AUG"));
		domap(EID_VWR_LANG_EN, 9, TEXT("SEP"));
		domap(EID_VWR_LANG_EN, 10, TEXT("OCT"));
		domap(EID_VWR_LANG_EN, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_EN, 12, TEXT("DEC"));
	}
}

#undef domap
#define domap(l,n,s) tomonth[l][n] = s
DobWriter::DobWriter(DateParser * p):DateWriter(p)
{
	if (tomonth.size() == 0)
	{
		domap(EID_VWR_LANG_DE, 0, TEXT("   "));
		domap(EID_VWR_LANG_DE, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_DE, 2, TEXT("FEB"));
		domap(EID_VWR_LANG_DE, 3, TEXT("MÄR"));
		domap(EID_VWR_LANG_DE, 4, TEXT("APR"));
		domap(EID_VWR_LANG_DE, 5, TEXT("MAI"));
		domap(EID_VWR_LANG_DE, 6, TEXT("JUN"));
		domap(EID_VWR_LANG_DE, 7, TEXT("JUL"));
		domap(EID_VWR_LANG_DE, 8, TEXT("AUG"));
		domap(EID_VWR_LANG_DE, 9, TEXT("SEP"));
		domap(EID_VWR_LANG_DE, 10, TEXT("OKT"));
		domap(EID_VWR_LANG_DE, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_DE, 12, TEXT("DEZ"));

		domap(EID_VWR_LANG_FR, 0, TEXT("    "));
		domap(EID_VWR_LANG_FR, 1, TEXT("JAN "));
		domap(EID_VWR_LANG_FR, 2, TEXT("FEV "));
		domap(EID_VWR_LANG_FR, 3, TEXT("MARS"));
		domap(EID_VWR_LANG_FR, 4, TEXT("AVR "));
		domap(EID_VWR_LANG_FR, 5, TEXT("MAI "));
		domap(EID_VWR_LANG_FR, 6, TEXT("JUIN"));
		domap(EID_VWR_LANG_FR, 7, TEXT("JUIL"));
		domap(EID_VWR_LANG_FR, 8, TEXT("AOUT"));
		domap(EID_VWR_LANG_FR, 9, TEXT("SEPT"));
		domap(EID_VWR_LANG_FR, 10, TEXT("OCT "));
		domap(EID_VWR_LANG_FR, 11, TEXT("NOV "));
		domap(EID_VWR_LANG_FR, 12, TEXT("DEC "));

		domap(EID_VWR_LANG_NL, 0, TEXT("    "));
		domap(EID_VWR_LANG_NL, 1, TEXT("JAN "));
		domap(EID_VWR_LANG_NL, 2, TEXT("FEB "));
		domap(EID_VWR_LANG_NL, 3, TEXT("MAAR"));
		domap(EID_VWR_LANG_NL, 4, TEXT("APR "));
		domap(EID_VWR_LANG_NL, 5, TEXT("MEI "));
		domap(EID_VWR_LANG_NL, 6, TEXT("JUN "));
		domap(EID_VWR_LANG_NL, 7, TEXT("JUL "));
		domap(EID_VWR_LANG_NL, 8, TEXT("AUG "));
		domap(EID_VWR_LANG_NL, 9, TEXT("SEP "));
		domap(EID_VWR_LANG_NL, 10, TEXT("OKT "));
		domap(EID_VWR_LANG_NL, 11, TEXT("NOV "));
		domap(EID_VWR_LANG_NL, 12, TEXT("DEC "));

		domap(EID_VWR_LANG_EN, 0, TEXT("   "));
		domap(EID_VWR_LANG_EN, 1, TEXT("JAN"));
		domap(EID_VWR_LANG_EN, 2, TEXT("FEB"));
		domap(EID_VWR_LANG_EN, 3, TEXT("MAR"));
		domap(EID_VWR_LANG_EN, 4, TEXT("APR"));
		domap(EID_VWR_LANG_EN, 5, TEXT("MAY"));
		domap(EID_VWR_LANG_EN, 6, TEXT("JUN"));
		domap(EID_VWR_LANG_EN, 7, TEXT("JUL"));
		domap(EID_VWR_LANG_EN, 8, TEXT("AUG"));
		domap(EID_VWR_LANG_EN, 9, TEXT("SEP"));
		domap(EID_VWR_LANG_EN, 10, TEXT("OCT"));
		domap(EID_VWR_LANG_EN, 11, TEXT("NOV"));
		domap(EID_VWR_LANG_EN, 12, TEXT("DEC"));
	}
}

void DobParser::input(EID_STRING original)
{
	day = original.substr(0, 2);
	size_t spaceIdx = original.find_first_of(TEXT("."));
	if (std::string::npos == spaceIdx) {
		spaceIdx = original.find_first_of(TEXT(" "));
	}
	month = frommonth[original.substr(spaceIdx + 1, original.find_first_of(TEXT(" ."), spaceIdx + 1) - (spaceIdx + 1))];
	year = original.substr(original.find_last_of(TEXT(" .")) + 1);
}

EID_STRING DobWriter::output()
{
	EID_CHAR sep1 = target_ == EID_VWR_LANG_DE ? TEXT('.') : TEXT(' ');
	EID_CHAR sep2 = sep1;

	if(parser->day == TEXT("  ")) sep1 = TEXT(' ');
	if(parser->month == 0) sep2 = TEXT(' ');
	return parser->day + sep1 + tomonth[target_][parser->month] + sep2 +
		parser->year;
}
