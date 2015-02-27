#ifndef EID_VWR_CONV_WORKER_H
#define EID_VWR_CONV_WORKER_H

#include <string>
#include <conversions.h>

class ConversionWorker {
protected:
	static eid_vwr_langs target_;
public:
	static void set_lang(eid_vwr_langs which) { target_ = which; };
	static bool have_language() { return target_ > EID_VWR_LANG_NONE; };
	virtual std::string convert(const char* original) = 0;
	virtual std::string convert(std::string original) { convert(original.c_str()); };
};

#endif
