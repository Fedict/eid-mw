/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT.
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

#include "globmdrv.h"
#include "log.h"
#include "smartcard.h"
#include "externalpinui.h"

#include <commctrl.h>
/****************************************************************************************************/

#define CHALLENGE_DATA_SIZE         16

#define BELPIC_MAX_FILE_SIZE        65535
#define BELPIC_PIN_BUF_SIZE         8
#define BELPIC_MIN_USER_PIN_LEN     4
#define BELPIC_MAX_USER_PIN_LEN     12
#define BELPIC_PAD_CHAR			    0xFF
#define BELPIC_KEY_REF_NONREP		0x83

/****************************************************************************************************/

#define WHERE "BeidParsePrKDF"
DWORD BeidParsePrKDF(PCARD_DATA  pCardData, DWORD *cbStream, BYTE *pbStream, WORD *cbKeySize)
{
	DWORD dwReturn  = 0;
	DWORD dwCounter = 0;
	DWORD dwInc = 0;
	*cbKeySize = 0;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
	/********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbStream == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [ppbStream]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
	if ( cbStream == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbStream]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
	 }

	 if(pbStream[dwCounter] == 0x30) //0x30 means sequence
	 {
		 LogTrace(LOGTYPE_TRACE, WHERE, "sequence [0x30]");
		 dwCounter++; //jump to sequence length
		 LogTrace(LOGTYPE_TRACE, WHERE, "sequence length [0x%.2X]",pbStream[dwCounter]);
		 dwInc = pbStream[dwCounter];
		 dwCounter += dwInc; //add length (to jump over sequence)
		 if( dwCounter < (*cbStream))
		 {
			 //the last 2 bytes are the key size
			 *cbKeySize = (pbStream[dwCounter-1])*256;
			 *cbKeySize += (pbStream[dwCounter]);
			 LogTrace(LOGTYPE_INFO, WHERE, "rsa key size is %d",*cbKeySize);
		 }
		 else
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "*cbStream = %d dwCounter = %d",*cbStream,dwCounter);
			 LogDump(*cbStream,pbStream);
			 CLEANUP(PEERDIST_ERROR_CANNOT_PARSE_CONTENTINFO);		 
		 }
	 }
	 else
	 {
		 LogTrace(LOGTYPE_ERROR, WHERE, "Expected 0x30 instead of ox%.2x",pbStream[dwCounter]);
		 LogDump(*cbStream,pbStream);
		 CLEANUP(PEERDIST_ERROR_CANNOT_PARSE_CONTENTINFO);		 
	 }

cleanup:

	 LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	 return(dwReturn);
}
#undef WHERE



#define WHERE "BeidDelayAndRecover"
void BeidDelayAndRecover(PCARD_DATA  pCardData,
						 BYTE   SW1, 
						 BYTE   SW2,
						 DWORD  dwReturn)
{

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	if( (dwReturn == SCARD_E_COMM_DATA_LOST) || (dwReturn == SCARD_E_NOT_TRANSACTED) )
	{
		DWORD ap = 0;
		int i = 0;

		LogTrace(LOGTYPE_WARNING, WHERE, "Card is confused, trying to recover...");

		for (i = 0; (i < 10) && (dwReturn != SCARD_S_SUCCESS); i++)
		{
			if (i != 0)
				Sleep(1000);

			dwReturn = SCardReconnect(pCardData->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, SCARD_RESET_CARD, &ap);

			LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] SCardReconnect errorcode: [0x%08X]", i, dwReturn);

			if ( dwReturn != SCARD_S_SUCCESS )
			{			
				if(dwReturn == SCARD_W_REMOVED_CARD)
				{
					LogTrace(LOGTYPE_INFO, WHERE, "SCARD_W_REMOVED_CARD");
					LogTrace(LOGTYPE_INFO, WHERE, "Exit API... ");
					return;
				}
				continue;
			}
			// transaction is lost after an SCardReconnect()
			dwReturn = SCardBeginTransaction(pCardData->hScard);
			LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] SCardBeginTransaction errorcode: [0x%08X]", i, dwReturn);

			if ( dwReturn != SCARD_S_SUCCESS )
			{		
				continue;
			}
			dwReturn = BeidSelectApplet(pCardData);
			LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] BeidSelectApplet errorcode: [0x%08X]", i, dwReturn);

			if ( dwReturn != SCARD_S_SUCCESS )
			{
				continue;
			}

			LogTrace(LOGTYPE_INFO, WHERE, "  Card recovered in loop %d", i);
		}
		if(i >=10)
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X], Failed to recover", dwReturn);
		}
	}
	if ( (( SW1 == 0x90 ) && ( SW2 == 0x00 )) ||
		 ( SW1 == 0x61 ) ||
		 ( SW1 == 0x6c ) )
	{
		;//no error received, no sleep needed
	}
	else
	{
		Sleep(25);
	}
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
}
#undef WHERE



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
   unsigned int      i         = 0;

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

	 for ( i = 0 ; i < (unsigned char) cbPin ; i++ )
	 {
		 /* Don't allow non-numerical PIN entries */
		 if ( ( pbPin[i] < 0x30 ) || ( pbPin[i] > 0x39 ) )
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbPin[i]]");
			 CLEANUP(SCARD_W_WRONG_CHV);
		 }
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
			// 0x30 = ASCII '0'
			Cmd [6 + (i/2)] = (((pbPin[i] - 0x30) << 4) | 0x0F);
      }
      else
      {
		  // 0x30 = ASCII '0'
          Cmd [6 + (i/2)] = (Cmd[6 + (i/2)] & 0xF0) + ((pbPin[i] - 0x30) & 0x0F);
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

   memset(Cmd, 0, uiCmdLg);

   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit returncode: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
	  LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }

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

