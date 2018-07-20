#include "toxmlmemberoffamilyconv.h"
#include <iostream>
#include <sstream>
#include <iomanip>

//no value of this field is stored on the card, so if present, return "1" for true
EID_STRING ToXmlMemberOfFamilyConvertor::convert(const void *normal)
{
	return(TEXT("1"));
}
