#include "genderconv.h"

std::string GenderConvertor::convert(const char* original) {
	switch(original[0]) {
		case 'M':
			return original;
		case 'V':
		case 'F':
		case 'W':
			switch(target_) {
				case EID_VWR_LANG_NL:
					return "V";
				case EID_VWR_LANG_FR:
				case EID_VWR_LANG_DE:
					return "F";
				case EID_VWR_LANG_EN:
					return "W";
				default:
					return original;
			}
		default:
			return original;
	}
}

std::string XmlGenderConvertor::convert(const char* original) {
	switch(original[0]) {
		case 'M':
			return "male";
		case 'F':
		case 'V':
		case 'W':
			return "female";
		case 'm':
			return GenderConvertor::convert("M");
		case 'f':
			return GenderConvertor::convert("F");
	}
}