#define WHERE "BeidAuthenticateExternal"
DWORD BeidAuthenticateExternal(
	PCARD_DATA   pCardData, 
	PDWORD       pcAttemptsRemaining,
	BOOL		 bSilent
	) 
{
	DWORD						dwReturn  = 0;
	SCARD_IO_REQUEST			ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
	SCARD_IO_REQUEST			ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

	PIN_VERIFY_STRUCTURE		verifyCommand;

	unsigned int				uiCmdLg   = 0;
	unsigned char				recvbuf[256];
	unsigned char				ucLastKey;
	unsigned long				recvlen     = sizeof(recvbuf);
	BYTE						SW1, SW2;
	int							i           = 0;
	int							offset		= 0;
	DWORD						dwDataLen;
	BOOL						bRetry      = TRUE;
	int							nButton;

	EXTERNAL_PIN_INFORMATION	externalPinInfo;
	HANDLE						DialogThreadHandle;

#ifndef NO_DIALOGS
	DWORD						dwRetriesLeft;

	wchar_t						wchErrorMessage[500];
	wchar_t						wchMainInstruction[100];
#endif

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}


	/*********************/
	/* External PIN Info */
	/*********************/
	externalPinInfo.hCardHandle = pCardData->hScard;
	CCIDgetFeatures(&(externalPinInfo.features), externalPinInfo.hCardHandle);

	/*********************/
	/* Get Parent Window */
	/*********************/
	dwReturn = CardGetProperty(pCardData, 
		CP_PARENT_WINDOW, 
		(PBYTE) &(externalPinInfo.hwndParentWindow), 
		sizeof(externalPinInfo.hwndParentWindow), 
		&dwDataLen, 
		0);
	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
		externalPinInfo.hwndParentWindow = NULL;
	}


	/*********************/
	/* Get Pin Context String */
	/*********************/		
	dwReturn = CardGetProperty(pCardData, 
		CP_PIN_CONTEXT_STRING, 
		(PBYTE) externalPinInfo.lpstrPinContextString, 
		sizeof(externalPinInfo.lpstrPinContextString), 
		&dwDataLen, 
		0);
	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
		wcsncpy_s(externalPinInfo.lpstrPinContextString, sizeof(externalPinInfo.lpstrPinContextString) / sizeof(wchar_t), L"", sizeof(externalPinInfo.lpstrPinContextString) / sizeof(wchar_t));
#else
		wcsncpy(externalPinInfo.lpstrPinContextString, L"", sizeof(externalPinInfo.lpstrPinContextString) / sizeof(wchar_t));
#endif
	}

	/**********/
	/* Log On */
	/**********/

	createVerifyCommand(&verifyCommand);

	uiCmdLg = sizeof(verifyCommand);
	recvlen = sizeof(recvbuf);

	while (bRetry) {
		bRetry = FALSE;
		nButton = -1;

		// We introduce a short sleep before starting the PIN VERIFY procedure
		// Reason: we do this for users with a combined keyboard/secure PIN pad smartcard reader
		//   "enter" key far right on the keyboard ==  "OK" button of the PIN pad
		//   Problem: key becomes PIN-pad button before key is released. Result: the keyup event is not sent.
		//   This sleep gives the user some time to release the Enter key.

		Sleep(100);

		if(externalPinInfo.features.VERIFY_PIN_DIRECT != 0)
		{
			externalPinInfo.iPinCharacters = 0;
			externalPinInfo.cardState = CS_PINENTRY;
			// show dialog
			if (!bSilent)
				DialogThreadHandle = CreateThread(NULL, 0, DialogThreadPinEntry, &externalPinInfo, 0, NULL);

			if(externalPinInfo.features.USE_PPDU == 0)
			{
				dwReturn = SCardControl(pCardData->hScard, 
					externalPinInfo.features.VERIFY_PIN_DIRECT, 
					&verifyCommand, 
					uiCmdLg,                              
					recvbuf, 
					recvlen,
					&recvlen);

				LogTrace(LOGTYPE_TRACE, WHERE, "SCardControl return code: [0x%08X]", dwReturn);
				externalPinInfo.cardState = CS_PINENTERED;
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%08X]", dwReturn);
					CLEANUP(dwReturn);
				}
			}
			else{
				BYTE pbSendBufferVerifyPINDirect[256];// = {0xFF ,0xC2 ,0x01 ,0x06 , 0x00};
				BYTE bSendBufferVerifyPINDirectLength = 5;

				pbSendBufferVerifyPINDirect[0] = 0xFF;
				pbSendBufferVerifyPINDirect[1] = 0xC2;
				pbSendBufferVerifyPINDirect[2] = 0x01;
				pbSendBufferVerifyPINDirect[3] = 0x06;
				pbSendBufferVerifyPINDirect[4] = sizeof(verifyCommand);				

				if(sizeof(verifyCommand) < (sizeof(pbSendBufferVerifyPINDirect)+5) )
				{
					memcpy(&pbSendBufferVerifyPINDirect[5],&verifyCommand,sizeof(verifyCommand));
					bSendBufferVerifyPINDirectLength += sizeof(verifyCommand);
				}
				LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit PPDU");

				dwReturn = SCardTransmit(pCardData->hScard,
					&ioSendPci,
					pbSendBufferVerifyPINDirect,
					bSendBufferVerifyPINDirectLength,
					&ioRecvPci,
					recvbuf,
					&recvlen);

				LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit PPDU return code: [0x%08X]", dwReturn);
				externalPinInfo.cardState = CS_PINENTERED;
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit PPDU errorcode: [0x%08X]", dwReturn);
					CLEANUP(dwReturn);
				}

			}
		}
		else
		{
			dwReturn = SCardControl(pCardData->hScard, 
				externalPinInfo.features.VERIFY_PIN_START, 
				&verifyCommand, 
				uiCmdLg,                              
				recvbuf, 
				recvlen,
				&recvlen);
			LogTrace(LOGTYPE_TRACE, WHERE, "SCardControl return code: [0x%08X]", dwReturn);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%08X]", dwReturn);
				CLEANUP(dwReturn);
			}
			externalPinInfo.iPinCharacters = 0;
			externalPinInfo.cardState = CS_PINENTRY;

			// show dialog
			if (!bSilent)
				DialogThreadHandle = CreateThread(NULL, 0, DialogThreadPinEntry, &externalPinInfo, 0, NULL);
			while (1) {
				dwReturn = SCardControl(pCardData->hScard,
					externalPinInfo.features.GET_KEY_PRESSED,
					NULL,
					0,
					recvbuf,
					recvlen,
					&recvlen);
				LogTrace(LOGTYPE_TRACE, WHERE, "SCardControl return code: [0x%08X]", dwReturn);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%08X]", dwReturn);
					CLEANUP(dwReturn);
				}
				ucLastKey = recvbuf[0];
				switch (recvbuf[0]) {
				case 0x00:
					// No key 
					Sleep(200);
					break;
				case 0x0d:
					// OK button
					goto endkeypress;
				case 0x1b:
					// Cancel button
					goto endkeypress;
				case 0x40:
					// Aborted/timeout
					goto endkeypress;
				case 0x2b:
					// 0-9
					externalPinInfo.iPinCharacters++;
					break;
				case 0x08:	
					// Backspace
					externalPinInfo.iPinCharacters--;
					break;
				case 0x0a:
					// Clear
					externalPinInfo.iPinCharacters = 0;
					break;
				default:
					//printf("Key pressed: 0x%x\n", bRecvBuffer[0]);
					;
				}

			}
