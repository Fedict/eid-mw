// beidread_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "getcarddata.h"
#include "getreadercount.h"

extern char* g_firstNames;
extern char* g_firstLetterThirdName;
extern char* g_surName;
extern char* g_address_Street_Number;
extern char* g_address_Zip;
extern char* g_address_Municipality;


int _tmain(int argc, _TCHAR* argv[])
{
	CK_RV retval;
	int readerCount = 0;
	retval = ReadTheCardData();

	printf("Firsname is %s\n",g_firstNames);
	printf("firstLetterThirdName is %s\n",g_firstLetterThirdName);
	printf("surName is %s\n",g_surName);
	printf("Street_Number is %s\n",g_address_Street_Number);
	printf("Zip is %s\n",g_address_Zip);
	printf("Municipality is %s\n",g_address_Municipality);

	retval = GetTheReaderCount(&readerCount,0);

	printf("readercount is %d\n",readerCount);

	return 0;
}

