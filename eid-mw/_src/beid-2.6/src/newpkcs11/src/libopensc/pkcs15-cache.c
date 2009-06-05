/*
 * pkcs15-cache.c: PKCS #15 file caching functions
 *
 * Copyright (C) 2001, 2002  Juha Yrjölä <juha.yrjola@iki.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "internal.h"
#include "pkcs15.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include "openssl/evp.h"
#include "openssl/aes.h"

static int lookup_file(struct sc_pkcs15_card *p15card,
	const struct sc_path *path, int offset,
	int *action, u8 **buf, size_t *bufsize);


#define MAX_KNOWN_INTERFACE_VERSION  0


#define TYPE_PERSO_DEPENDENT    0x01    /* identical for all cards with the same perso version */
#define TYPE_FIXED_PER_CARD     0x02    /* card-dependend but fixed (e.g. picture) */
#define TYPE_DO_CHECK           0x04    /* if the contents need to be checked (certs, signatures) */
#define TYPE_DO_LOOKUP          0x08    /* lookup in a small amount of cache files */
#define FLAG_CRYPT              0x1000  /* encrypt contents (key = last 16 bytes of ID signature) */

/* Harcoded data, used if type = TYPE_PERSO_DEPENDENT */
typedef struct {
	u8 persoversion;
	const u8 *data;
	int data_len;
} belpic_file_content;

const static u8 odf[] = {
	0xA0,0x0A,0x30,0x08,0x04,0x06,0x3F,0x00,0xDF,0x00,0x50,0x35,0xA4,0x0A,0x30,0x08,
	0x04,0x06,0x3F,0x00,0xDF,0x00,0x50,0x37,0xA8,0x0A,0x30,0x08,0x04,0x06,0x3F,0x00,
	0xDF,0x00,0x50,0x34};
const belpic_file_content odf_contents[] = {
	{1, odf, sizeof(odf)},
	{2, odf, sizeof(odf)},
	{0, NULL, 0}
};

const static u8 aodf_5034_1[] = {
	0x30,0x33,0x30,0x0F,0x0C,0x09,0x42,0x61,0x73,0x69,0x63,0x20,0x50,0x49,0x4E,0x03,
	0x02,0x06,0xC0,0x30,0x03,0x04,0x01,0x01,0xA1,0x1B,0x30,0x19,0x03,0x02,0x02,0x0C,
	0x0A,0x01,0x00,0x02,0x01,0x04,0x02,0x01,0x08,0x80,0x01,0x01,0x04,0x01,0xFF,0x30,
	0x04,0x04,0x02,0x3F,0x00};
const belpic_file_content aodf_contents[] = {
	{1, aodf_5034_1, sizeof(aodf_5034_1)},
	{0, NULL, 0}
};

const static u8 prkdf_5035_1[] = {
	0x30,0x3A,0x30,0x17,0x0C,0x0E,0x41,0x75,0x74,0x68,0x65,0x6E,0x74,0x69,0x63,0x61,
	0x74,0x69,0x6F,0x6E,0x03,0x02,0x06,0xC0,0x04,0x01,0x01,0x30,0x0F,0x04,0x01,0x02,
	0x03,0x02,0x05,0x20,0x03,0x02,0x03,0xB8,0x02,0x02,0x00,0x82,0xA1,0x0E,0x30,0x0C,
	0x30,0x06,0x04,0x04,0x3F,0x00,0xDF,0x00,0x02,0x02,0x04,0x00,0x30,0x39,0x30,0x15,
	0x0C,0x09,0x53,0x69,0x67,0x6E,0x61,0x74,0x75,0x72,0x65,0x03,0x02,0x06,0xC0,0x04,
	0x01,0x01,0x02,0x01,0x01,0x30,0x10,0x04,0x01,0x03,0x03,0x03,0x06,0x00,0x40,0x03,
	0x02,0x03,0xB8,0x02,0x02,0x00,0x83,0xA1,0x0E,0x30,0x0C,0x30,0x06,0x04,0x04,0x3F,
	0x00,0xDF,0x00,0x02,0x02,0x04,0x00};
