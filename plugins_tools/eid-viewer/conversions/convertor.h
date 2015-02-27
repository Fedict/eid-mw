#ifndef EID_VWR_CONVERTOR_H
#define EID_VWR_CONVERTOR_H

#include <conversions.h>
#include <map>
#include <string>

class ConversionWorker;

class Convertor {
private:
	static std::map<std::string, ConversionWorker*> convertors;
public:
	Convertor();
	const char* convert(const char* label, const char* normal);
	int can_convert(const char* label);
};

#endif
