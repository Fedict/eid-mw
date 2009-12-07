/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

#include "GlobMdrv.h"

#include "Log.h"
#include "smartcard.h"

/****************************************************************************************************/

#define CHALLENGE_DATA_SIZE         16

#define BELPIC_MAX_FILE_SIZE        65535
#define BELPIC_PIN_BUF_SIZE         8
#define BELPIC_MIN_USER_PIN_LEN     4
#define BELPIC_MAX_USER_PIN_LEN     12
#define BELPIC_PAD_CHAR			      0xFF
#define BELPIC_KEY_REF_NONREP		   0x83

/****************************************************************************************************/

#define WHERE "BeidAuthenticate"
DWORD BeidAuthenticate(PCARD_DATA   pCardData, 
                       PBYTE        pbPin, 
                       DWORD        cbPin, 
                       PDWORD       pcAttemptsRemaining) 
{
   DWORD             dwReturn  = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg   = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen   = sizeof(recvbuf);
   BYTE              SW1, SW2;
   int               i         = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbPin == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbPin]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /* Don't allow zero-length PIN */
   if ( ( cbPin < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbPin > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbPin]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }

   /**********/
   /* Log On */
   /**********/
   Cmd [0] = 0x00;
   Cmd [1] = 0x20; /* VERIFY COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x01;
   Cmd [4] = 0x08;

   /* Fill verification data with padding character */
   for ( i = 0 ; i < 0x08 ; i++ )
   {
      Cmd [5 + i] = BELPIC_PAD_CHAR;
   }

   Cmd [5] = 0x20 + (unsigned char)cbPin;  /* 0x20 + length of pin */
   for ( i = 0 ; i < (unsigned char) cbPin ; i++ )
   {
      if ( (i % 2) == 0 )
      {
         Cmd [6 + (i/2)] = (((pbPin[i] - 48) << 4) & 0xF0);
      }
      else
      {
         Cmd [6 + (i/2)] = (Cmd[6 + (i/2)] & 0xF0) + ((pbPin[i] - 48) & 0x0F);
      }
   }
   uiCmdLg = 13;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      dwReturn = SCARD_W_WRONG_CHV;
      LogTrace(LOGTYPE_ERROR, WHERE, "CardAuthenticatePin Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( ((SW1 == 0x63) && ((SW2 & 0xF0) == 0xC0)) )
      {
         if ( pcAttemptsRemaining != NULL )
         {
            /* -1: Don't support returning the count of remaining authentication attempts */
            *pcAttemptsRemaining = (SW2 & 0x0F);
         }
      }
      else if ( (SW1 == 0x69) && (SW2 == 0x83) )
      {
         dwReturn = SCARD_W_CHV_BLOCKED;
      }
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Logged on...");
   }

cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidDeAuthenticate"
DWORD BeidDeAuthenticate(PCARD_DATA    pCardData) 
{
   DWORD             dwReturn  = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg   = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen   = sizeof(recvbuf);
   BYTE              SW1, SW2;
   int               i         = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /***********/
   /* Log Off */
   /***********/
   Cmd [0] = 0x80;
   Cmd [1] = 0xE6; /* LOG OFF */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   uiCmdLg = 4;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( (SW1 != 0x90) || (SW2 != 0x00) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit status bytes: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidMSE"
DWORD BeidMSE(PCARD_DATA   pCardData, 
              DWORD        dwRole) 
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   int               i = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /*
    * The MSE: SET Command will fail with error 0x000006f7
    * if the command is executed too fast after an command which resulted in an error condition
    */
   Sleep(20);

   /* Prepare SET COMMAND before a VERIFY PIN */
   Cmd [0] = 0x00;
   Cmd [1] = 0x22;   /* MSE: SET COMMAND */
   Cmd [2] = 0x41;
   Cmd [3] = 0xB6;
   Cmd [4] = 0x05;
   Cmd [5] = 0x04;   /* Length of following data      */
   Cmd [6] = 0x80;   /* ALGO Rreference               */
   Cmd [7] = 0x01;   /* RSA PKCS#1                    */
   Cmd [8] = 0x84;   /* TAG for private key reference */

   if ( dwRole == ROLE_DIGSIG )
   {
      Cmd [9] = 0x82;
   }
   else if ( dwRole == ROLE_NONREP )
   {
      Cmd [9] = 0x83;
   } 
   else 
   {
      LogTrace(LOGTYPE_INFO, WHERE, "SET COMMAND: undefined role [%d]", dwRole);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
   uiCmdLg = 10;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SET) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SET Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidChangePIN"
DWORD    BeidChangePIN
         (
            PCARD_DATA  pCardData, 
            PBYTE       pbCurrentAuthenticator,
            DWORD       cbCurrentAuthenticator,
            PBYTE       pbNewAuthenticator,
            DWORD       cbNewAuthenticator,
            PDWORD      pcAttemptsRemaining
         ) 
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   int               i        = 0;
   int               offset   = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbCurrentAuthenticator == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbCurrentAuthenticator]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbNewAuthenticator == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbNewAuthenticator]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( cbCurrentAuthenticator < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbCurrentAuthenticator > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbCurrentAuthenticator]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }
   if ( ( cbNewAuthenticator < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbNewAuthenticator > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbCurrentAuthenticator]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }

   /* Change PIN code */
   Cmd [0] = 0x00;
   Cmd [1] = 0x24;   /* CHANGE REFERENCE DATA COMMAND    */
   Cmd [2] = 0x00;   /* Support 'USER' password change   */
   Cmd [3] = 0x01;
   Cmd [4] = 0x10;

   /* Fill verification data with padding character */
   for ( i = 0 ; i < 0x10 ; i++ )
   {
      Cmd [5 + i] = BELPIC_PAD_CHAR;
   }

   Cmd [5] = 0x20 + (unsigned char)cbCurrentAuthenticator;  /* 0x20 + length of pin */
   for ( i = 0 ; i < (unsigned char) cbCurrentAuthenticator ; i++ )
   {
   	offset = 6 + (i/2);

      if ( (i % 2) == 0 )
      {
         Cmd [offset] = (((pbCurrentAuthenticator[i] - 48) << 4) & 0xF0);
      }
      else
      {
         Cmd [offset] = (Cmd[offset] & 0xF0) + ((pbCurrentAuthenticator[i] - 48) & 0x0F);
      }
   }
   Cmd [13] = 0x20 + (unsigned char)cbNewAuthenticator;  /* 0x20 + length of pin */
   for ( i = 0 ; i < (unsigned char) cbNewAuthenticator ; i++ )
   {
   	offset = 14 + (i/2);

      if ( (i % 2) == 0 )
      {
         Cmd [offset] = (((pbNewAuthenticator[i] - 48) << 4) & 0xF0);
      }
      else
      {
         Cmd [offset] = (Cmd[offset] & 0xF0) + ((pbNewAuthenticator[i] - 48) & 0x0F);
      }
   }

   uiCmdLg = 21;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      dwReturn = SCARD_W_WRONG_CHV;
      LogTrace(LOGTYPE_ERROR, WHERE, "CardChangeAuthenticator Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( ((SW1 == 0x63) && ((SW2 & 0xF0) == 0xC0)) )
      {
         if ( pcAttemptsRemaining != NULL )
         {
            /* -1: Don't support returning the count of remaining authentication attempts */
            *pcAttemptsRemaining = (SW2 & 0x0F);
         }
      }
      else if ( (SW1 == 0x69) && (SW2 == 0x83) )
      {
         dwReturn = SCARD_W_CHV_BLOCKED;
      }
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Changed PIN...");
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidGetCardSN"
DWORD BeidGetCardSN(PCARD_DATA  pCardData, unsigned int iSerNumLg, unsigned char *pa_cSerNum) 
{
   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[256];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   int                     i = 0;

   Cmd [0] = 0x80;
   Cmd [1] = 0xE4;
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x10;
   uiCmdLg = 5;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SIGN) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];

   if ( ( SW1 != 0x61 ) || ( SW2 != 0x0C ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Bad status bytes: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

   memset(pa_cSerNum, '\0', iSerNumLg);
   for ( i = 0 ; i < 16 ; i++ )
   {
      sprintf (pa_cSerNum + 2*i, "%02X", recvbuf[i]);
   }

cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidSignData"
DWORD BeidSignData(PCARD_DATA  pCardData, unsigned int HashAlgo, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature)
{
   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[1024];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   static const unsigned char MD2_AID[] = {
      0x30, 0x20, 
         0x30, 0x0c, 
            0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x02, 
            0x05, 0x00, 
         0x04, 0x10
   };
   static const unsigned char MD4_AID[] = {
      0x30, 0x20, 
         0x30, 0x0c, 
            0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x04, 
            0x05, 0x00, 
         0x04, 0x10
   };
   static const unsigned char MD5_AID[] = {
	   0x30, 0x20,
		   0x30, 0x0c,
			   0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
			   0x05, 0x00,
		   0x04, 0x10
   };
   static const unsigned char SHA1_AID[] = {
	   0x30, 0x21,
		   0x30, 0x09,
			   0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
		   0x05, 0x00,
		   0x04, 0x14
   };
   static const unsigned char SHA256_AID[] = {
	   0x30, 0x31,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
		   0x05, 0x00,
		   0x04, 0x20
   };
   static const unsigned char SHA384_AID[] = {
	   0x30, 0x41,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02,
		   0x05,0x00,
		   0x04, 0x30
   };
   static const unsigned char SHA512_AID[] = {
	   0x30, 0x51,
		   0x30, 0x0d,
			   0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03,
		   0x05, 0x00,
		   0x04, 0x40
   };
   static const unsigned char RIPEMD160_AID[] = {
	   0x30, 0x21,
		   0x30, 0x09,
			   0x06, 0x05, 0x2B, 0x24,	0x03, 0x02, 0x01,
		   0x05, 0x00,
		   0x04, 0x14
   };

   unsigned int            i          = 0;
   unsigned int            cbHdrHash  = 0;
   const unsigned char     *pbHdrHash = NULL;

   /* Sign Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0x2A;   /* PSO: Compute Digital Signature COMMAND */
   Cmd [2] = 0x9E;
   Cmd [3] = 0x9A;

   /* Length of data to be signed   */
   switch (HashAlgo)
   {
   case HASH_ALGO_NONE:
      LogTrace(LOGTYPE_INFO, WHERE, "NONE");
      cbHdrHash = 0;
      pbHdrHash = NULL;
      break;

   case HASH_ALGO_MD2:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD2");
      cbHdrHash = sizeof(MD2_AID);
      pbHdrHash = MD2_AID;
      break;
   case HASH_ALGO_MD4:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD4");
      cbHdrHash = sizeof(MD4_AID);
      pbHdrHash = MD4_AID;
      break;
   case HASH_ALGO_MD5:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_MD5");
      cbHdrHash = sizeof(MD5_AID);
      pbHdrHash = MD5_AID;
      break;
   case HASH_ALGO_SHA1:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA1");
      cbHdrHash = sizeof(SHA1_AID);
      pbHdrHash = SHA1_AID;
      break;
   case HASH_ALGO_SHA_256:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_256");
      cbHdrHash = sizeof(SHA256_AID);
      pbHdrHash = SHA256_AID;
      break;
   case HASH_ALGO_SHA_384:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_384");
      cbHdrHash = sizeof(SHA384_AID);
      pbHdrHash = SHA384_AID;
      break;
   case HASH_ALGO_SHA_512:
      LogTrace(LOGTYPE_INFO, WHERE, "CALG_SHA_512");
      cbHdrHash = sizeof(SHA512_AID);
      pbHdrHash = SHA512_AID;
      break;
   default:
      break;
   }
   if ( HashAlgo == HASH_ALGO_NONE )
   {
      Cmd [4] = (BYTE)(cbToBeSigned);
      memcpy(Cmd + 5, pbToBeSigned, cbToBeSigned);
      uiCmdLg = 5 + cbToBeSigned;
   }
   else
   {
      Cmd [4] = (BYTE)(cbToBeSigned + cbHdrHash);
      memcpy(Cmd + 5, pbHdrHash, cbHdrHash);
      memcpy(Cmd + 5 + cbHdrHash, pbToBeSigned, cbToBeSigned);
      uiCmdLg = 5 + cbHdrHash + cbToBeSigned;
   }

#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\signdata.bin", cbHdrHash + cbToBeSigned, (char *)&Cmd[5]);
#endif

   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SIGN) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];

   if ( (SW1 != 0x61) || (SW2 != 0x80) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Sign Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( SW1 == 0x69 )
      {
         CLEANUP(SCARD_W_SECURITY_VIOLATION);
      }
      else
      {
         CLEANUP(SCARD_E_UNEXPECTED);
      }
   }

   /* Retrieve signature Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0xC0;   /* PSO: GET RESPONSE COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x80;   /* Length of response */
   uiCmdLg = 5;

   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (Get Response) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Get Response Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   if ( (recvlen - 2) != 0x80 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X][0x%02X]", recvlen - 2, 0x80);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   *pcbSignature = 0x80;

   /* Allocate memory for the target buffer */
   *ppbSignature = pCardData->pfnCspAlloc(*pcbSignature);
   if ( *ppbSignature == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbSignature]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
   /* Copy the signature */
   for ( i = 0 ; i < *pcbSignature ; i++ )
   {
      (*ppbSignature)[i] = recvbuf[*pcbSignature - i - 1];
   }

cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidReadFile"
DWORD BeidReadFile(PCARD_DATA  pCardData, DWORD dwOffset, DWORD *cbStream, PBYTE pbStream)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   DWORD             cbRead      = 0;
   DWORD             cbPartRead  = 0;

   /***************/
   /* Read File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xB0; /* READ BINARY COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x00;
   uiCmdLg = 5;

   while ( ( *cbStream - cbRead ) > 0 )
   {
		Cmd[2] = (BYTE)((dwOffset + cbRead) >> 8);   /* set reading startpoint     */
		Cmd[3] = (BYTE)(dwOffset + cbRead);

      cbPartRead = *cbStream - cbRead;
		if(cbPartRead > BEID_READ_BINARY_MAX_LEN)    /*if more than maximum length */
      {
         Cmd[4] = BEID_READ_BINARY_MAX_LEN;        /* is requested, than read    */
      }
		else                                         /* maximum length             */
      {
			Cmd[4] = (BYTE)(cbPartRead);
      }
      dwReturn = SCardTransmit(pCardData->hScard, 
                               &ioSendPci, 
                               Cmd, 
                               uiCmdLg, 
                               &ioRecvPci, 
                               recvbuf, 
                               &recvlen);
      if ( dwReturn != SCARD_S_SUCCESS )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
         CLEANUP(dwReturn);
      }
      SW1 = recvbuf[recvlen - 2];
      SW2 = recvbuf[recvlen - 1];
      if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
         CLEANUP(dwReturn);
      }

      memcpy (pbStream + cbRead, recvbuf, recvlen - 2);
      cbRead += recvlen - 2;
   }

cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidReadCert"
DWORD BeidReadCert(PCARD_DATA  pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   BYTE              bFileID[6] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x00};
   BYTE              cbFileID   = sizeof(bFileID);

   BYTE              bRead [255];
   DWORD             cbRead;

   DWORD             cbCertif;

   /***************/
   /* Select File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xA4; /* SELECT COMMAND */
   Cmd [2] = 0x08;
   Cmd [3] = 0x0C;
   Cmd [4] = cbFileID;
   uiCmdLg = 5;

   switch (dwCertSpec)
   {
   case CERT_AUTH:
      bFileID [5] = 0x38;
      break;
   case CERT_NONREP:
      bFileID [5] = 0x39;
      break;
   case CERT_CA:
      bFileID [5] = 0x3a;
      break;
   case CERT_ROOTCA:
      bFileID [5] = 0x3b;
      break;
   }

   memcpy(&Cmd[5], bFileID, cbFileID);
   uiCmdLg += cbFileID;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

   /* Read First 4 bytes */
   cbRead = 4;
   dwReturn = BeidReadFile(pCardData, 0, &cbRead, bRead);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   cbCertif = (bRead[2] << 8) + bRead[3] + 4;
   cbRead = cbCertif;

   *ppbCertif = pCardData->pfnCspAlloc(cbCertif);
   if ( *ppbCertif == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbCertif]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   dwReturn = BeidReadFile(pCardData, 0, &cbCertif, *ppbCertif);
   if ( ( dwReturn != SCARD_S_SUCCESS ) ||
        ( cbCertif != cbRead          ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   /* Certificate Length */
   *pcbCertif = cbCertif;

cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/