const belpic_file_content prkdf_contents[] = {
	{1, prkdf_5035_1, sizeof(prkdf_5035_1)},
	{0, NULL, 0}
};

const static u8 cdf_5037_1[] = {
	0x30,0x2C,0x30,0x17,0x0C,0x0E,0x41,0x75,0x74,0x68,0x65,0x6E,0x74,0x69,0x63,0x61,
	0x74,0x69,0x6F,0x6E,0x03,0x02,0x06,0xC0,0x04,0x01,0x01,0x30,0x03,0x04,0x01,0x02,
	0xA1,0x0C,0x30,0x0A,0x30,0x08,0x04,0x06,0x3F,0x00,0xDF,0x00,0x50,0x38,0x30,0x27,
	0x30,0x12,0x0C,0x09,0x53,0x69,0x67,0x6E,0x61,0x74,0x75,0x72,0x65,0x03,0x02,0x06,
	0xC0,0x04,0x01,0x01,0x30,0x03,0x04,0x01,0x03,0xA1,0x0C,0x30,0x0A,0x30,0x08,0x04,
	0x06,0x3F,0x00,0xDF,0x00,0x50,0x39,0x30,0x23,0x30,0x0B,0x0C,0x02,0x43,0x41,0x03,
	0x02,0x06,0xC0,0x04,0x01,0x01,0x30,0x06,0x04,0x01,0x04,0x01,0x01,0xFF,0xA1,0x0C,
	0x30,0x0A,0x30,0x08,0x04,0x06,0x3F,0x00,0xDF,0x00,0x50,0x3A,0x30,0x25,0x30,0x0D,
	0x0C,0x04,0x52,0x6F,0x6F,0x74,0x03,0x02,0x06,0xC0,0x04,0x01,0x01,0x30,0x06,0x04,
	0x01,0x06,0x01,0x01,0xFF,0xA1,0x0C,0x30,0x0A,0x30,0x08,0x04,0x06,0x3F,0x00,0xDF,
	0x00,0x50,0x3B};
const belpic_file_content cdf_contents[] = {
	{1, cdf_5037_1, sizeof(cdf_5037_1)},
	{0, NULL, 0}
};

typedef struct {
	const u8 *path;
	int path_len;
	int type;
	int offset;                           /* used for TYPE_DO_LOOKUP */
	const belpic_file_content *contents;  /* used for TYPE_PERSO_DEPENDENT */ 
} belpic_file_info;

const static belpic_file_info belpic_file_infos[] = {
	{"\xDF\x00\x50\x31", 4, TYPE_PERSO_DEPENDENT, 0, odf_contents},        /* EF(ODF) */
	{"\xDF\x00\x50\x34", 4, TYPE_PERSO_DEPENDENT, 0, aodf_contents},       /* EF(AODF) */
	{"\xDF\x00\x50\x35", 4, TYPE_PERSO_DEPENDENT, 0, prkdf_contents},      /* EF(PrKDF) */
	{"\xDF\x00\x50\x37", 4, TYPE_PERSO_DEPENDENT, 0, cdf_contents},        /* EF(CDF) */
	{"\xDF\x00\x50\x38", 4, TYPE_DO_CHECK, 0, NULL},                       /* Auth cert */
	{"\xDF\x00\x50\x39", 4, TYPE_DO_CHECK, 0, NULL},                       /* Nonrep cert */
	{"\xDF\x00\x50\x3A", 4, TYPE_DO_LOOKUP, 870, NULL},                       /* Citizen CA cert */
	{"\xDF\x00\x50\x3B", 4, TYPE_DO_LOOKUP, 840, NULL},                       /* Root CA cert */
	{"\xDF\x00\x50\x3C", 4, TYPE_DO_LOOKUP, 790, NULL},                    /* RRN cert */
	{"\xDF\x00\x50\x3D", 4, TYPE_FIXED_PER_CARD, 0, NULL},                 /* ID CA cert */
	{"\xDF\x01\x40\x31", 4, TYPE_FIXED_PER_CARD | FLAG_CRYPT, 0, NULL},    /* ID */
	{"\xDF\x01\x40\x32", 4, TYPE_DO_CHECK, 0, NULL},                       /* ID signature */
	{"\xDF\x01\x40\x35", 4, TYPE_FIXED_PER_CARD | FLAG_CRYPT, 0, NULL},    /* photo */
	{NULL, 0, 0, 0, NULL}
};

