#ifndef EID_VWR_XML_FILEVERS_CONV_H
#define EID_VWR_XML_FILEVERS_CONV_H

#include "convworker.h"
#include <map>

class XmlFileversConvertor:public ConversionWorker
{
private:
	static std::map<eid_vwr_langs, const EID_CHAR*> msgs_;
public:
	XmlFileversConvertor();
	virtual EID_STRING convert(const void *original);
};

#endif
