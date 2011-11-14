// beidread_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "getcarddata.h"

extern char* g_firstNames;

int _tmain(int argc, _TCHAR* argv[])
{
	CK_RV retval;
	retval = ReadTheCardData();

	printf("Firsname is %s\n",g_firstNames);
	return 0;
}