#ifdef _WIN32
#pragma pack(1)
#define __PACKED
#else
#define __PACKED __attribute__((__packed__))
#endif

#define HEADER_VERSION  0x10
typedef struct {
	u8 version;   /* currently 0x10 */
	u8 crc[4];    /* checksum over the contents */
	u8 iv[8];     /* IV for encryption */
	u8 offset[2]; /* check the contents at this offset, if needed */
	u8 rfu[5];    /* set to 0 for this version */
} __PACKED cache_header;

sc_path_t photo_path = {
	{0x3F, 0x00, 0xDF, 0x01, 0x40, 0x35},
	6,
	0,
	-1,
	SC_PATH_TYPE_PATH
};

const static unsigned long crc_table[256];

/* Get 16 bytes of an EF at offset 'offset' */
static int get_file_part(sc_pkcs15_card_t *p15card,
	const sc_path_t *path, int offset, u8 *file_part);

static int cache_crypt(struct sc_pkcs15_card *p15card,
	cache_header *header, const u8* datain, u8 *dataout, int datalen);

static char cachedir[PATH_MAX];
static int cachedir_known = 0;

/* If p15card == NULL, then filename = <path>_<persoversion>.bin  (for TYPE_PERSO_DEPENDENT)
 * If p15card != NULL, then filename = <serialnr>_<path>.bin
 */
static int generate_cache_filename(struct sc_pkcs15_card *p15card,
				   u8 persoversion, int fileindex,
				   const struct sc_path *path,
				   char *buf, size_t bufsize)
{
	char pathname[SC_MAX_PATH_SIZE*2+1];
	int  r;
	const u8 *pathptr;
	size_t i, pathlen;

	if (path->type != SC_PATH_TYPE_PATH)
		return SC_ERROR_INVALID_ARGUMENTS;
	assert(path->len <= SC_MAX_PATH_SIZE);

	if (!cachedir_known) {
		r = sc_get_cache_dir(cachedir, sizeof(cachedir));
		if (r)
			return r;
		cachedir_known = 1;
	}

	pathptr = path->value;
	pathlen = path->len;
	if (pathlen > 2 && memcmp(pathptr, "\x3F\x00", 2) == 0) {
		pathptr += 2;
		pathlen -= 2;
	}
	for (i = 0; i < pathlen; i++)
		sprintf(pathname + 2*i, "%02X", pathptr[i]);

	if (p15card == NULL)
		r = snprintf(buf, bufsize, "%s/%s_%d.bin", cachedir, pathname,
		    fileindex == -1 ? persoversion : fileindex);
	else
		r = snprintf(buf, bufsize, "%s/%s_%s.bin", cachedir, p15card->serial_number, pathname);
	if (r < 0)
		return SC_ERROR_BUFFER_TOO_SMALL;

	return 0;
}

static void make_header(cache_header *header, const u8 *iv, const u8 *contents, size_t contentslen)
{
	unsigned long crc = 0xFFFFFFFF;
	int i;

	header->version = HEADER_VERSION;

	for (i = 0; i < contentslen; i++)
		crc = crc_table[(crc ^ contents[i]) & 0xff] ^ (crc >> 8);
	crc ^= 0xFFFFFFFF;
	for (i = 3; i >= 0; i--) {
		header->crc[i] = 0xFF & crc;
		crc >>= 8;
	}

	if (iv)
		memcpy(header->iv, iv, sizeof(header->iv));

	/* Usually we check at the end of the contents, but if it is padded
	 * with zeros, we move towards the start */
	for (i = contentslen - 1; i >= FILE_PART_LEN; i--) {
		if (contents[i] != 0)
			break;
	}
	i -= FILE_PART_LEN;
	header->offset[0] = (u8) (i / 256);
	header->offset[1] = (u8) (i % 256);
}

