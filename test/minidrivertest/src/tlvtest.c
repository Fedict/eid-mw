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
#include <windows.h>
#include "tlvtest.h"
#include "tlv.h"
#include "utf8.h"
#include <assert.h>

/* *****************************************************************************/

DWORD readFileBuffer(LPCTSTR lpFileName, PBYTE pbBuffer, DWORD cbBuffer) 
{
    HANDLE hFile; 
    DWORD  dwBytesRead = 0;
 
    hFile = CreateFile(lpFileName,            // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        _tprintf(TEXT("CreateFile"));
        _tprintf(TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), lpFileName);
        return 0; 
    }

    // Read one character less than the buffer size to save room for
    // the terminating NULL character. 

    if( FALSE == ReadFile(hFile, pbBuffer, cbBuffer-1, &dwBytesRead, NULL) )
    {
        _tprintf(TEXT("ReadFile"));
        _tprintf(TEXT("Terminal failure: Unable to read from file.\n"));
        CloseHandle(hFile);
        return 0;
    }
	 CloseHandle(hFile);
	 return dwBytesRead;
}


DWORD TLVGetFieldTest()
{
	BYTE pbBuffer[1024];
	BYTE pbFile[4096];
	DWORD cbBuffer = sizeof(pbBuffer);
	DWORD cbFile = sizeof(pbFile);
	DWORD dwDataLen;
	BYTE pbChipNumber[] = { 0x53, 0x4c, 0x49, 0x4e, 0x33, 0x66, 0x00, 0x29, 0x6c, 0xff, 0x29, 0x3b, 0x2a, 0x12, 0x22, 0x20 };
	BYTE pbPhotoHash[] = { 0x28, 0xf3, 0xec, 0x53, 0x21, 0x02, 0xB0, 0xcc, 0xd9, 0x3c, 0x7b, 0xdf, 0x58, 0xed, 0x83, 0x40, 0x0a, 0x89, 0x97, 0x90 };

	cbFile = readFileBuffer(TEXT("..\\..\\src\\res\\alice_id"), pbFile, cbFile);

	assert(cbFile != 0);

	// ID CARD NUMBER
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_CARD_NUMBER);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("000000122662"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_CARD_NUMBER);
	assert(dwDataLen == strlen("000000122662"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "000000122662", dwDataLen));

	// ID CHIP NUMBER
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_CHIP_NUMBER);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == sizeof(pbChipNumber));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_CHIP_NUMBER);
	assert(dwDataLen == sizeof(pbChipNumber));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, pbChipNumber, dwDataLen));

	// ID CARD VALIDITIY BEGIN
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_CARD_VALIDITY_BEGIN);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("23.07.2009"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_CARD_VALIDITY_BEGIN);
	assert(dwDataLen == strlen("23.07.2009"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "23.07.2009", dwDataLen));

	// ID CARD VALIDTIY END
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_CARD_VALIDITY_END);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("23.07.2014"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_CARD_VALIDITY_END);
	assert(dwDataLen == strlen("23.07.2014"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "23.07.2014", dwDataLen));

	// ID CARD DELIVERY MUNICIPALITY
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_CARD_DELIVIRY_MUNICIPALITY);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("Antwerpen")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_CARD_DELIVIRY_MUNICIPALITY);
	assert(dwDataLen == strlen(utf8("Antwerpen")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("Antwerpen"), dwDataLen));

	// ID NATIONAL NUMBER
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_NATIONAL_NUMBER);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("71715100070"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_NATIONAL_NUMBER);
	assert(dwDataLen == strlen("71715100070"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "71715100070", dwDataLen));

	// ID NAME
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_NAME);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("SPECIMEN")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_NAME);
	assert(dwDataLen == strlen(utf8("SPECIMEN")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("SPECIMEN"), dwDataLen));

	// ID FIRST TWO GIVEN NAMES
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_FIRST_TWO_GIVEN_NAMES);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("Alice Geldigekaart")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_FIRST_TWO_GIVEN_NAMES);
	assert(dwDataLen == strlen(utf8("Alice Geldigekaart")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("Alice Geldigekaart"), dwDataLen));

	// ID FIRST LETTER THIRD GIVEN NAME 
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_FIRST_LETTER_THIRD_GIVEN_NAME);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("A")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_FIRST_LETTER_THIRD_GIVEN_NAME);
	assert(dwDataLen == strlen(utf8("A")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("A"), dwDataLen));

	// ID NATIONALITY
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_NATIONALITY);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("BELG")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_NATIONALITY);
	assert(dwDataLen == strlen(utf8("BELG")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("BELG"), dwDataLen));

	// ID BIRTH LOCATION
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_BIRTH_LOCATION);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("Hamont-Achel")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_BIRTH_LOCATION);
	assert(dwDataLen == strlen(utf8("Hamont-Achel")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("Hamont-Achel"), dwDataLen));

	// ID BIRTH DATE
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_BIRTH_DATE);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("01 JAN 1971")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_BIRTH_DATE);
	assert(dwDataLen == strlen(utf8("01 JAN 1971")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("01 JAN 1971"), dwDataLen));

	// ID SEX
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_SEX);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("V"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_SEX);
	assert(dwDataLen == strlen("V"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "V", dwDataLen));

	// ID NOBLE CONDITION
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_NOBLE_CONDITION);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_NOBLE_CONDITION);
	assert(dwDataLen == strlen(utf8("")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8(""), dwDataLen));

	// ID DOCUMENT TYPE
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_DOCUMENT_TYPE);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("1"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_DOCUMENT_TYPE);
	assert(dwDataLen == strlen("1"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "1", dwDataLen));

	// ID SPECIAL STATUS
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_SPECIAL_STATUS);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("0"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_SPECIAL_STATUS);
	assert(dwDataLen == strlen("0"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "0", dwDataLen));

   // ID PHOTO HASH
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_PHOTO_HASH);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == sizeof(pbPhotoHash));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_PHOTO_HASH);
	assert(dwDataLen == sizeof(pbPhotoHash));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, pbPhotoHash, dwDataLen));


	cbFile = readFileBuffer(TEXT("..\\..\\src\\res\\alice_address"), pbFile, cbFile);

	assert(cbFile != 0);

	// ADDRESS STREET & NUMBER
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_ADDRESS_STREET_NUMBER);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("Meirplaats 1 bus 1")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_ADDRESS_STREET_NUMBER);
	assert(dwDataLen == strlen(utf8("Meirplaats 1 bus 1")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("Meirplaats 1 bus 1"), dwDataLen));

	// ADDRESS ZIP CODE
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_ADDRESS_ZIP);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen("2000"));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_ADDRESS_ZIP);
	assert(dwDataLen == strlen("2000"));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, "2000", dwDataLen));

	// ADDRESS MUNICIPALITY
	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, NULL, 0, &dwDataLen, ID_ADDRESS_MUNICIPALITY);
	printf("dwDataLen: %d\n", dwDataLen);
	assert(dwDataLen == strlen(utf8("Antwerpen")));

	dwDataLen = 0;
	TLVGetField(pbFile, cbFile, pbBuffer, cbBuffer, &dwDataLen, ID_ADDRESS_MUNICIPALITY);
	assert(dwDataLen == strlen(utf8("Antwerpen")));
	printf("dwDataLen: %d\n", dwDataLen);
	assert(0 == memcmp(pbBuffer, utf8("Antwerpen"), dwDataLen));

	return 0;

}
