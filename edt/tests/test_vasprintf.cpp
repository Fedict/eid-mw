#include <stdio.h>
#include "secure_helper.h"

int main(int argc, char* argv[])
{
	fprintf(STDERR,"%d written : [%s]\n",fwprintf(STDOUT,"hello %s world\n","beautiful"));
}

