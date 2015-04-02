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
#include "hexdecode.h"

#include <map>
#include <string>
#include <cstring>

std::map<std::string, ConversionWorker*> Convertor::convertors;
std::map<std::string, ConversionWorker*> Convertor::to_xml;
std::map<std::string, ConversionWorker*> Convertor::from_xml;

Convertor::Convertor() {
	if(convertors.empty()) {
		convertors["national_number"] = new NationalNumberConvertor();
		convertors["chip_number"] = new HexNumberConvertor(16);
		convertors["special_status"] = new SpecConvertor();
		convertors["document_type"] = new DocTypeConvertor();
		convertors["date_of_birth"] = new DobWriter(new DobParser);
		convertors["card_number"] = new BBANNumberConvertor();
	}
	if(to_xml.empty()) {
		to_xml["document_type"] = new XmlDoctypeConvertor();
		to_xml["special_status"] = new XmlSpecConvertor();
		to_xml["chip_number"] = new HexNumberConvertor(16);
		to_xml["date_of_birth"] = new XmlDateWriter(new DobParser);
	}
	if(from_xml.empty()) {
		from_xml["document_type"] = new XmlDoctypeConvertor();
		from_xml["special_status"] = new XmlSpecConvertor();
		from_xml["chip_number"] = new HexDecodeConvertor(16);
		from_xml["date_of_birth"] = new DobWriter(new XmlDateParser);
	}
}

const char* Convertor::convert(const char* label, const char* normal) {
	if(can_convert(label)) {
		return convertors[label]->convert(normal).c_str();
	} else {
		return normal;
	}
}

void* Convertor::convert_from_xml(const char* name, const char* value, int* len_return) {
	if(!value) {
		*len_return = 0;
		return strdup("");
	}
	if(from_xml.count(name) > 0) {
		return from_xml[name]->convert(value, len_return);
	}
	*len_return = strlen(value);
	return strdup(value);
}

const char* Convertor::convert_to_xml(const char* label, const char* normal) {
	if(to_xml.count(label) > 0) {
		return to_xml[label]->convert(normal).c_str();
	}
	return normal;
}

int Convertor::can_convert(const char* label) {
	return ConversionWorker::have_language() && convertors.count(label);
}
