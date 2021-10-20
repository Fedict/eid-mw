#ifndef EID_VWR_CONV_WORKER_H
#define EID_VWR_CONV_WORKER_H

#include <string>
#include <conversions.h>
#include <cstring>
#include <eid-util/utftranslate.h>
#include "cppeidstring.h"

class ConversionWorker
{
protected:
	static eid_vwr_langs target_;
	static eid_vwr_graphvers graphvers_; //the graphical version number
public:
	static void set_lang(eid_vwr_langs which)
	{
		target_ = which;
	};
	static eid_vwr_langs get_lang()
	{
		return target_;
	};
	static bool have_language()
	{
		return target_ > EID_VWR_LANG_NONE;
	};

	static void set_graphvers(eid_vwr_graphvers graphvers)
	{
		graphvers_ = graphvers;
	};
	/*static eid_vwr_graphvers get_graphvers()
	{
		return graphvers_;
	};*/
	virtual EID_STRING convert(const void *original) = 0;
	virtual EID_STRING convert(std::string original)
	{
		return convert(original.c_str());
	};
	virtual void *convert(const void *original, int *len_return)
	{
		EID_STRING cv = convert(original);

		*len_return = (int) cv.length();
		return EID_STRDUP(cv.c_str());
	}
};

#endif