endkeypress:

			externalPinInfo.cardState = CS_PINENTERED;
			dwReturn = SCardControl(pCardData->hScard,
				externalPinInfo.features.VERIFY_PIN_FINISH,
				NULL,
				0,
				recvbuf,
				sizeof(recvbuf),
				&recvlen);
			LogTrace(LOGTYPE_TRACE, WHERE, "SCardControl return code: [0x%08X]", dwReturn);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%08X]", dwReturn);
				CLEANUP(dwReturn);
			}
			SW1 = recvbuf[recvlen-2];
			SW2 = recvbuf[recvlen-1];
			if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
			{
				dwReturn = SCARD_W_WRONG_CHV;
				LogTrace(LOGTYPE_ERROR, WHERE, "CardAuthenticateEx Failed: [0x%02X][0x%02X]", SW1, SW2);
#ifndef NO_DIALOGS
				if (SW1 == 0x64) {
					//error during pin entry
					switch(SW2){
					case 0x00:
						// Timeout
						if (ucLastKey == 0x0d) {
							// OK button preceded by no other keys also results in 0x64 0x00

							swprintf(wchMainInstruction, t[PIN_TOO_SHORT_MAININSTRUCTIONS][getLanguage()]);
							swprintf(wchErrorMessage, t[PIN_TOO_SHORT_CONTENT][getLanguage()] );
						} else {
							swprintf(wchMainInstruction, t[PIN_TIMED_OUT_MAININSTRUCTIONS][getLanguage()]);
							// the user entered something but probably forgot to push OK.
							swprintf(wchErrorMessage, t[PIN_TIMED_OUT_CONTENT][getLanguage()]);
						}
						break;
					case 0x01:
						// Cancelled
						swprintf(wchMainInstruction, t[PIN_CANCELLED_MAININSTRUCTIONS][getLanguage()]);
						swprintf(wchErrorMessage, t[PIN_CANCELLED_CONTENT][getLanguage()]);
						break;
					case 0x02:
						// PINs do not match
						swprintf(wchMainInstruction, t[PIN_DO_NOT_MATCH_MAININSTRUCTIONS][getLanguage()]);
						swprintf(wchErrorMessage, t[PIN_DO_NOT_MATCH_CONTENT][getLanguage()]);
						break;
					case 0x03:
						// PIN size error
						if (externalPinInfo.iPinCharacters > 0 && externalPinInfo.iPinCharacters < BELPIC_MIN_USER_PIN_LEN) {
							// PIN too short
							swprintf(wchMainInstruction, t[PIN_TOO_SHORT_MAININSTRUCTIONS][getLanguage()]);
							swprintf(wchErrorMessage, t[PIN_TOO_SHORT_CONTENT][getLanguage()]);
						} else {
							if (externalPinInfo.iPinCharacters >= BELPIC_MAX_USER_PIN_LEN) {
								// PIN too long
								swprintf(wchMainInstruction, t[PIN_TOO_LONG_MAININSTRUCTIONS][getLanguage()]);
								swprintf(wchErrorMessage, t[PIN_TOO_LONG_CONTENT][getLanguage()]);
							} else {
								// no info about PIN chars
								swprintf(wchMainInstruction,  t[PIN_SIZE_MAININSTRUCTIONS][getLanguage()]);
								swprintf(wchErrorMessage, t[PIN_SIZE_CONTENT][getLanguage()]);
							}
						}
						break;
					default:
						// Should not happen
						swprintf(wchMainInstruction, t[PIN_UNKNOWN_MAININSTRUCTIONS][getLanguage()]);
						swprintf(wchErrorMessage,  t[PIN_UNKNOWN_CONTENT][getLanguage()], SW1,SW2);
						break;
					}
					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_RETRY_BUTTON  | TDCBF_CANCEL_BUTTON ,
						TD_ERROR_ICON,
						&nButton);

				}
				if (SW1 == 0x63) {
					// Invalid PIN
					dwRetriesLeft = SW2 & 0x0F;
					if ( pcAttemptsRemaining != NULL )
					{
						/* -1: Don't support returning the count of remaining authentication attempts */
						*pcAttemptsRemaining = dwRetriesLeft;
					}
					swprintf(wchMainInstruction, t[PIN_INVALID_MAININSTRUCTIONS][getLanguage()]);
					swprintf(wchErrorMessage, t[PIN_INVALID_CONTENT][getLanguage()], dwRetriesLeft);

					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_RETRY_BUTTON  | TDCBF_CANCEL_BUTTON,
						TD_ERROR_ICON,
						&nButton);
				}

				if (SW1 == 0x69 && SW2 == 0x83) {
					// PIN blocked
					swprintf(wchMainInstruction, t[PIN_BLOCKED_MAININSTRUCTIONS][getLanguage()]);
					swprintf(wchErrorMessage, t[PIN_BLOCKED_CONTENT][getLanguage()]);
					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_OK_BUTTON,
						TD_ERROR_ICON,
						&nButton);
					dwReturn = SCARD_W_CHV_BLOCKED;
				}
				bRetry = (nButton == IDRETRY);
