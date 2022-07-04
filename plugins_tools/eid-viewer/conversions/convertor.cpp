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
#include "specorgconv.h"
#include "xmlspecorgconv.h"
#include "workpermitconv.h"
#include "xmlfileversconv.h"
#include "toxmlmemberoffamilyconv.h"
#include "booldecode.h"
#include <eid-util/utftranslate.h>
#include "cppeidstring.h"
#include "brexitment1conv.h"
#include "brexitment2conv.h"
#include "cardAment1conv.h"
#include "cardAment2conv.h"

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
		convertors[TEXT("national_number")] = new NationalNumberConvertor();
		convertors[TEXT("chip_number")] = new HexNumberConvertor(16);
		convertors[TEXT("special_status")] = new SpecConvertor();
		convertors[TEXT("document_type")] = new DocTypeConvertor();
		convertors[TEXT("date_of_birth")] = new DobWriter(new DobParser);
		convertors[TEXT("card_number")] = new BBANNumberConvertor();
		convertors[TEXT("gender")] = new GenderConvertor();
		convertors[TEXT("special_organisation")] = new SpecOrgConvertor();
		convertors[TEXT("work_permit_mention")] = new WorkPermitConvertor();
		convertors[TEXT("brexit_mention_1")] = new BrexitMention1Convertor();
		convertors[TEXT("brexit_mention_2")] = new BrexitMention2Convertor();
		convertors[TEXT("cardA_mention_1")] = new CardAMention1Convertor();
		convertors[TEXT("cardA_mention_2")] = new CardAMention2Convertor();
	}
	if (to_xml.empty())
	{
		to_xml[TEXT("document_type")] = new XmlDoctypeConvertor();
		to_xml[TEXT("special_status")] = new XmlSpecConvertor();
		to_xml[TEXT("carddata_appl_version")] = new HexNumberConvertor(1);
		to_xml[TEXT("chip_number")] = new HexNumberConvertor(16);
		to_xml[TEXT("date_of_birth")] = new XmlDateWriter(new DobParser);
		to_xml[TEXT("validity_begin_date")] = new XmlDateWriter(new ValidityDateParser);
		to_xml[TEXT("validity_end_date")] = new XmlDateWriter(new ValidityDateParser);
		to_xml[TEXT("gender")] = new XmlGenderConvertor();
		to_xml[TEXT("special_organisation")] = new XmlSpecOrgConvertor();
		to_xml[TEXT("work_permit_mention")] = new XmlWorkPermitConvertor();
		to_xml[TEXT("member_of_family")] = new ToXmlMemberOfFamilyConvertor();
		to_xml[TEXT("cardA_mention_1")] = new XmlCardAMention1Convertor();
		to_xml[TEXT("cardA_mention_2")] = new XmlCardAMention2Convertor();
		to_xml[TEXT("tokeninfo_graph_perso_version")] = new HexNumberConvertor(1);
		to_xml[TEXT("cardEU_start_date")] = new XmlDateWriter(new ValidityDateParser);
	}
	if (from_xml.empty())
	{
		from_xml[TEXT("document_type")] = new XmlDoctypeConvertor();
		from_xml[TEXT("special_status")] = new XmlSpecConvertor();
		from_xml[TEXT("carddata_appl_version")] = new HexDecodeConvertor(1);
		from_xml[TEXT("chip_number")] = new HexDecodeConvertor(16);
		from_xml[TEXT("date_of_birth")] = new DobWriter(new XmlDateParser);
		from_xml[TEXT("validity_begin_date")] = new ValidityDateWriter(new XmlDateParser);
		from_xml[TEXT("validity_end_date")] = new ValidityDateWriter(new XmlDateParser);
		from_xml[TEXT("gender")] = new XmlGenderConvertor();
		from_xml[TEXT("special_organisation")] = new XmlSpecOrgConvertor();
		from_xml[TEXT("work_permit_mention")] = new XmlWorkPermitConvertor();
		from_xml[TEXT("xml_file_version")] = new XmlFileversConvertor(); // hack to produce an error message when file is opened that was created by a more recent version of eID Viewer
		from_xml[TEXT("member_of_family")] = new BoolDecodeConvertor();
		from_xml[TEXT("cardA_mention_1")] = new XmlCardAMention1Convertor();
		from_xml[TEXT("cardA_mention_2")] = new XmlCardAMention2Convertor();
		from_xml[TEXT("tokeninfo_graph_perso_version")] = new HexDecodeConvertor(1);
		from_xml[TEXT("cardEU_start_date")] = new ValidityDateWriter(new XmlDateParser);
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
