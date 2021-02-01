#ifdef __cplusplus
#ifndef CPPEIDSTRING_H
#define CPPEIDSTRING_H
#include <stdio.h>
#include <string>
#include <eid-util/utftranslate.h>

#ifndef WIN32
typedef std::string EID_STRING;

#define EID_OSTRINGSTREAM std::ostringstream
#define EID_STRINGSTREAM std::stringstream
#else
typedef std::wstring EID_STRING;

#define EID_OSTRINGSTREAM std::wostringstream
#define EID_STRINGSTREAM std::wstringstream
#endif
#endif
#endif