#endif
			}
			else
			{
				LogTrace(LOGTYPE_INFO, WHERE, "Logged on...");
			}
		}
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
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }
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
DWORD BeidMSE(PCARD_DATA		pCardData, 
              BYTE					bKey,
							BYTE					bAlgo) 
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

	 if ( (bKey != 0x82) && (bKey != 0x83) )
	 {
      LogTrace(LOGTYPE_INFO, WHERE, "SET COMMAND: undefined key [0x%.2x]", bKey);
      CLEANUP(SCARD_E_UNEXPECTED);
   }
	 if ( (bAlgo != 0x01) && (bAlgo != 0x02) && (bAlgo != 0x04) && (bAlgo != 0x08) && (bAlgo != 0x10) && (bAlgo != 0x20) && (bAlgo != 0x40) )
	 {
		  LogTrace(LOGTYPE_INFO, WHERE, "SET COMMAND: undefined algo [0x%.2x]", bAlgo);
      CLEANUP(SCARD_E_UNEXPECTED);
	 }

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
   Cmd [7] = bAlgo;//0x01;   /* RSA PKCS#1                    */
   Cmd [8] = 0x84;   /* TAG for private key reference */
	 Cmd [9] = bKey;	/*0x82 for AUTH, 0x83 for NONREP*/

   uiCmdLg = 10;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SET) errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }
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

   unsigned int      i        = 0;
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

	 for ( i = 0 ; i < (unsigned char) cbCurrentAuthenticator ; i++ )
	 {
		 /* Don't allow non-numerical PIN entries */
		 if ( ( pbCurrentAuthenticator[i] < 0x30 ) || ( pbCurrentAuthenticator[i] > 0x39 ) )
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbCurrentAuthenticator[i]]");
			 CLEANUP(SCARD_W_WRONG_CHV);
		 }
	 }

	 for ( i = 0 ; i < (unsigned char) cbNewAuthenticator ; i++ )
	 {
		 /* Don't allow non-numerical PIN entries */
		 if ( ( pbNewAuthenticator[i] < 0x30 ) || ( pbNewAuthenticator[i] > 0x39 ) )
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbNewAuthenticator[i]]");
			 CLEANUP(SCARD_W_WRONG_CHV);
		 }
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

   memset(Cmd, 0, uiCmdLg);

   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }

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
DWORD BeidGetCardSN(PCARD_DATA  pCardData, 
					PBYTE pbSerialNumber, 
					DWORD cbSerialNumber, 
					PDWORD pdwSerialNumber) 
{
	DWORD                   dwReturn = 0;

	SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
	SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

	unsigned char           Cmd[128];
	unsigned int            uiCmdLg = 0;

	unsigned char           recvbuf[256];
	unsigned long           recvlen = sizeof(recvbuf);
	BYTE                    SW1 = 0;
	BYTE                    SW2 = 0;

	int                     i = 0;
	int                     iWaitApdu = 100;
	int   				      bRetry = 0;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
	if (cbSerialNumber < 16) {
		CLEANUP(ERROR_INSUFFICIENT_BUFFER);
	}

	*pdwSerialNumber = 0;

	Cmd [0] = 0x80;
	Cmd [1] = 0xE4;
	Cmd [2] = 0x00;
	Cmd [3] = 0x00;
	Cmd [4] = 0x1C;//10 we only need 16 bytes, asking for 28 would help out some readers in combination with default ccid driver (if it weren't for certprop service, which also asks 16 bytes)
	uiCmdLg = 5;

	do {
		i++;
		bRetry = 0;

		Sleep(iWaitApdu);
		recvlen = sizeof(recvbuf);
		dwReturn = SCardTransmit(pCardData->hScard, &ioSendPci, Cmd, uiCmdLg, &ioRecvPci, recvbuf, &recvlen);
		if (recvlen >= 2)
		{
			SW1 = recvbuf[recvlen-2];
			SW2 = recvbuf[recvlen-1];
			LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
			BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);

			if (dwReturn == SCARD_E_COMM_DATA_LOST)
			{
				bRetry++;
				LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit failed with SCARD_E_COMM_DATA_LOST. Sleep %d ms and try again", iWaitApdu);
			}
			if (dwReturn == SCARD_S_SUCCESS)
			{
				// 6d = "command not available in current life cycle"
				if ( SW1 == 0x6d )
				{
					LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit returned SW1 = 6d. Sleep %d ms and try again", iWaitApdu);
					bRetry++;
				}
			}
		}
		else
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "GetCardData recvlen = %d\n", recvlen);
		}
	} while (bRetry != 0 && i < 10);


	if(dwReturn == SCARD_S_SUCCESS ) 
	{
//		if (SW1 == 0x6c)
//		{
//			//SW2 contains length of the carddata (1C)
//			Cmd [4] = SW2;
//			uiCmdLg = 5;
//			recvlen = sizeof(recvbuf);
//			dwReturn = SCardTransmit(pCardData->hScard, &ioSendPci, Cmd, uiCmdLg, &ioRecvPci, recvbuf, &recvlen);
//			if (recvlen >= 2)
//			{
//				SW1 = recvbuf[recvlen-2];
//				SW2 = recvbuf[recvlen-1];
//			}
//			else
//			{
//				LogTrace(LOGTYPE_ERROR, WHERE, "GetCardData recvlen = %d\n", recvlen);
//			}
//		}
		if(recvlen >= 18)
		{
			if( (( SW1 == 0x90 ) && ( SW2 == 0x00 )) || (( SW1 == 0x61 ) && ( SW2 == 0x0C )) )
			{
				*pdwSerialNumber = 16;
				memcpy(pbSerialNumber, recvbuf, 16);
			}
			else
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Bad status bytes: [0x%02X][0x%02X]", SW1, SW2);
				CLEANUP(SCARD_E_UNEXPECTED);
			}
		}
		else
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Serial number too short, status bytes: [0x%02X][0x%02X]", SW1, SW2);
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}
	else //dwReturn == SCARD_S_SUCCESS
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (GET_CARD_DATA) errorcode: [0x%08X]", dwReturn);
		CLEANUP(dwReturn);
	}

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidSignData"
DWORD BeidSignData(PCARD_DATA  pCardData, unsigned int HashAlgo, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature)
{
	DWORD                   dwReturn = 0;

	SCARD_IO_REQUEST        ioSendPci = { 1, sizeof(SCARD_IO_REQUEST) };
	SCARD_IO_REQUEST        ioRecvPci = { 1, sizeof(SCARD_IO_REQUEST) };

	unsigned char           Cmd[128];
	unsigned int            uiCmdLg = 0;

	unsigned char           recvbuf[1024];
	unsigned long           recvlen = sizeof(recvbuf);
	BYTE                    SW1, SW2;
	VENDOR_SPECIFIC*		pVendorSpec;

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

	unsigned int            i = 0;
	unsigned int            cbHdrHash = 0;
	const unsigned char     *pbHdrHash = NULL;

	/* Sign Command */
	Cmd[0] = 0x00;
	Cmd[1] = 0x2A;   /* PSO: Compute Digital Signature COMMAND */
	Cmd[2] = 0x9E;
	Cmd[3] = 0x9A;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
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
	if (HashAlgo == HASH_ALGO_NONE)
	{
		Cmd[4] = (BYTE)(cbToBeSigned);
		memcpy(Cmd + 5, pbToBeSigned, cbToBeSigned);
		uiCmdLg = 5 + cbToBeSigned;
	}
	else
	{
		Cmd[4] = (BYTE)(cbToBeSigned + cbHdrHash);
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
	SW1 = recvbuf[recvlen - 2];
	SW2 = recvbuf[recvlen - 1];
	LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
	BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
	if (dwReturn != SCARD_S_SUCCESS)
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SIGN) errorcode: [0x%08X]", dwReturn);
		CLEANUP(dwReturn);
	}
	LogTrace(LOGTYPE_TRACE, WHERE, "ADPU response: [0x%02X][0x%02X]", SW1, SW2);
	LogTrace(LOGTYPE_TRACE, WHERE, "recvlen = %d", recvlen, SW2);
	if (SW1 == 0x61)
	{
		/* Retrieve signature Command */
		Cmd[0] = 0x00;
		Cmd[1] = 0xC0;   /* PSO: GET RESPONSE COMMAND */
		Cmd[2] = 0x00;
		Cmd[3] = 0x00;
		Cmd[4] = SW2;   /* Length of response */
		uiCmdLg = 5;

		recvlen = sizeof(recvbuf);
		dwReturn = SCardTransmit(pCardData->hScard,
			&ioSendPci,
			Cmd,
			uiCmdLg,
			&ioRecvPci,
			recvbuf,
			&recvlen);
		SW1 = recvbuf[recvlen - 2];
		SW2 = recvbuf[recvlen - 1];
		LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
		BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
		if (dwReturn != SCARD_S_SUCCESS)
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (Get Response) errorcode: [0x%08X]", dwReturn);
			CLEANUP(dwReturn);
		}
		if ((SW1 != 0x90) || (SW2 != 0x00))
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Get Response Failed: [0x%02X][0x%02X]", SW1, SW2);
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}//end of get response handling
	else if ((SW1 != 0x90) || (SW2 != 0x00))
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Sign Failed: [0x%02X][0x%02X]", SW1, SW2);

		if (SW1 == 0x69)
		{
			CLEANUP(SCARD_W_SECURITY_VIOLATION);
		}
		else
		{
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}
	pVendorSpec = pCardData->pvVendorSpecific;
	if (pVendorSpec->bBEIDCardType == BEID_ECC_CARD)
	{
		if ((recvlen - 2) == 0x60)
		{
			*pcbSignature = 0x60; //48d * 2
		}
		else //not supported
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X]", recvlen - 2);
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}
	else
	{
		if ((recvlen - 2) == 0x80)
		{
			*pcbSignature = 0x80; //128d
		}
		else if ((recvlen - 2) == 0x100)
		{
			*pcbSignature = 0x100; //256d
		}
		else //not supported
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X]", recvlen - 2);
			CLEANUP(SCARD_E_UNEXPECTED);
		}
	}
	/* Allocate memory for the target buffer */
	*ppbSignature = (PBYTE)(pCardData->pfnCspAlloc(*pcbSignature));
	if (*ppbSignature == NULL)
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbSignature]");
		CLEANUP(SCARD_E_NO_MEMORY);
	}
	/* Copy the signature */
	if (pVendorSpec->bBEIDCardType == BEID_RSA_CARD)
	{
		//for RSA, revert byte order
		for (i = 0; i < *pcbSignature; i++)
		{
			(*ppbSignature)[i] = recvbuf[*pcbSignature - i - 1];
		}
	}
	else
	{
		for (i = 0; i < *pcbSignature; i++)
		{
			(*ppbSignature)[i] = recvbuf[i];
		}
	}

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/
// read up to *cbStream bytes from the currently selected file, starting at offset dwOffset
// and store them in pbStream
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
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	 while ( ( *cbStream - cbRead ) > 0 )
	 {
		 Cmd[2] = (BYTE)((dwOffset + cbRead) >> 8);   /* set reading startpoint     */
		Cmd[3] = (BYTE)( (dwOffset + cbRead)&(0xFF) );

		 cbPartRead = *cbStream - cbRead;
		 if(cbPartRead > BEID_READ_BINARY_MAX_LEN)    /*if more than maximum length */
		 {
			 Cmd[4] = BEID_READ_BINARY_MAX_LEN;        /* is requested, than read    */
		 }
		 else                                         /* maximum length             */
		 {
			 Cmd[4] = (BYTE)(cbPartRead);
		 }
		 recvlen = sizeof(recvbuf);
#ifdef _DEBUG
		 LogDump(uiCmdLg,Cmd);
#endif
		 dwReturn = SCardTransmit(pCardData->hScard, 
			 &ioSendPci, 
			 Cmd, 
			 uiCmdLg, 
			 &ioRecvPci, 
			 recvbuf, 
			 &recvlen);
		 SW1 = recvbuf[recvlen-2];
		 SW2 = recvbuf[recvlen-1];
		 LogTrace(LOGTYPE_TRACE, WHERE, "recvlen = %d SW1 = 0x%.02X SW2 = 0x%.02X", recvlen,SW1,SW2);
#ifdef _DEBUG
		 LogDump(recvlen,recvbuf);
#endif
		 LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
		 BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
		 if ( dwReturn != SCARD_S_SUCCESS )
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
			 CLEANUP(dwReturn);
		 }

		 /* Special case: when SW1 == 0x6C (=incorrect value of Le), we will
		 retransmit with SW2 as Le, if SW2 is smaller then the 
		 BEID_READ_BINARY_MAX_LEN */
		 if ( ( SW1 == 0x6c ) && ( SW2 <= BEID_READ_BINARY_MAX_LEN ) ) 
		 {
			 Cmd[4] = SW2;
			 recvlen = sizeof(recvbuf);
#ifdef _DEBUG
			 LogDump(uiCmdLg,Cmd);
#endif
			 dwReturn = SCardTransmit(pCardData->hScard, 
				 &ioSendPci, 
				 Cmd, 
				 uiCmdLg, 
				 &ioRecvPci, 
				 recvbuf, 
				 &recvlen);
			 LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
			 if ( dwReturn != SCARD_S_SUCCESS )
			 {
				 LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
				 CLEANUP(dwReturn);
			 }
			 SW1 = recvbuf[recvlen - 2];
			 SW2 = recvbuf[recvlen - 1];

			 memcpy (pbStream + cbRead, recvbuf, recvlen - 2);
			 cbRead += recvlen - 2;
			 break; //stop reading, we already had the length corrected
		 }

		 if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
		 {
			 LogTrace(LOGTYPE_ERROR, WHERE, "Read Binary Failed: [0x%02X][0x%02X]", SW1, SW2);
			 break; //stop reading, something ain't right
		 }

		 memcpy (pbStream + cbRead, recvbuf, recvlen - 2);
		 cbRead += recvlen - 2;
	 }
	*cbStream = cbRead;
	LogTrace(LOGTYPE_TRACE, WHERE, "cbRead = %d",cbRead);
cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "BeidSelectAndReadFile"
DWORD BeidSelectAndReadFile(PCARD_DATA  pCardData, DWORD dwOffset, BYTE cbFileID, PBYTE pbFileID, DWORD *cbStream, PBYTE * ppbStream)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;
   DWORD             cbReadBuf;

   /***************/
   /* Select File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xA4; /* SELECT COMMAND */
   Cmd [2] = 0x08;
   Cmd [3] = 0x0C;
   Cmd [4] = cbFileID;
   uiCmdLg = 5;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");


   memcpy(&Cmd[5], pbFileID, cbFileID);
   uiCmdLg += cbFileID;

#ifdef _DEBUG
	 LogDump(uiCmdLg,Cmd);
#endif
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

	*cbStream = 0;
	*ppbStream = NULL;
	cbReadBuf = 1024;
	//read the file in chunks of 1024 bytes
	while (cbReadBuf == 1024) 
	{
		if (*ppbStream == NULL)
			*ppbStream = (PBYTE) pCardData->pfnCspAlloc(cbReadBuf);
		else
			*ppbStream = (PBYTE) pCardData->pfnCspReAlloc(*ppbStream, *cbStream + cbReadBuf);

		if (*ppbStream == NULL) {
			LogTrace(LOGTYPE_ERROR, WHERE, "pfnCsp(Re)Alloc failed");
			CLEANUP(dwReturn);
		}
		LogTrace(LOGTYPE_ERROR, WHERE, "dwOffset = %d", dwOffset);
		dwReturn = BeidReadFile(pCardData, dwOffset, &cbReadBuf, *ppbStream + (*cbStream * sizeof(BYTE)));
		if ( dwReturn != SCARD_S_SUCCESS )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%08X]", dwReturn);
			pCardData->pfnCspFree(*ppbStream);
			*ppbStream = NULL;
			*cbStream = 0;
			CLEANUP(dwReturn);
		}
		dwOffset += cbReadBuf;
		*cbStream += cbReadBuf;
	}
cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
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
   BYTE              cbFileID   = (BYTE)sizeof(bFileID);

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

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

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
#ifdef _DEBUG
	 LogDump(uiCmdLg,Cmd);
#endif
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
   BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }
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
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }

   cbCertif = (bRead[2] << 8) + bRead[3] + 4;
   if (ppbCertif == NULL)
   {
	   // we will only return the file length
	   if (pcbCertif != NULL)
		   *pcbCertif = cbCertif;
	   LogTrace(LOGTYPE_INFO, WHERE, "returning filelength = %d", cbCertif);
	   CLEANUP(SCARD_S_SUCCESS);
   }
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
      LogTrace(LOGTYPE_ERROR, WHERE, "BeidReadFile errorcode: [0x%08X]", dwReturn);
      CLEANUP(dwReturn);
   }

   /* Certificate Length */
   *pcbCertif = cbCertif;

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return (dwReturn);
}
#undef WHERE


#define WHERE "BeidSelectApplet"
DWORD BeidSelectApplet(PCARD_DATA  pCardData)
{
	DWORD             dwReturn = 0;

	SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
	SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

	unsigned char     Cmd[128];
	unsigned int      uiCmdLg = 0;

	unsigned char     recvbuf[256];
	unsigned long     recvlen = sizeof(recvbuf);
	BYTE              SW1, SW2;
	BYTE              bBELPIC_AID[12] = { 0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35 };  
	BYTE              cbBELPIC_AID = sizeof(bBELPIC_AID);
	BYTE			  bAPPLET_AID[15] = { 0xA0, 0x00, 0x00, 0x00, 0x30, 0x29, 0x05, 0x70, 0x00, 0xAD, 0x13, 0x10, 0x01, 0x01, 0xFF };
	BYTE              cbAPPLET_AID = sizeof(bAPPLET_AID);

	int               i = 0;

	/***************/
	/* Select File */
	/***************/
	Cmd [0] = 0x00;
	Cmd [1] = 0xA4; /* SELECT COMMAND */
	Cmd [2] = 0x04;
	Cmd [3] = 0x0C;
	Cmd [4] = cbBELPIC_AID;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	memcpy(&Cmd[5], bBELPIC_AID, cbBELPIC_AID);

	uiCmdLg = 5 + cbBELPIC_AID;

	dwReturn = SCardTransmit(pCardData->hScard, 
		&ioSendPci, 
		Cmd, 
		uiCmdLg, 
		&ioRecvPci, 
		recvbuf, 
		&recvlen);
	SW1 = recvbuf[recvlen-2];
	SW2 = recvbuf[recvlen-1];
	LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit return code: [0x%08X]", dwReturn);
	BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
		CLEANUP(dwReturn);
    }

	if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);

		Cmd [0] = 0x00;
		Cmd [1] = 0xA4; /* SELECT COMMAND */
		Cmd [2] = 0x04;
		Cmd [3] = 0x00;
		Cmd [4] = cbAPPLET_AID;
		memcpy(&Cmd[5], bAPPLET_AID, cbAPPLET_AID);

		uiCmdLg = 5 + cbAPPLET_AID;
		recvlen = sizeof(recvbuf);
		dwReturn = SCardTransmit(pCardData->hScard, 
			&ioSendPci, 
			Cmd, 
			uiCmdLg, 
			&ioRecvPci, 
			recvbuf, 
			&recvlen);
		SW1 = recvbuf[recvlen-2];
		SW2 = recvbuf[recvlen-1];
		BeidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
		if ( dwReturn != SCARD_S_SUCCESS )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%08X]", dwReturn);
			CLEANUP(dwReturn);
		}

		if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
			CLEANUP(dwReturn);
		}
	}


cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return (dwReturn);
}
#undef WHERE



/****************************************************************************************************/

/****************************************************************************************************/

#define WHERE "CCIDgetPPDUFeatures"
DWORD CCIDgetPPDUFeatures(PFEATURES pFeatures, SCARDHANDLE hCard)
{
	DWORD             dwReturn = SCARD_S_SUCCESS;
	SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
  SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};
	BYTE pbRecvBuffer[200];
	DWORD cbRecvLength = sizeof(pbRecvBuffer);

	wchar_t szReaderName[1024];
	DWORD dwReaderLen = 1024;
	DWORD dwState;
	DWORD dwProtocol;
	BYTE bAttribute[32];
	DWORD dwAtrLen = 32;

	dwReturn = SCardStatus(hCard, szReaderName, &dwReaderLen, &dwState, &dwProtocol, &bAttribute[0], &dwAtrLen);

	if ((dwReaderLen > 1024) || dwReturn != SCARD_S_SUCCESS)
	{
		return dwReturn;
	}
	//add friendlynames of readers that support PPDU over transmit here
	if ((_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"VASCO DIGIPASS 870", wcslen(L"VASCO DIGIPASS 870")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"VASCO DIGIPASS 875", wcslen(L"VASCO DIGIPASS 875")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"VASCO DIGIPASS 920", wcslen(L"VASCO DIGIPASS 920")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"VASCO DIGIPASS 876", wcslen(L"VASCO DIGIPASS 876")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"VASCO DIGIPASS 840", wcslen(L"VASCO DIGIPASS 840")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"Gemalto ING Shield Pro", wcslen(L"Gemalto ING Shield Pro")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"ETSWW eKrypto PINPhab", wcslen(L"ETSWW eKrypto PINPhab")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"ETSWW eKrypto PINPad", wcslen(L"ETSWW eKrypto PINPad")) == 0) ||
		(_wcsnicmp((wchar_t*)szReaderName, (const wchar_t*)L"DIOSS pinpad", wcslen(L"DIOSS pinpad")) == 0))
	{
		BYTE Cmd[] = { 0xFF ,0xC2 ,0x01 ,0x00 , 0x00 };
		DWORD uiCmdLg = sizeof(Cmd);

		dwReturn = SCardTransmit(hCard, &ioSendPci, Cmd, uiCmdLg, &ioRecvPci, pbRecvBuffer, &cbRecvLength);
		//Sleep(25);//checked by whitelist, goes to reader, not card
		LogTrace(LOGTYPE_TRACE, WHERE, "CCIDgetPPDUFeatures returncode: [0x%08X]", dwReturn);
		if (dwReturn == SCARD_S_SUCCESS)
		{
			if ((pbRecvBuffer[cbRecvLength - 2] == 0x90) && (pbRecvBuffer[cbRecvLength - 1] == 0x00))
			{
				BYTE bsupportedFeatureIndex = 0;
				pFeatures->USE_PPDU = 1;
				for (bsupportedFeatureIndex = 0; bsupportedFeatureIndex < (cbRecvLength - 2); bsupportedFeatureIndex++)
				{
					switch (pbRecvBuffer[bsupportedFeatureIndex])
					{
					case 0x06:
						pFeatures->VERIFY_PIN_DIRECT = 1;
						break;
					default:
						break;
					}
				}//end of for
			}
			dwReturn = SCARD_F_INTERNAL_ERROR;
		}
	}
	return dwReturn;
}
#undef WHERE