/* Check header version + CRC, return 0 if OK */
static int check_header(struct sc_pkcs15_card *p15card, const char *fname,
	cache_header *header, const u8 *contents, size_t contentslen)
{
	unsigned long crc = 0xFFFFFFFF;
	int i;

	/* Incompatible versions have a different most significant nibble */
	if ((header->version & 0xF0) != (HEADER_VERSION & 0xF0)) {
		sc_error(p15card->card->ctx, "Unsupported cache file version (%d) for file \"%s\"\n",
			header->version, fname);
		return SC_ERROR_NOT_SUPPORTED;
	}

	for (i = 0; i < contentslen; i++)
		crc = crc_table[(crc ^ contents[i]) & 0xff] ^ (crc >> 8);
	crc ^= 0xFFFFFFFF;

	for (i = 3; i >= 0; i--) {
		if (header->crc[i] != (0xFF & crc)) {
			sc_error(p15card->card->ctx, "Wrong checksum for file \"%s\"\n", fname);
			return SC_ERROR_OBJECT_NOT_VALID;
		}
		crc >>= 8;
	}

	return 0;
}

static int read_file_contents(FILE *f, cache_header *header, u8 *data, int count)
{
	int got;

	got = fread(header, 1, sizeof(cache_header), f);
	if (got != sizeof(cache_header))
		return SC_ERROR_BUFFER_TOO_SMALL;

	got = fread(data, 1, count, f);
	if (got != count)
		return SC_ERROR_BUFFER_TOO_SMALL;

	return 0;
}

int sc_pkcs15_read_cached_file(struct sc_pkcs15_card *p15card,
			       const struct sc_path *path, int *action,
			       u8 **buf, size_t *bufsize)
{
	const belpic_file_info *fi;
	const belpic_file_content *contents;
	int type;
	cache_header header;
	char fname[PATH_MAX];
	int offset;
	int r;
	FILE *f;
	size_t count, got;
	struct stat stbuf;
	u8 *data = NULL;

	*action = 0;

	/* We only do caching for known perso interface versions */
	if (p15card->persointerfaceversion > MAX_KNOWN_INTERFACE_VERSION)
		return SC_ERROR_NOT_SUPPORTED;

	/* See if we do 'something cacheable' with the file */
	for (fi = &belpic_file_infos[0]; fi->path != NULL; fi++) {
		if ((path->len == fi->path_len + 2) && !memcmp(path->value + 2, fi->path, fi->path_len)) {
			type = fi->type;
			contents = fi->contents;
			offset = fi->offset;
			break;
		}
	}
	if (fi->path == NULL)
		return SC_ERROR_FILE_NOT_FOUND;

	/* See if the file's contents are hardcoded */
	if ((type & TYPE_PERSO_DEPENDENT) && (contents != NULL)) {
		for ( ; contents->data != NULL; contents++) {
			if (p15card->persoversion != contents->persoversion)
				continue;
			/* OK, hardcoded contents found */
			if (*buf == NULL) {
				*buf = (u8 *) malloc(contents->data_len);
				if (*buf == NULL)
					return SC_ERROR_OUT_OF_MEMORY;
			}
			else if (contents->data_len > *bufsize)
					return SC_ERROR_BUFFER_TOO_SMALL;
			*bufsize = contents->data_len;
			memcpy(*buf, contents->data, *bufsize);

			return 0;
		}
	}

	/* Not hardcoded */

	r = generate_cache_filename(
		(type & TYPE_PERSO_DEPENDENT) || (type & TYPE_DO_LOOKUP) ? NULL : p15card,
		p15card->persoversion, type & TYPE_DO_LOOKUP ? 0 : -1,
		path, fname, sizeof(fname));
	if (r != 0)
		return r;
	*action = ACTION_CACHE;

	r = stat(fname, &stbuf);
	if (r)
		return SC_ERROR_FILE_NOT_FOUND;

	if (path->count < 0)
		count = stbuf.st_size - sizeof(header);
	else
		return SC_ERROR_FILE_NOT_FOUND; /* not supported */

	/* This type requires a whole different way to deal with */
    if (type & TYPE_DO_LOOKUP)
        return lookup_file(p15card, path, offset, action, buf, bufsize);

	if (count < FILE_PART_LEN)
		return SC_ERROR_OBJECT_NOT_VALID;

	if (*buf == NULL) {
		data = (u8 *) malloc(count);
		if (data == NULL)
			return SC_ERROR_OUT_OF_MEMORY;

	} else
		if (count > *bufsize)
			return SC_ERROR_BUFFER_TOO_SMALL;

	f = fopen(fname, "rb");
	if (f == NULL) {
		r = SC_ERROR_FILE_NOT_FOUND;
		goto error;
	}
	r = read_file_contents(f, &header, data, count);
	fclose(f);
	if (r != 0)
		goto error;
	r = check_header(p15card, fname, &header, data, count);
	if (r != 0)
		goto error;

	if (type & TYPE_DO_CHECK) {
		u8 file_end[FILE_PART_LEN];
		int offset = (int) header.offset[0] * 256 + header.offset[1];
		if (offset + FILE_PART_LEN > count) {
			r = SC_ERROR_OBJECT_NOT_VALID;
			goto error;
		}
		r = get_file_part(p15card, path, offset, file_end);
		if (r < 0)
			goto error;
		if ((r != FILE_PART_LEN) || memcmp(data + offset, file_end, FILE_PART_LEN)) {
			r = SC_ERROR_OBJECT_NOT_VALID;
			goto error;
		}
	}

	if (type & FLAG_CRYPT) {
		r = cache_crypt(p15card, &header, data, data, count);
		if (r < 0)
			goto error;
	}

	*action = 0;
	*buf = data;
	*bufsize = count;
	if (data)
		*buf = data;
	return 0;

error:
	if (data)
		free(data);
	return r;
}

