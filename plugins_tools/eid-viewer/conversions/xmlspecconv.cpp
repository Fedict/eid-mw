#include "xmlspecconv.h"
#include "cppeidstring.h"

std::map < EID_STRING, EID_STRING > XmlSpecConvertor::conversions;

XmlSpecConvertor::XmlSpecConvertor()
{
	if (conversions.empty())
	{
		conversions[TEXT("NO_STATUS")] = TEXT("0");
		conversions[TEXT("WHITE_CANE")] = TEXT("1");
		conversions[TEXT("EXTENDED_MINORITY")] = TEXT("2");
		conversions[TEXT("WHITE_CANE_EXTENDED_MINORITY")] = TEXT("3");
		conversions[TEXT("YELLOW_CANE")] = TEXT("4");
		conversions[TEXT("YELLOW_CANE_EXTENDED_MINORITY")] = TEXT("5");
		conversions[TEXT("0")] = TEXT("NO_STATUS");
		conversions[TEXT("1")] = TEXT("WHITE_CANE");
		conversions[TEXT("2")] = TEXT("EXTENDED_MINORITY");
		conversions[TEXT("3")] = TEXT("WHITE_CANE_EXTENDED_MINORITY");
		conversions[TEXT("4")] = TEXT("YELLOW_CANE");
		conversions[TEXT("5")] = TEXT("YELLOW_CANE_EXTENDED_MINORITY");
	}
}

EID_STRING XmlSpecConvertor::convert(const void *original)
{
	if (conversions.count((const EID_CHAR *) original) > 0)
	{
		return conversions[(const EID_CHAR *) original];
	}
	return TEXT("");
}
