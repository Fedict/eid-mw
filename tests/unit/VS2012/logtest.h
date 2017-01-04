
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#ifndef logtest_h
#define logtest_h
	
#include <stdio.h>
	

/****************************************************************************
 * Logging definitions
 ***************************************************************************/ 
typedef enum pkcs11test_log_level 
{ LVL_DEBUG = 0, LVL_INFO, LVL_WARNING, LVL_ERROR, LVL_NOLEVEL 
} PKCS11TEST_LOG_LEVEL;
 static PKCS11TEST_LOG_LEVEL g_logLevel = LVL_DEBUG;

 

/****************************************************************************
 * Logging macro's
 ***************************************************************************/ 
#define pkcs11test_filename "test_pkcs11.log"

/****************************************************************************
 * macro's for logging to file
 ***************************************************************************/ 
//#define testlog(LOGLEVEL,...) logPrefix(LOGLEVEL), fprintf(pkcs11test_logfile, __VA_ARGS__)
//#define printlogprefic(LOGLEVEL,LOGPREFIX) fprintf(LOGLEVEL,LOGPREFIX)
	

/****************************************************************************
 * macro's for logging to stdout
 ***************************************************************************/ 
#define testlog(LOGLEVEL,...) if (LOGLEVEL >= g_logLevel) {logPrefix(LOGLEVEL); printf(__VA_ARGS__);}
#define printlogprefic(LOGLEVEL,LOGPREFIX) printf(LOGPREFIX)
	

/****************************************************************************
 * Log Function global variables
 ***************************************************************************/ 
	FILE * pkcs11test_logfile;
 

/****************************************************************************
 * Log Function prototypes
 ***************************************************************************/ 
	errno_t initLog();
void endLog();
 void logPrefix(PKCS11TEST_LOG_LEVEL level);
void testlogbytes(PKCS11TEST_LOG_LEVEL loglevel, unsigned char *pValue,
		   unsigned long ulValueLen);
 
#endif /*  */
