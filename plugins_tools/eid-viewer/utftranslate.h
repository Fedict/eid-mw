#ifndef UTFTRANSLATE_H
#define UTFTRANSLATE_H

#ifndef WIN32
#define EID_STRCMP(x,y) strcmp(x,y)
#define EID_STRTOL(x,y,z) strtol(x,y,z)
#define EID_STRDUP(x) strdup(x)
#define EID_STRLEN(x) strlen(x)
#define EID_VSNPRINTF(str, size, string, ap) vsnprintf(str, size, string, ap)
typedef char EID_CHAR;
#define UTF8TOEID(utf8string, len) utf8string
#define EIDTOUTF8(eidstring, len) utf8string
  #ifndef TEXT(string)
    #define TEXT(string) string
  #endif
#else
#include <stdio.h>
#include <Windows.h>

//caller is responsible for freeing the returned string
wchar_t* Utf8ToUtf16(const char* utf8string, unsigned long* utf16len);
//caller is responsible for freeing the returned string
char* Utf16ToUtf8(const wchar_t * utf16string, unsigned long* utf8len);


#define EID_STRCMP(x,y) wcscmp(x,y)
#define EID_STRTOL(x,y,z) wcstol(x,y,z)
#define EID_STRDUP(x) _wcsdup(x)
#define EID_STRLEN(x) wcslen(x)
#define EID_VSNPRINTF(str, size, string, ap) _vsnwprintf(str, size, string, ap)
typedef wchar_t EID_CHAR;
#define UTF8TOEID(utf8string, len) Utf8ToUtf16(utf8string, len)
#define EIDTOUTF8(eidstring, len) Utf16ToUtf8(eidstring, len)


#endif //no WIN32

#define EID_SAFE_FREE(pointer) if(pointer != NULL) {free(pointer); pointer=NULL;}

#endif