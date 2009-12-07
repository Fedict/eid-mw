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
#ifndef __SMARTCARD_H__
#define __SMARTCARD_H__

extern DWORD   BeidAuthenticate
               (
                  PCARD_DATA     pCardData, 
                  PBYTE          pbPin, 
                  DWORD          cbPin, 
                  PDWORD         pcAttemptsRemaining
               );

extern DWORD   BeidDeAuthenticate
               (
                  PCARD_DATA     pCardData
               ); 

extern DWORD   BeidMSE
               (
                  PCARD_DATA     pCardData,             
                  DWORD          dwRole
               ) ;

extern DWORD   BeidChangePIN
               (
                  PCARD_DATA     pCardData, 
                  PBYTE          pbCurrentAuthenticator,
                  DWORD          cbCurrentAuthenticator,
                  PBYTE          pbNewAuthenticator,
                  DWORD          cbNewAuthenticator,
                  PDWORD         pcAttemptsRemaining
               );

extern DWORD BeidGetCardSN(PCARD_DATA  pCardData, unsigned int iSerNumLg, unsigned char *pa_cSerNum);
extern DWORD BeidSignData(PCARD_DATA  pCardData, unsigned int HashAlgo, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature);
extern DWORD BeidReadFile(PCARD_DATA  pCardData, DWORD dwOffset, DWORD *cbStream, PBYTE pbStream);
extern DWORD BeidReadCert(PCARD_DATA  pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif);

#endif