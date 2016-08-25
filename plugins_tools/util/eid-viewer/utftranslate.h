#ifndef UTFTRANSLATE_H
#define UTFTRANSLATE_H

#ifndef WIN32
#include <string.h>

static inline char *f(const char *c, unsigned long *l)
{
	*l = strlen(c);
	return strdup(c);
}

#define EID_STRCMP(x,y) strcmp(x,y)
#define EID_STRTOL(x,y,z) strtol(x,y,z)
#define EID_STRDUP(x) strdup(x)
#define EID_STRLEN(x) strlen(x)
#define EID_VSNPRINTF(str, size, string, ap) vsnprintf(str, size, string, ap)
#define EID_FOPEN(filename, mode) fopen(filename, mode)
typedef char EID_CHAR;

#define UTF8TOEID(utf8string, len) f(utf8string, len)
#define UTF8TOEID_L(utf8string, len, loc) strncpy((loc), (utf8string), *(len))
#define EIDTOUTF8(eidstring, len) f(eidstring, len)
#define EIDTOUTF8_L(eidstring, len, loc) strncpy((loc), (eidstring), *(len))
#ifndef TEXT
#define TEXT(string) string
#endif
#else
#include <stdio.h>
#include <Windows.h>

//caller is responsible for freeing the returned string
wchar_t *Utf8ToUtf16(const char *utf8string, unsigned long *utf16len,
		     wchar_t * buf);
//caller is responsible for freeing the returned string
char *Utf16ToUtf8(const wchar_t * utf16string, unsigned long *utf8len,
		  char *buf);


#define EID_STRCMP(x,y) wcscmp(x,y)
#define EID_STRTOL(x,y,z) wcstol(x,y,z)
#define EID_STRDUP(x) _wcsdup(x)
#define EID_STRLEN(x) wcslen(x)
#define EID_VSNPRINTF(str, size, string, ap) _vsnwprintf(str, size, string, ap)
#define EID_FOPEN(filename, mode) _wfopen(filename, mode)
typedef wchar_t EID_CHAR;

#define UTF8TOEID(utf8string, len) Utf8ToUtf16(utf8string, len, NULL)
#define UTF8TOEID_L(utf8string, len, loc) Utf8ToUtf16(utf8string, len, loc)
#define EIDTOUTF8(eidstring, len) Utf16ToUtf8(eidstring, len, NULL)
#define EIDTOUTF8_L(eidstring, len, loc) Utf16ToUtf8(eidstring, len, loc)

#endif //no WIN32

#define EID_SAFE_FREE(pointer) if(pointer != NULL) {free(pointer); pointer=NULL;}

#endif
