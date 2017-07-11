#include "genderconv.h"
#include "cppeidstring.h"


EID_STRING GenderConvertor::convert(const void *original)
{
	const EID_CHAR *eid_original = (const EID_CHAR *) original;

	switch (eid_original[0])
	{
		case TEXT('M'):
			return eid_original;
		case 'V':
		case 'F':
		case 'W':
			switch (target_)
			{
				case EID_VWR_LANG_NL:
					return TEXT("V");
				case EID_VWR_LANG_FR:
				case EID_VWR_LANG_EN:
					return TEXT("F");			
				case EID_VWR_LANG_DE:
					return TEXT("W");
				default:
					return eid_original;
			}
		default:
			return eid_original;
	}
}

EID_STRING XmlGenderConvertor::convert(const void *original)
{
	const EID_CHAR *eid_original = (const EID_CHAR *) original;

	switch (eid_original[0])
	{
		case 'M':
			return TEXT("male");
		case 'F':
		case 'V':
		case 'W':
			return TEXT("female");
			//bidirectional translation, we only check the first char
		case 'm':
			return GenderConvertor::convert("M");
		case 'f':
			return GenderConvertor::convert("F");
	}

	return TEXT("unknown");
}
