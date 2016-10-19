#include <stdio.h>
#include <Windows.h>

#include <eid-util/utftranslate.h>

//caller is responsible for freeing the returned string,
//utf16len is the length of the returned string in bytes, excluding the terminating null
wchar_t* Utf8ToUtf16(const char* utf8string, unsigned long* utf16len, wchar_t *buf)
{
	int wcharcount = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, NULL, 0);
	unsigned int reqlen = sizeof(wchar_t) * (wcharcount);
	wchar_t *utf16string = (buf) ? buf : (wchar_t*)malloc(sizeof(wchar_t) * (wcharcount));
	if(reqlen > *utf16len && buf != NULL) {
		return NULL;
	}
	if (utf16string != NULL)
	{
		MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, utf16string, wcharcount);
		*utf16len = sizeof(wchar_t) * (wcharcount - 1);
	}

	return utf16string;
}

//caller is responsible for freeing the returned string
//utf8len is the length of the returned string in bytes, excluding the terminating null
char* Utf16ToUtf8(const wchar_t * utf16string, unsigned long* utf8len, char* buf)
{
	unsigned int utf8bytesize = WideCharToMultiByte(CP_UTF8, 0, utf16string, -1, NULL, 0, NULL, NULL);
	char *utf8string = (buf) ? buf : (char*) malloc(utf8bytesize);
	if(utf8bytesize > *utf8len && buf != NULL) {
		return NULL;
	}
	if (utf8string != NULL)
	{
		WideCharToMultiByte(CP_UTF8, 0, utf16string, -1, utf8string, utf8bytesize, NULL, NULL);
		*utf8len = utf8bytesize - 1;
	}

	return utf8string;
}
