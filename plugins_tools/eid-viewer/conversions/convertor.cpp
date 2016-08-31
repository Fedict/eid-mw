#include "convertor.h"
#include "natnumconv.h"
#include "hexnumconv.h"
#include "specconv.h"
#include "doctypeconv.h"
#include "dateconv.h"
#include "bbannumconv.h"
#include "cache.h"
#include "xmldoctypeconv.h"
#include "xmlspecconv.h"
#include "dob.h"
#include "valdate.h"
#include "hexdecode.h"
#include "genderconv.h"
#include <eid-util/utftranslate.h>
#include "cppeidstring.h"

#include <map>
#include <string>
#include <cstring>



std::map < EID_STRING, ConversionWorker * >Convertor::convertors;
std::map < EID_STRING, ConversionWorker * >Convertor::to_xml;
std::map < EID_STRING, ConversionWorker * >Convertor::from_xml;

/* Top-level class for conversions. */
Convertor::Convertor()
{
	if (convertors.empty())
	{
		convertors[TEXT("national_number")] =
			new NationalNumberConvertor();
		convertors[TEXT("chip_number")] = new HexNumberConvertor(16);
		convertors[TEXT("special_status")] = new SpecConvertor();
		convertors[TEXT("document_type")] = new DocTypeConvertor();
		convertors[TEXT("date_of_birth")] =
			new DobWriter(new DobParser);
		convertors[TEXT("card_number")] = new BBANNumberConvertor();
		convertors[TEXT("gender")] = new GenderConvertor();
	}
	if (to_xml.empty())
	{
		to_xml[TEXT("document_type")] = new XmlDoctypeConvertor();
		to_xml[TEXT("special_status")] = new XmlSpecConvertor();
		to_xml[TEXT("chip_number")] = new HexNumberConvertor(16);
		to_xml[TEXT("date_of_birth")] =
			new XmlDateWriter(new DobParser);
		to_xml[TEXT("validity_begin_date")] =
			new XmlDateWriter(new ValidityDateParser);
		to_xml[TEXT("validity_end_date")] =
			new XmlDateWriter(new ValidityDateParser);
		to_xml[TEXT("gender")] = new XmlGenderConvertor();
	}
	if (from_xml.empty())
	{
		from_xml[TEXT("document_type")] = new XmlDoctypeConvertor();
		from_xml[TEXT("special_status")] = new XmlSpecConvertor();
		from_xml[TEXT("chip_number")] = new HexDecodeConvertor(16);
		from_xml[TEXT("date_of_birth")] =
			new DobWriter(new XmlDateParser);
		from_xml[TEXT("validity_begin_date")] =
			new ValidityDateWriter(new XmlDateParser);
		from_xml[TEXT("validity_end_date")] =
			new ValidityDateWriter(new XmlDateParser);
		from_xml[TEXT("gender")] = new XmlGenderConvertor();
	}
}

EID_CHAR *Convertor::convert(const EID_CHAR * label, const EID_CHAR * normal)
{
	if (can_convert(label))
	{
		return EID_STRDUP(convertors[label]->convert(normal).c_str());
	} else
	{
		return EID_STRDUP(normal);
	}
}

void *Convertor::convert_from_xml(const EID_CHAR * name,
				  const EID_CHAR * value, int *len_return)
{
	if (!value)
	{
		*len_return = 0;
		return EID_STRDUP(TEXT(""));
	}
	if (from_xml.count(name) > 0)
	{
		return from_xml[name]->convert(value, len_return);
	}
	*len_return = (int) EID_STRLEN(value);
	return EID_STRDUP(value);
}

EID_CHAR *Convertor::convert_to_xml(const EID_CHAR * label,
				    const EID_CHAR * normal)
{
	if (to_xml.count(label) > 0)
	{
		return EID_STRDUP(to_xml[label]->convert(normal).c_str());
	}
	return EID_STRDUP(normal);
}

int Convertor::can_convert(const EID_CHAR * label)
{
	return ConversionWorker::have_language() && convertors.count(label);
}
