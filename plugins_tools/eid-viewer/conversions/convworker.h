#ifndef EID_VWR_CONV_WORKER_H
#define EID_VWR_CONV_WORKER_H

#include <string>
#include <conversions.h>
#include <cstring>

class ConversionWorker {
protected:
	static eid_vwr_langs target_;
public:
	static void set_lang(eid_vwr_langs which) { target_ = which; };
	static eid_vwr_langs get_lang() { return target_; };
	static bool have_language() { return target_ > EID_VWR_LANG_NONE; };
	virtual std::string convert(const char* original) = 0;
	virtual std::string convert(std::string original) { return convert(original.c_str()); };
	virtual void* convert(const char* original, int* len_return) {
		std::string cv = convert(original);
		*len_return = cv.length();
		return strdup(cv.c_str());
	}
};

#endif