int sc_pkcs15_cache_file(struct sc_pkcs15_card *p15card,
			 const struct sc_path *path,
			 const u8 *buf, size_t bufsize)
{
	const belpic_file_info *fi;
	int type;
	cache_header header;
	char fname[PATH_MAX];
	int fileindex = -1;
	int r;
	FILE *f;
	size_t c;
	u8 *encrypted = NULL;

	if (bufsize < FILE_PART_LEN)
		return SC_ERROR_OBJECT_NOT_VALID;

	/* See if we do 'something cacheable' with the file */
	for (fi = &belpic_file_infos[0]; fi->path != NULL; fi++) {
		if ((path->len == fi->path_len + 2) && !memcmp(path->value + 2, fi->path, fi->path_len)) {
			type = fi->type;
			break;
		}
	}
	if (fi->path == NULL)
		return SC_ERROR_FILE_NOT_FOUND;

	if (type & TYPE_DO_LOOKUP) {
		for (fileindex = 0 ; fileindex < 1000; fileindex++) {
			struct stat stbuf;
			r = generate_cache_filename(
				(type & TYPE_PERSO_DEPENDENT) || (type & TYPE_DO_LOOKUP) ? NULL : p15card,
				p15card->persoversion, fileindex,
				path, fname, sizeof(fname));
			if (r != 0)
				return r;
			r = stat(fname, &stbuf);
			if (r)
				break; /* file with this fileindex doesn't exist yet */
		}
	}

	r = generate_cache_filename(
		(type & TYPE_PERSO_DEPENDENT) || (type & TYPE_DO_LOOKUP) ? NULL : p15card,
		p15card->persoversion, fileindex,
		path, fname, sizeof(fname));
	if (r != 0)
		return r;

	memset(&header, 0, sizeof(header));

	if (type & FLAG_CRYPT) {
		encrypted = (u8 *) malloc(bufsize);
		if (encrypted == NULL)
			return SC_ERROR_OUT_OF_MEMORY;
		RAND_pseudo_bytes(header.iv, sizeof(header.iv));
		r = cache_crypt(p15card, &header, buf, encrypted, bufsize);
		if (r)
			goto error;
		buf = encrypted;
	}

	f = fopen(fname, "wb");
	/* If the open failed because the cache directory does
	 * not exist, create it and a re-try the fopen() call.
	 */
	if (f == NULL && errno == ENOENT) {
		if ((r = sc_make_cache_dir()) < 0)
			goto error;
		f = fopen(fname, "wb");
	}
	if (f == NULL) {
		r = SC_ERROR_INTERNAL;
		goto error;
	}

	make_header(&header, NULL, buf, bufsize);

	c = fwrite(&header, 1, sizeof(header), f);
	if (c == sizeof(header))
		c += fwrite(buf, 1, bufsize, f);
	fclose(f);
	if (c != (bufsize + sizeof(header))) {
		sc_error(p15card->card->ctx, "fwrite() failed: wrote only %d bytes", c);
		unlink(fname);
		r = SC_ERROR_INTERNAL;
		goto error;
	}
	r = 0;

error:
	if (encrypted)
		free(encrypted);
	return r;
}

