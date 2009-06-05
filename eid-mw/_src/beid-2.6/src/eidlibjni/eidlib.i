/* File : eidlib.i */
%module eidlib

%{
#include <eidlib.h>
%}

%include cpointer.i
%pointer_functions(long, longp);

/* Let's just grab the original header file here */
/*%include "C:/programming/Include/eidlib.h"*/

