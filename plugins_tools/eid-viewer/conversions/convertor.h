#ifndef EID_VWR_CONVERTOR_H
#define EID_VWR_CONVERTOR_H

#include <conversions.h>
#include <eid-util/utftranslate.h>
#include "cppeidstring.h"
#include <map>
#include <string>

class ConversionWorker;

class Convertor
{
private:
	static std::map < EID_STRING, ConversionWorker * >convertors;
	static std::map < EID_STRING, ConversionWorker * >to_xml;
	static std::map < EID_STRING, ConversionWorker * >from_xml;
public:
	Convertor();
	EID_CHAR *convert(const EID_CHAR * label, const EID_CHAR * normal);
	EID_CHAR *convert_to_xml(const EID_CHAR * label,
				 const EID_CHAR * normal);
	void *convert_from_xml(const EID_CHAR * name, const EID_CHAR * value,
				int *len_return);
	int can_convert(const EID_CHAR * label);
};

#endif