/* CCID Features */
#define WHERE "CCIDfindFeature"
DWORD CCIDfindFeature(BYTE featureTag, BYTE* features, DWORD featuresLength) {
    DWORD idx = 0;
    int count;
    while (idx < featuresLength) {
        BYTE tag = features[idx];
        idx++;
        idx++;
        if (featureTag == tag) {
            DWORD feature = 0;
            for (count = 0; count < 3; count++) {
                feature |= features[idx] & 0xff;
                idx++;
                feature <<= 8;
            }
            feature |= features[idx] & 0xff;
            return feature;
        }
        idx += 4;
    }
    return 0;
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CCIDgetFeatures"
DWORD CCIDgetFeatures(PFEATURES pFeatures, SCARDHANDLE hCard) {
	BYTE pbRecvBuffer[200];
	DWORD dwRecvLength, dwReturn;
	pFeatures->VERIFY_PIN_START = 0;
	pFeatures->VERIFY_PIN_FINISH = 0;
	pFeatures->VERIFY_PIN_DIRECT = 0;
	pFeatures->MODIFY_PIN_START = 0;
	pFeatures->MODIFY_PIN_FINISH = 0;
	pFeatures->MODIFY_PIN_DIRECT = 0;
	pFeatures->GET_KEY_PRESSED = 0;
	pFeatures->ABORT = 0;
	pFeatures->USE_PPDU = 0;

	dwReturn = SCardControl(hCard, 
		SCARD_CTL_CODE(3400),
		NULL,
		0,
		pbRecvBuffer,
		sizeof(pbRecvBuffer),
		&dwRecvLength);
	LogTrace(LOGTYPE_TRACE, WHERE, "CCIDgetFeatures returncode: [0x%08X]", dwReturn);

	if ( (SCARD_S_SUCCESS != dwReturn) || (dwRecvLength == 0) ) {
		dwReturn = CCIDgetPPDUFeatures(pFeatures,hCard);
		if ( SCARD_S_SUCCESS != dwReturn ){
			LogTrace(LOGTYPE_ERROR, WHERE, "CCIDgetFeatures errorcode: [0x%08X]", dwReturn);		
		}
		CLEANUP(dwReturn);
	}
	pFeatures->VERIFY_PIN_START = CCIDfindFeature(FEATURE_VERIFY_PIN_START, pbRecvBuffer, dwRecvLength);
	pFeatures->VERIFY_PIN_FINISH = CCIDfindFeature(FEATURE_VERIFY_PIN_FINISH, pbRecvBuffer, dwRecvLength);
	pFeatures->VERIFY_PIN_DIRECT = CCIDfindFeature(FEATURE_VERIFY_PIN_DIRECT, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_START = CCIDfindFeature(FEATURE_MODIFY_PIN_START, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_FINISH = CCIDfindFeature(FEATURE_MODIFY_PIN_FINISH, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_DIRECT = CCIDfindFeature(FEATURE_MODIFY_PIN_DIRECT, pbRecvBuffer, dwRecvLength);
	pFeatures->GET_KEY_PRESSED = CCIDfindFeature(FEATURE_GET_KEY_PRESSED, pbRecvBuffer, dwRecvLength);
	pFeatures->ABORT = CCIDfindFeature(FEATURE_ABORT, pbRecvBuffer, dwRecvLength);
cleanup:
   return (dwReturn);
}

#undef WHERE

DWORD createVerifyCommand(PPIN_VERIFY_STRUCTURE pVerifyCommand) {
		LANGUAGES displayLanguage = en;
    pVerifyCommand->bTimeOut = 30;
    pVerifyCommand->bTimeOut2 = 30;
    pVerifyCommand->bmFormatString = 0x80 | 0x08 | 0x00 | 0x01;
    /*
     * bmFormatString. 
     *  01234567
     *  10001001
     *
     * bit 7: 1 = system units are bytes
     *
     * bit 6-3: 1 = PIN position in APDU command after Lc, so just after the
     * 0x20 | pinSize.
     * 
     * bit 2: 0 = left justify data
     * 
     * bit 1-0: 01 = BCD
     */

    pVerifyCommand->bmPINBlockString = 0x47;
    /*
     * bmPINBlockString
     * 
     * bit 7-4: 4 = PIN length
     * 
     * bit 3-0: 7 = PIN block size (7 times 0xff)
     */

    pVerifyCommand->bmPINLengthFormat = 0x04;
    /*
     * bmPINLengthFormat. weird... the values do not make any sense to me.
     * 
     * bit 7-5: 0 = RFU
     * 
     * bit 4: 0 = system units are bits
     * 
     * bit 3-0: 4 = PIN length position in APDU
     */


    pVerifyCommand->wPINMaxExtraDigit = BELPIC_MIN_USER_PIN_LEN << 8 | BELPIC_MAX_USER_PIN_LEN ;
    /*
     * First byte:  maximum PIN size in digit
     * 
     * Second byte: minimum PIN size in digit
     */

    pVerifyCommand->bEntryValidationCondition = 0x02;
    /*
     * 0x02 = validation key pressed. So the user must press the green
     * button on his pinpad.
     */

    pVerifyCommand->bNumberMessage = 0x01;
    /*
     * 0x01 = message with index in bMsgIndex
     */

    pVerifyCommand->wLangId = getLangID();
    /*
     * We support multiple languages for CCID devices with LCD screen
     */

    pVerifyCommand->bMsgIndex = 0x00;
    /*
     * 0x00 = PIN insertion prompt
     */

    pVerifyCommand->bTeoPrologue[0] = 0x00;
    pVerifyCommand->bTeoPrologue[1] = 0x00;
    pVerifyCommand->bTeoPrologue[2] = 0x00;
    /*
     * bTeoPrologue : only significant for T=1 protocol.
     */

    pVerifyCommand->abData[0] = 0x00; // CLA
    pVerifyCommand->abData[1] = 0x20; // INS Verify
    pVerifyCommand->abData[2] = 0x00; // P1
    pVerifyCommand->abData[3] = 0x01; // P2
    pVerifyCommand->abData[4] = 0x08; // Lc = 8 bytes in command data
    pVerifyCommand->abData[5] = 0x20 ; // 
    pVerifyCommand->abData[6] = BELPIC_PAD_CHAR; // Pin[1]
    pVerifyCommand->abData[7] = BELPIC_PAD_CHAR; // Pin[2]
    pVerifyCommand->abData[8] = BELPIC_PAD_CHAR; // Pin[3]
    pVerifyCommand->abData[9] = BELPIC_PAD_CHAR; // Pin[4]
    pVerifyCommand->abData[10] = BELPIC_PAD_CHAR; // Pin[5]
    pVerifyCommand->abData[11] = BELPIC_PAD_CHAR; // Pin[6]
    pVerifyCommand->abData[12] = BELPIC_PAD_CHAR; // Pin[7]

    pVerifyCommand->ulDataLength = 13;

    return 0;
}
/****************************************************************************************************/
