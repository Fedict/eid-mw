#include "xmlfileversconv.h"
#include "backend.h"

std::map<eid_vwr_langs, const EID_CHAR*> XmlFileversConvertor::msgs_;

XmlFileversConvertor::XmlFileversConvertor() {
	if(msgs_.empty()) {
		msgs_[EID_VWR_LANG_DE] = TEXT("The opened file was created with a more recent eID version of the eID Viewer. Some data may not be displayed");
		msgs_[EID_VWR_LANG_EN] = TEXT("The opened file was created with a more recent eID version of the eID Viewer. Some data may not be displayed");
		msgs_[EID_VWR_LANG_FR] = TEXT("The opened file was created with a more recent eID version of the eID Viewer. Some data may not be displayed");
		msgs_[EID_VWR_LANG_NL] = TEXT("Het geopende bestand werd aangemaakt met een recentere versie van de eID Viewer. Bepaalde data wordt mogelijk niet weergegeven.");
	}
}

EID_STRING XmlFileversConvertor::convert(const void *original) {
	EID_STRING orig((const EID_CHAR*)original);

	if(orig.compare(TEXT("4.1")) != 0) {
		be_log(EID_VWR_LOG_ERROR, msgs_[target_]);
	}
	return orig;
}