/* If p15card == NULL -> deleted add pkcs15 files (the ones with TYPE_PERSO_DEPENDENT),
 * otherwise delete all card-dependent data (certs, photo, ID, signatures) */ 
int sc_pkcs15_cache_clear(struct sc_pkcs15_card *p15card)
{
	const belpic_file_info *fi;
	char fname[PATH_MAX];
	sc_path_t path;
	int i, r, ret = 0, endloop;

	path.type = SC_PATH_TYPE_PATH;

	if (p15card == NULL) {
		for (fi = &belpic_file_infos[0]; fi->path != NULL; fi++) {
			if ((fi->type == TYPE_FIXED_PER_CARD) || (fi->type == TYPE_DO_CHECK))
				continue;
			endloop = (fi->type == TYPE_DO_LOOKUP) ? 100 : 20;
			for (i = 0; i < endloop; i++) {
				memcpy(path.value, fi->path, fi->path_len);
				path.len = fi->path_len;
				r = generate_cache_filename(NULL, i, -1, &path, fname, sizeof(fname));
				if (r == 0)
					unlink(fname);
			}
		}
	}
	else {
		for (fi = &belpic_file_infos[0]; fi->path != NULL; fi++) {
			if ((fi->type == TYPE_PERSO_DEPENDENT) || (fi->type == TYPE_DO_LOOKUP))
				continue;
			memcpy(path.value, fi->path, fi->path_len);
			path.len = fi->path_len;
			r = generate_cache_filename(p15card, p15card->persoversion, -1, &path,
				fname, sizeof(fname));
			if (r != 0) {
				ret = r;
				continue;
			}

			for (i = 0; i < 2; i++) {
				r = unlink(fname);
				if ((r == EBUSY) && (i == 0)) { /* busy -> wait 0,5 sec and try again */
#ifdef _WIN32
					Sleep(500);
#else
					usleep(500000);
#endif
					continue;
				}
				if ((r != 0) && (errno != ENOENT)) {
					sc_error(p15card->card->ctx, "unlink-ing \"%s\" failed: %d\n", fname, r);
					ret = r;
				}
				break;
			}
		}
	}

	return ret;
}

