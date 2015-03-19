#include "convertor.h"
#include "natnumconv.h"
#include "hexnumconv.h"
#include "specconv.h"
#include "doctypeconv.h"
#include "dateconv.h"
#include "bbannumconv.h"

std::map<std::string, ConversionWorker*> Convertor::convertors;

Convertor::Convertor() {
	if(convertors.empty()) {
		convertors["national_number"] = new NationalNumberConvertor();
		convertors["chip_number"] = new HexNumberConvertor(16);
		convertors["special_status"] = new SpecConvertor();
		convertors["document_type"] = new DocTypeConvertor();
		convertors["date_of_birth"] = new DateConvertor();
		convertors["card_number"] = new BBANNumberConvertor();
	}
}

const char* Convertor::convert(const char* label, const char* normal) {
	if(can_convert(label)) {
		return convertors[label]->convert(normal).c_str();
	} else {
		return normal;
	}
}

int Convertor::can_convert(const char* label) {
	return ConversionWorker::have_language() && convertors.count(label);
}
