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
#ifdef WIN32
#include <win32.h>
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
#else
#include <unix.h>
#include <pkcs11.h>
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "testlib.h"


#if HAVE_JPEGLIB
#include <jpeglib.h>

enum weights {
	TOPLEFT,
	TOPCENTER,
	TOPRIGHT,
	CENTLEFT,
	CENTER,
	CENTRIGHT,
	BOTLEFT,
	BOTCENTER,
	BOTRIGHT,
	SLASH,
	BACKSLASH,
};

enum duty {
	NONE,
	LIGHT,
	MED,
	HEAVY,
};

static int orientations[16] = {
		   //br bl tr tl
	CENTER,    // 0  0  0  0
	TOPLEFT,   // 0  0  0  1
	TOPRIGHT,  // 0  0  1  0
	TOPCENTER, // 0  0  1  1
	BOTLEFT,   // 0  1  0  0
	CENTLEFT,  // 0  1  0  1
	SLASH,     // 0  1  1  0
	TOPLEFT,   // 0  1  1  1
	BOTRIGHT,  // 1  0  0  0
	BACKSLASH, // 1  0  0  1
	CENTRIGHT, // 1  0  1  0
	TOPRIGHT,  // 1  0  1  1
	BOTCENTER, // 1  1  0  0
	BOTLEFT,   // 1  1  0  1
	BOTRIGHT,  // 1  1  1  0
	CENTER,    // 1  1  1  1
};

static char translate[4][11] = {
	{ ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ', ' ', ' ', },
	{ 0x60,'\'', 0x27,  '>',  '-',  '<',  ',',  '_',  '.', '/', '\\', },
	{ '"',  '?',  '"',  '[',  '*',  ']',  'b',  'o',  'd', '/', '\\', },
	{ 'F',  'V',  '$',  '#',  '@',  '#',  '&',  'W',  'Q', '/', '\\' },
};

int jpegdump(char* data, CK_ULONG length) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY imgbuf;
	JDIMENSION size = 0;
	int rlen, j;
	unsigned int i;

	/* initialize JPEG decompression */
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (unsigned char*)data, length);

	if(jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
		printf("Could not read JPEG header\n");
		return TEST_RV_FAIL;
	}

	if(!jpeg_start_decompress(&cinfo)) {
		printf("Could not decompress JPEG data\n");
		return TEST_RV_FAIL;
	}

	printf("image has %d byte(s) per pixel\n", cinfo.output_components);
	rlen = cinfo.output_width * cinfo.output_components;
	imgbuf = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, rlen, cinfo.output_height);
	while(size != cinfo.output_height) {
		size += jpeg_read_scanlines(&cinfo, &(imgbuf[size]), cinfo.output_height - size);
	}

	printf("Read %u scanlines\n", size);

	for(i=0; i<cinfo.output_height; i+=2) {
		for(j=0; j<rlen; j+=2) {
			int p[4]; // pixel
			int d[4], dt; // duty
			int k, m, or;
			p[0] = 255 - (imgbuf[i][j]);
			p[1] = 255 - (imgbuf[i][j+1]);
			p[2] = 255 - (imgbuf[i+1][j]);
			p[3] = 255 - (imgbuf[i+1][j+1]);
			or = 0;
			for(k=0, m=0; k<4; k++) {
				d[k] = p[k] >> 6;
				if(d[k] > m) {
					m = d[k];
					or = 1 << k;
				} else if(d[k] == m) {
					or |= 1 << k;
				}
			}
			dt = (p[0] + p[1] + p[2] + p[3]) >> 8;
			or = orientations[or];
			printf("%c", translate[dt][or]);
		}
		printf("\n");
	}
	return TEST_RV_OK;
}
#endif

TEST_FUNC(decode_photo) {
	CK_SESSION_HANDLE session;
	CK_OBJECT_HANDLE object;
	CK_ULONG count;
	CK_ULONG type;
	CK_SLOT_ID slot;
	CK_ATTRIBUTE attr[2];
	int ret;

	if(!can_confirm()) {
		printf("Need the ability to read privacy-sensitive data from the card for this test...\n");

		return TEST_RV_SKIP;
	}

	check_rv(C_Initialize(NULL_PTR));

	if((ret = find_slot(CK_TRUE, &slot)) != TEST_RV_OK) {
		check_rv(C_Finalize(NULL_PTR));
		return ret;
	}

	check_rv(C_OpenSession(slot, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session));

	attr[0].type = CKA_CLASS;
	attr[0].pValue = &type;
	type = CKO_DATA;
	attr[0].ulValueLen = sizeof(CK_ULONG);

	attr[1].type = CKA_LABEL;
	attr[1].pValue = (void*)("PHOTO_FILE");
	attr[1].ulValueLen = strlen("PHOTO_FILE");

	check_rv(C_FindObjectsInit(session, attr, 2));

	do {
		char* label_str;
		char* value_str;
		char* objid_str;
		CK_ATTRIBUTE data[3] = {
			{CKA_LABEL, NULL_PTR, 0},
			{CKA_VALUE, NULL_PTR, 0},
			{CKA_OBJECT_ID, NULL_PTR, 0},
		};
		FILE* f;

		check_rv(C_FindObjects(session, &object, 1, &count));
		if(!count) continue;

		check_rv(C_GetAttributeValue(session, object, data, 3));

		label_str = malloc(data[0].ulValueLen + 1);
		data[0].pValue = label_str;

		value_str = malloc(data[1].ulValueLen + 1);
		data[1].pValue = value_str;

		objid_str = malloc(data[2].ulValueLen + 1);
		data[2].pValue = objid_str;

		check_rv(C_GetAttributeValue(session, object, data, 3));

		label_str[data[0].ulValueLen] = '\0';
		value_str[data[1].ulValueLen] = '\0';
		objid_str[data[2].ulValueLen] = '\0';

		f = fopen("foto.jpg", "wb+");
		if(f) {
			fwrite(value_str, 1, data[1].ulValueLen, f);
			fclose(f);
		}
#if HAVE_JPEGLIB
		printf("Data object with object ID: %s; label: %s; length: %lu\nContents(ASCII art representation):\n", objid_str, label_str, data[1].ulValueLen);
		if((ret = jpegdump(value_str, data[1].ulValueLen)) != TEST_RV_OK) {
			return ret;
		}
#else
		printf("Data object with object ID: %s; label: %s; length: %lu\nContents(hexdump):\n", objid_str, label_str, data[1].ulValueLen);
		hex_dump(value_str, data[1].ulValueLen);
#endif

		free(label_str);
		free(value_str);
	} while(count);

	check_rv(C_CloseSession(session));

	check_rv(C_Finalize(NULL_PTR));

	return TEST_RV_OK;
}