static int lookup_file(struct sc_pkcs15_card *p15card,
	const struct sc_path *path, int offset,
	int *action, u8 **buf, size_t *bufsize)
{
	FILE *f;
	cache_header header;
	u8 from_card[FILE_PART_LEN];
	u8 temp[FILE_PART_LEN];
	char fname[PATH_MAX];
	struct stat stbuf;
	u8 *data = NULL;
	int i, count;
	int r;

	*action = ACTION_CACHE;

	r = get_file_part(p15card, path, offset, from_card);
	if (r != FILE_PART_LEN)
		return r < 0 ? r : SC_ERROR_OBJECT_NOT_VALID;

	/* Search in all cached files (for this path) for one that
	 * matches the 16 bytes read */
	for (i = 0; i < 1000; i++) {
		r = generate_cache_filename(
			NULL,
			p15card->persoversion, i,
			path, fname, sizeof(fname));
		if (r != 0)
			return r;

		r = stat(fname, &stbuf);
		if (r)
			return SC_ERROR_FILE_NOT_FOUND;
		count = stbuf.st_size - sizeof(header);
		f = fopen(fname, "rb");
		if (f == NULL)
			continue;

		fseek(f, (long) (sizeof(header) + offset), SEEK_SET);
		r = fread(temp, 1, FILE_PART_LEN, f);
		if ((r != FILE_PART_LEN) || memcmp(temp, from_card, FILE_PART_LEN)) {
			fclose(f);
			continue;
		}

		/* So it seems we have the correct cache file -> read completely */
		data = (u8 *) malloc(count);
		if (data == NULL)
			return SC_ERROR_OUT_OF_MEMORY;
		fseek(f, 0, SEEK_SET);
		r = read_file_contents(f, &header, data, count);
		fclose(f);
		if ((r != 0) || check_header(p15card, fname, &header, data, count)) {
			free(data);
			return r == 0 ? SC_ERROR_OBJECT_NOT_VALID : r;
		}
		else {
			*buf = data;
			*bufsize = count;
			*action = 0;
			return 0;
		}
	}

	return SC_ERROR_FILE_NOT_FOUND;
}

static void increment_counter(u8 *counter)
{
	int i;
	for (i = 15; i >=0; i--) {
		counter[i]++;
		if (counter[i] != 0)
			return;
	}
}
/* En/decrypt with AES128 in counter mode. The IV for the counter is
 * stored in the file header. The key comes from the photo file.
 * No MAC is needed: integrity will be ensured by checking the
 * correspondig signature file. */
static int cache_crypt(struct sc_pkcs15_card *p15card,
	cache_header *header, const u8* datain, u8 *dataout, int datalen)
{
	u8 counter[16];
	u8 exor[16];
	AES_KEY key;
	int i, j;
	int r;

	/* Read 16 bytes from the photo file (sufficiently far away from the JPG
	 * header at the start of the file). Because of the (JPG) compression, these
	 * 16 bytes should contain plenty of randomness to serve as a key */
	if (!p15card->cache_key_read) {
		r = get_file_part(p15card, &photo_path, 1000, p15card->cache_key);
		if (r != FILE_PART_LEN)
			return r < 0 ? r : SC_ERROR_OBJECT_NOT_VALID;
		p15card->cache_key_read = 1;
	}

	r = AES_set_encrypt_key(p15card->cache_key, 128, &key);
	if (r)
		return SC_ERROR_INTERNAL;

	memset(counter + 8, 0, 8);
	memcpy(counter, header->iv, 8);

	i = 0;
	j = 16;
	while (i < datalen) {
		if (j == 16) {
			increment_counter(counter);
			AES_encrypt(counter, exor, &key);
			j = 0;
		}
		dataout[i] = datain[i] ^ exor[j++];
		i++;
	}

	return 0;
}

static int get_file_part(sc_pkcs15_card_t *p15card,
	const sc_path_t *path, int offset, u8 *file_part)
{
	int r;

	/* Read from card and cache */

	r = sc_lock(p15card->card);
	SC_TEST_RET(p15card->card->ctx, r, "sc_lock() failed");

	r = sc_select_file(p15card->card, path, NULL);
	if (r) {
		sc_unlock(p15card->card);
		return r;
	}

	r = sc_read_binary(p15card->card, offset,
		file_part, FILE_PART_LEN, 0);
	sc_unlock(p15card->card);
	SC_TEST_RET(p15card->card->ctx, r, "sc_read_binary() failed");

	return FILE_PART_LEN;
}

/* CRC-32 checksum table, used in PNG, see http://www.w3.org/TR/PNG-CRCAppendix.html */
const static unsigned long crc_table[] = {
	0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d,
};
