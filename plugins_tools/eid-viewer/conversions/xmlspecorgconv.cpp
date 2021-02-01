#include "xmlspecorgconv.h"
#include <eid-util/utftranslate.h>

#include <sstream>

std::map < EID_STRING, EID_STRING > XmlSpecOrgConvertor::conversions;

XmlSpecOrgConvertor::XmlSpecOrgConvertor()
{
	if (conversions.empty())
	{
		conversions[TEXT("1")] = TEXT("SHAPE");
		conversions[TEXT("2")] = TEXT("NATO");
		conversions[TEXT("4")] = TEXT("old-carte-bleue-euro");
		conversions[TEXT("5")] = TEXT("researcher");
		conversions[TEXT("SHAPE")] = TEXT("1");
		conversions[TEXT("NATO")] = TEXT("2");
		conversions[TEXT("old-carte-bleue-euro")] = TEXT("4");
		conversions[TEXT("researcher")] = TEXT("5");
	}
}

EID_STRING XmlSpecOrgConvertor::convert(const void *normal)
{
	EID_STRING from = (EID_CHAR*)normal;
	if (conversions.count(from) > 0)
	{
		return conversions[from];
	}
	return from;
}
