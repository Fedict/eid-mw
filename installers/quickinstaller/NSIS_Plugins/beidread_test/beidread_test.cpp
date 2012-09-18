// beidread_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "getcarddata.h"
#include "getreadercount.h"

extern char* g_firstNames;

int _tmain(int argc, _TCHAR* argv[])
{
	CK_RV retval;
	int readerCount = 0;
	retval = ReadTheCardData();

	printf("Firsname is %s\n",g_firstNames);

	retval = GetTheReaderCount(&readerCount,0);

	printf("readercount is %d\n",readerCount);

	return 0;
}

