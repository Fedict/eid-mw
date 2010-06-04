/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
/****************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "globmdrv.h"
#include "log.h"

/****************************************************************************************************/

char           g_szLogFile[512]  = "C:\\SmartCardMinidriverTest\\AZEBEIDMDRV.LOG";

#ifdef _DEBUG
unsigned int   g_uiLogLevel      = LOGTYPE_TRACE;
#else
unsigned int   g_uiLogLevel      = LOGTYPE_NONE;
#endif

/****************************************************************************************************/

void LogInit (char *pszLogFile, unsigned int uiLogLevel)
{
   if ( pszLogFile != NULL )
   {
      // MS only brrrrr
	  //strncpy_s(g_szLogFile, sizeof(g_szLogFile), pszLogFile, _TRUNCATE);
      strncpy(pszLogFile,g_szLogFile,sizeof(g_szLogFile));
   }

   g_uiLogLevel = uiLogLevel;
}

/****************************************************************************************************/

void LogTrace(int info, const char *pWhere, const char *format,... )
{
   char           buffer[2048];

   time_t         timer;
   struct tm      *t;
   char           timebuf  [26];
   unsigned int   uiYear;

   va_list        listArg;
   int            iLog = 0;

   FILE           *fp = NULL;

   switch (g_uiLogLevel)
   {
   case LOGTYPE_ERROR:
      if ( info == LOGTYPE_ERROR )
      {
         iLog++;
      }
      break;

   case LOGTYPE_WARNING:
      if ( info == LOGTYPE_WARNING )
      {
         iLog++;
      }
      break;

   case LOGTYPE_INFO:
      if ( info == LOGTYPE_INFO )
      {
         iLog++;
      }
      break;

   case LOGTYPE_TRACE:
      iLog++;
      break;

   default:
      /* No Logging */
      break;
   }

   if ( iLog == 0 )
   {
      return;
   }

   if ( pWhere == NULL )
   {
      return;
   }

   /* Gets time of day */
   timer = time(NULL);

   /* Converts date/time to a structure */
   memset(timebuf, '\0', sizeof(timebuf));
   t = localtime(&timer);
   if (t != NULL)
   {
	   uiYear = t->tm_year;

	   /* Add century to year */
	   uiYear += 1900;

	   /* Converts date/time to string */
	   _snprintf(timebuf, sizeof(timebuf)
                       , "%02d/%02d/%04d - %02d:%02d:%02d"
                       , t->tm_mday
                       , t->tm_mon + 1
                       , uiYear
                       , t->tm_hour
                       , t->tm_min
                       , t->tm_sec);
   }

   memset (buffer, '\0', sizeof(buffer));
   va_start(listArg, format);
   _vsnprintf(buffer, sizeof(buffer), format, listArg);
   va_end(listArg);

   fp = fopen(g_szLogFile, "a");
   if ( fp != NULL )
   {
      fprintf (fp, "%s|%30s|%s\n", timebuf, pWhere, buffer);
      fclose(fp);
   }
}

/****************************************************************************************************/

#define TT_HEXDUMP_LZ      16

void LogDump (int iStreamLg, unsigned char *pa_cStream)
{
   FILE           *fp = NULL;

   int            i        = 0;
   int            iOffset  = 0;
   unsigned char  *p       = pa_cStream;

   if ( pa_cStream == NULL )
   {
      return;
   }

   fp = fopen(g_szLogFile, "a");
   if ( fp == NULL )
   {
      return;
   }

   for ( i = 0 ; ((i < iStreamLg) && (p != NULL)) ; i++ )
   {
      if ( ( i % TT_HEXDUMP_LZ ) == 0 )
      {
         fprintf (fp, "\n");
         fprintf (fp, "%08X: ", i);
      }

      fprintf (fp, "%02X ", *p++);
   }
   fprintf (fp, "\n\n");

   fclose(fp);
}

/****************************************************************************************************/

void LogDumpBin (char *pa_cName, int iStreamLg, unsigned char *pa_cStream)
{
   FILE           *fp = NULL;

   if ( ( pa_cName   == NULL ) ||
        ( pa_cStream == NULL ) )
   {
      return;
   }

   fp = fopen(pa_cName, "wb");
   if ( fp != NULL )
   {
      fwrite(pa_cStream, sizeof(char), iStreamLg, fp);
      fclose(fp);
   }
}

/****************************************************************************************************/
