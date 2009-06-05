// Verify.cpp: implementation of the CVerify class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
    #include <winsock.h>
    #pragma warning(disable:4786)
#else
    #include <sys/socket.h> 

    // SK for umask in RetrieveCRL
    #include <sys/types.h>
    #include <sys/stat.h>
#endif


#include "Verify.h"
#include "eiddefines.h"
#include "../beidcommon/bytearray.h"
#include "../beidcommon/config.h"

#include <string.h>
#include <openssl/ocsp.h>
#include <openssl/bio.h>
#include <openssl/opensslv.h> // SK
#include <sys/stat.h>

// SK
//#if OPENSSL_VERSION_NUMBER >= 0x00908000L // SK
//#define OPENSSL_CAST(a) const_cast<const unsigned char **>((a))
//#else
//#define OPENSSL_CAST(a) static_cast<unsigned char **>((a))
//#endif

#ifndef _WIN32
#define strnicmp strncasecmp
#define stricmp strcasecmp
#define _fstat fstat
#define _stat stat
#define _fileno fileno
#endif

#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>

#define RRN_OID1   "2.16.56.1.1.1.3.1"
#define RRN_OID2   "2.16.56.1.1.1.4"
#define RRN_OID3   "2.16.56.9.1.1.4"
#define DN_RRN      "RRNRRNBE"  // CN + O + C

using namespace eidcommon;

CDownloadFile CVerify::m_oDownLoad;
CCertifManager *CVerify::m_pCertifManager = NULL;
QMutex CVerify::m_cs(true);

/* Root Certificate */
static unsigned char _beid_root_cert1[] = 
{
        0X30,0x82,0x03,0x94,0x30,0x82,0x02,0x7C,0xA0,0x03,0x02,0x01,0x02,0x02,0x10,0x58,0x0B,0x05,0x6C,0x53,0x24,0xDB,0xB2,0x50,0x57,0x18,0x5F,0xF9,0xE5,0xA6,0x50,0x30,0x0D,0x06,0x09,0x2A, 
        0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05,0x00,0x30,0x27,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x42,0x45,0x31,0x18,0x30,0x16,0x06,0x03,0x55,0x04,0x03,0x13,0x0F,
        0x42,0x65,0x6C,0x67,0x69,0x75,0x6D,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x1E,0x17,0x0D,0x30,0x33,0x30,0x31,0x32,0x36,0x32,0x33,0x30,0x30,0x30,0x30,0x5A,0x17,0x0D,0x31,0x34,
        0x30,0x31,0x32,0x36,0x32,0x33,0x30,0x30,0x30,0x30,0x5A,0x30,0x27,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x42,0x45,0x31,0x18,0x30,0x16,0x06,0x03,0x55,0x04,0x03,0x13,
        0x0F,0x42,0x65,0x6C,0x67,0x69,0x75,0x6D,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,
        0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xC8,0xA1,0x71,0xE9,0x1C,0x46,0x42,0x79,0x78,0x71,0x6F,0x9D,0xAE,0xA9,0xA8,0xAB,0x28,0xB7,0x4D,0xC7,0x20,0xEB,0x30,
        0x91,0x5A,0x75,0xF5,0xE2,0xD2,0xCF,0xC8,0x4C,0x14,0x98,0x42,0x58,0xAD,0xC7,0x11,0xC5,0x40,0x40,0x6A,0x5A,0xF9,0x74,0x12,0x27,0x87,0xE9,0x9C,0xE5,0x71,0x4E,0x22,0x2C,0xD1,0x12,0x18,
        0xAA,0x30,0x5E,0xA2,0x21,0xB9,0xD9,0xBB,0xFF,0xF6,0x74,0xEB,0x31,0x01,0xE7,0x3B,0x7E,0x58,0x0F,0x91,0x16,0x4D,0x76,0x89,0xA8,0x01,0x4F,0xAD,0x22,0x66,0x70,0xFA,0x4B,0x1D,0x95,0xC1,
        0x30,0x58,0xEA,0xBC,0xD9,0x65,0xD8,0x9A,0xB4,0x88,0xEB,0x49,0x46,0x52,0xDF,0xD2,0x53,0x15,0x76,0xCB,0x14,0x5D,0x19,0x49,0xB1,0x6F,0x6A,0xD3,0xD3,0xFD,0xBC,0xC2,0x2D,0xEC,0x45,0x3F,
        0x09,0x3F,0x58,0xBE,0xFC,0xD4,0xEF,0x00,0x8C,0x81,0x35,0x72,0xBF,0xF7,0x18,0xEA,0x96,0x62,0x7D,0x2B,0x28,0x7F,0x15,0x6C,0x63,0xD2,0xCA,0xCA,0x7D,0x05,0xAC,0xC8,0x6D,0x07,0x6D,0x32,
        0xBE,0x68,0xB8,0x05,0x40,0xAE,0x54,0x98,0x56,0x3E,0x66,0xF1,0x30,0xE8,0xEF,0xC4,0xAB,0x93,0x5E,0x07,0xDE,0x32,0x8F,0x12,0x74,0xAA,0x5B,0x34,0x23,0x54,0xC0,0xEA,0x6C,0xCE,0xFE,0x36,
        0x92,0xA8,0x09,0x17,0xEA,0xA1,0x2D,0xCF,0x6C,0xE3,0x84,0x1D,0xDE,0x87,0x2E,0x33,0x0B,0x3C,0x74,0xE2,0x21,0x50,0x38,0x95,0x2E,0x5C,0xE0,0xE5,0xC6,0x31,0xF9,0xDB,0x40,0xFA,0x6A,0xA1,
        0xA4,0x8A,0x93,0x9B,0xA7,0x21,0x06,0x87,0x1D,0x27,0xD3,0xC4,0xA1,0xC9,0x4C,0xB0,0x6F,0x02,0x03,0x01,0x00,0x01,0xA3,0x81,0xBB,0x30,0x81,0xB8,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,
        0x01,0xFF,0x04,0x04,0x03,0x02,0x01,0x06,0x30,0x0F,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,0x30,0x03,0x01,0x01,0xFF,0x30,0x42,0x06,0x03,0x55,0x1D,0x20,0x04,0x3B,0x30,0x39,
        0x30,0x37,0x06,0x05,0x60,0x38,0x01,0x01,0x01,0x30,0x2E,0x30,0x2C,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x20,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x72,0x65,0x70,0x6F,
        0x73,0x69,0x74,0x6F,0x72,0x79,0x2E,0x65,0x69,0x64,0x2E,0x62,0x65,0x6C,0x67,0x69,0x75,0x6D,0x2E,0x62,0x65,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x10,0xF0,0x0C,0x56,
        0x9B,0x61,0xEA,0x57,0x3A,0xB6,0x35,0x97,0x6D,0x9F,0xDD,0xB9,0x14,0x8E,0xDB,0xE6,0x30,0x11,0x06,0x09,0x60,0x86,0x48,0x01,0x86,0xF8,0x42,0x01,0x01,0x04,0x04,0x03,0x02,0x00,0x07,0x30,
        0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x10,0xF0,0x0C,0x56,0x9B,0x61,0xEA,0x57,0x3A,0xB6,0x35,0x97,0x6D,0x9F,0xDD,0xB9,0x14,0x8E,0xDB,0xE6,0x30,0x0D,0x06,0x09,
        0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0xC8,0x6D,0x22,0x51,0x8A,0x61,0xF8,0x0F,0x96,0x6E,0xD5,0x20,0xB2,0x81,0xF8,0xC6,0xDC,0xA3,0x16,0x00,
        0xDA,0xCD,0x6A,0xE7,0x6B,0x2A,0xFA,0x59,0x48,0xA7,0x4C,0x49,0x37,0xD7,0x73,0xA1,0x6A,0x01,0x65,0x5E,0x32,0xBD,0xE7,0x97,0xD3,0xD0,0x2E,0x3C,0x73,0xD3,0x8C,0x7B,0x83,0xEF,0xD6,0x42,
        0xC1,0x3F,0xA8,0xA9,0x5D,0x0F,0x37,0xBA,0x76,0xD2,0x40,0xBD,0xCC,0x2D,0x3F,0xD3,0x44,0x41,0x49,0x9C,0xFD,0x5B,0x29,0xF4,0x02,0x23,0x22,0x5B,0x71,0x1B,0xBF,0x58,0xD9,0x28,0x4E,0x2D,
        0x45,0xF4,0xDA,0xE7,0xB5,0x63,0x45,0x44,0x11,0x0D,0x2A,0x7F,0x33,0x7F,0x36,0x49,0xB4,0xCE,0x6E,0xA9,0x02,0x31,0xAE,0x5C,0xFD,0xC8,0x89,0xBF,0x42,0x7B,0xD7,0xF1,0x60,0xF2,0xD7,0x87,
        0xF6,0x57,0x2E,0x7A,0x7E,0x6A,0x13,0x80,0x1D,0xDC,0xE3,0xD0,0x63,0x1E,0x3D,0x71,0x31,0xB1,0x60,0xD4,0x9E,0x08,0xCA,0xAB,0xF0,0x94,0xC7,0x48,0x75,0x54,0x81,0xF3,0x1B,0xAD,0x77,0x9C,
        0xE8,0xB2,0x8F,0xDB,0x83,0xAC,0x8F,0x34,0x6B,0xE8,0xBF,0xC3,0xD9,0xF5,0x43,0xC3,0x64,0x55,0xEB,0x1A,0xBD,0x36,0x86,0x36,0xBA,0x21,0x8C,0x97,0x1A,0x21,0xD4,0xEA,0x2D,0x3B,0xAC,0xBA,
        0xEC,0xA7,0x1D,0xAB,0xBE,0xB9,0x4A,0x9B,0x35,0x2F,0x1C,0x5C,0x1D,0x51,0xA7,0x1F,0x54,0xED,0x12,0x97,0xFF,0xF2,0x6E,0x87,0x7D,0x46,0xC9,0x74,0xD6,0xEF,0xEB,0x3D,0x7D,0xE6,0x59,0x6E,
        0x06,0x94,0x04,0xE4,0xA2,0x55,0x87,0x38,0x28,0x6A,0x22,0x5E,0xE2,0xBE,0x74,0x12,0xB0,0x04,0x43,0x2A
};

static const unsigned char _beid_root_cert2[] = 
{
        0x30,0x82,0x03,0x8e,0x30,0x82,0x02,0x76,0xa0,0x03,0x02,0x01,0x02,0x02,0x08,0x2a,
        0xff,0xbe,0x9f,0xa2,0xf0,0xe9,0x87,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,
        0x0d,0x01,0x01,0x05,0x05,0x00,0x30,0x28,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,
        0x06,0x13,0x02,0x42,0x45,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x03,0x13,0x10,
        0x42,0x65,0x6c,0x67,0x69,0x75,0x6d,0x20,0x52,0x6f,0x6f,0x74,0x20,0x43,0x41,0x32,
        0x30,0x1e,0x17,0x0d,0x30,0x37,0x31,0x30,0x30,0x34,0x31,0x30,0x30,0x30,0x30,0x30,
        0x5a,0x17,0x0d,0x32,0x31,0x31,0x32,0x31,0x35,0x30,0x38,0x30,0x30,0x30,0x30,0x5a,
        0x30,0x28,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x42,0x45,0x31,
        0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x03,0x13,0x10,0x42,0x65,0x6c,0x67,0x69,0x75,
        0x6d,0x20,0x52,0x6f,0x6f,0x74,0x20,0x43,0x41,0x32,0x30,0x82,0x01,0x22,0x30,0x0d,
        0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,
        0x0f,0x00,0x30,0x82,0x01,0x0a,0x02,0x82,0x01,0x01,0x00,0xc6,0x73,0x42,0x1e,0x92,
        0xff,0x75,0x0f,0x8b,0xbf,0x74,0x86,0xa7,0x3f,0xed,0xb2,0x18,0x2d,0x2d,0x97,0xf9,
        0xa9,0xfb,0x98,0x4a,0x23,0xdf,0xd5,0x8d,0xe0,0x8c,0xc6,0x32,0x1c,0x16,0x2d,0x07,
        0x42,0xd6,0x2d,0xb6,0x6e,0x2f,0xa7,0xf5,0xfc,0xc6,0x85,0x82,0x5d,0x95,0xde,0xb5,
        0x22,0x00,0xcc,0xa8,0x53,0x40,0x9f,0xaf,0x0a,0x35,0xa2,0x7a,0xc7,0xe1,0xca,0xf7,

        0x60,0x10,0x67,0x0e,0xa7,0x50,0x73,0x17,0x53,0x95,0x9f,0x22,0x9f,0x0c,0x5d,0x6f,
        0xb6,0x41,0xbc,0x8c,0xeb,0xda,0x1d,0x46,0xbd,0xa7,0x94,0xbf,0xf3,0x1a,0xcb,0xd4,
        0xfb,0x8d,0x0e,0x1e,0x33,0xc8,0x96,0xd7,0xec,0x8c,0x53,0xde,0x93,0x1e,0x34,0x1e,
        0x8a,0x50,0x71,0x26,0x58,0xb4,0x5d,0xc2,0x88,0x89,0xda,0x60,0xd2,0x89,0x21,0x3f,
        0xde,0xd7,0x01,0x18,0x36,0x6e,0xe1,0x2c,0x70,0x03,0x04,0x65,0xef,0x98,0x06,0x2b,
        0x5d,0x1e,0x62,0xdc,0x55,0x6b,0xfe,0x66,0x64,0x21,0x5f,0xf8,0x2f,0xe1,0xd7,0x9b,
        0x29,0xaf,0x6f,0xcd,0x9f,0xaa,0x0c,0x46,0xd1,0x88,0xc5,0x9d,0xa2,0x95,0x9f,0xac,
        0x3f,0x15,0xae,0xf2,0x61,0xdf,0xef,0xc3,0x6b,0x9a,0x22,0xd8,0x2c,0x71,0xfd,0x58,
        0x1d,0xec,0x00,0xa4,0x38,0x20,0x95,0x5a,0xc7,0xd5,0x37,0x63,0xba,0x4f,0x9b,0xaa,
        0xfe,0x56,0x46,0x2e,0x63,0xd9,0x0b,0x23,0xd5,0x8a,0xfa,0xb5,0x23,0xf0,0x89,0x1b,
        0xa1,0x14,0x0d,0x26,0xda,0x41,0xf0,0x38,0x8f,0x30,0xed,0x05,0x26,0x79,0xc1,0x4b,
        0x4e,0xa1,0xb8,0x7e,0x55,0x52,0x3a,0x69,0x46,0x84,0x75,0x02,0x03,0x01,0x00,0x01,
        0xa3,0x81,0xbb,0x30,0x81,0xb8,0x30,0x0e,0x06,0x03,0x55,0x1d,0x0f,0x01,0x01,0xff,
        0x04,0x04,0x03,0x02,0x01,0x06,0x30,0x0f,0x06,0x03,0x55,0x1d,0x13,0x01,0x01,0xff,
        0x04,0x05,0x30,0x03,0x01,0x01,0xff,0x30,0x42,0x06,0x03,0x55,0x1d,0x20,0x04,0x3b,
        0x30,0x39,0x30,0x37,0x06,0x05,0x60,0x38,0x09,0x01,0x01,0x30,0x2e,0x30,0x2c,0x06,

        0x08,0x2b,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x20,0x68,0x74,0x74,0x70,0x3a,
        0x2f,0x2f,0x72,0x65,0x70,0x6f,0x73,0x69,0x74,0x6f,0x72,0x79,0x2e,0x65,0x69,0x64,
        0x2e,0x62,0x65,0x6c,0x67,0x69,0x75,0x6d,0x2e,0x62,0x65,0x30,0x1d,0x06,0x03,0x55,
        0x1d,0x0e,0x04,0x16,0x04,0x14,0x85,0x8a,0xeb,0xf4,0xc5,0xbb,0xbe,0x0e,0x59,0x03,
        0x94,0xde,0xd6,0x80,0x01,0x15,0xe3,0x10,0x9c,0x39,0x30,0x11,0x06,0x09,0x60,0x86,
        0x48,0x01,0x86,0xf8,0x42,0x01,0x01,0x04,0x04,0x03,0x02,0x00,0x07,0x30,0x1f,0x06,
        0x03,0x55,0x1d,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x85,0x8a,0xeb,0xf4,0xc5,0xbb,
        0xbe,0x0e,0x59,0x03,0x94,0xde,0xd6,0x80,0x01,0x15,0xe3,0x10,0x9c,0x39,0x30,0x0d,
        0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x05,0x05,0x00,0x03,0x82,0x01,
        0x01,0x00,0x51,0xd8,0x85,0xdd,0xbb,0x57,0x6f,0xcc,0xa0,0x6c,0xb5,0xa3,0x20,0x9c,
        0x53,0x09,0xf3,0x4a,0x01,0x0c,0x74,0xbf,0x2b,0xb3,0x9a,0x9a,0xba,0x18,0xf2,0x0b,
        0x88,0xac,0x1c,0xb3,0x33,0xaf,0xce,0xe5,0x13,0x01,0x27,0x92,0x84,0x58,0x9a,0x10,
        0xb9,0xf7,0xcc,0x14,0x92,0x6b,0x74,0x16,0x8a,0x96,0xe8,0x51,0xef,0xbf,0xfa,0x4a,
        0x25,0xa7,0x89,0xb6,0x63,0x2b,0x5d,0x94,0x58,0xd1,0xcf,0x11,0x72,0xb6,0x1e,0xb9,
        0x39,0x41,0x16,0x4d,0x29,0xbc,0x35,0x53,0x0b,0xda,0xde,0x8e,0x0e,0xcd,0xa9,0x95,
        0x77,0x25,0xca,0x94,0x5a,0xe9,0xb2,0x69,0xae,0xd8,0xc0,0x13,0xbe,0x98,0xfc,0x96,

        0x9c,0x84,0x7f,0x55,0x13,0xe6,0x3c,0x87,0xe3,0xbc,0x20,0xa4,0xa4,0x36,0x68,0x6b,
        0x4d,0x60,0x66,0x1c,0xf9,0xbf,0xac,0x80,0x94,0x66,0x2e,0xb9,0x41,0x8a,0xd3,0x65,
        0xd3,0x84,0x80,0x02,0xef,0x50,0x1d,0x5e,0x46,0xdc,0xf7,0xc9,0xba,0xb5,0x34,0x7c,
        0x2a,0xf3,0xc6,0xd8,0x5f,0x5f,0x54,0x9d,0xdb,0x4d,0xcd,0x11,0xe7,0xfd,0x14,0x02,
        0x83,0x66,0x5e,0xc8,0xa6,0x00,0x12,0xa0,0x5f,0xbe,0xce,0x14,0xfe,0xbb,0x1f,0xa7,
        0x61,0xf7,0xab,0x4a,0xf1,0x06,0x14,0x9f,0xca,0x49,0x42,0xc2,0xa9,0xbc,0xed,0x85,
        0xb1,0xab,0x81,0x41,0xe6,0x0d,0xc5,0x42,0x69,0x53,0x87,0x39,0x9d,0x4c,0x1f,0x00,
        0x0e,0x3e,0x07,0x0d,0x75,0x57,0x44,0xa8,0x53,0xb4,0x36,0x76,0x64,0x99,0xdc,0x6e,
        0xeb,0x3d,0x46,0x6e,0x14,0x5d,0x5e,0x47,0x53,0x8d,0x78,0x4d,0xe0,0x27,0xbb,0x8e,
        0x85,0x76
};
/*
//TEST ROOT FOR TESTING
static const unsigned char _beid_root_cert2[] = 
{0x30,0x82,0x03,0xB4,0x30,0x82,0x02,0x9C,0xA0,0x03,0x02,0x01,0x02,0x02,0x10
,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11
,0x12,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05
,0x00,0x30,0x30,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x42
,0x45,0x31,0x21,0x30,0x1F,0x06,0x03,0x55,0x04,0x03,0x13,0x18,0x53,0x50,0x45
,0x43,0x49,0x4D,0x45,0x4E,0x20,0x42,0x65,0x6C,0x67,0x69,0x75,0x6D,0x20,0x52
,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x1E,0x17,0x0D,0x30,0x33,0x30,0x38,0x31
,0x33,0x30,0x39,0x30,0x30,0x30,0x30,0x5A,0x17,0x0D,0x31,0x34,0x30,0x31,0x32
,0x36,0x32,0x33,0x30,0x30,0x30,0x30,0x5A,0x30,0x30,0x31,0x0B,0x30,0x09,0x06
,0x03,0x55,0x04,0x06,0x13,0x02,0x42,0x45,0x31,0x21,0x30,0x1F,0x06,0x03,0x55
,0x04,0x03,0x13,0x18,0x53,0x50,0x45,0x43,0x49,0x4D,0x45,0x4E,0x20,0x42,0x65
,0x6C,0x67,0x69,0x75,0x6D,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x82
,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01
,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01
,0x00,0xD3,0x6F,0xB9,0xD3,0x41,0x97,0xD1,0x34,0x4F,0xE0,0x80,0x2B,0xDB,0x1F
,0x24,0x95,0xDB,0x1A,0x80,0x72,0xBA,0x41,0xBC,0x08,0xB1,0x8F,0xF9,0xAD,0xB5
,0x8D,0x16,0xF6,0xCA,0x5E,0xFF,0x87,0xDB,0x94,0xD8,0x3E,0x5B,0x17,0xEB,0x3D
,0x46,0x61,0x7A,0x24,0xE5,0x8C,0x79,0xBE,0x21,0x33,0xF7,0xAD,0x04,0x52,0xB7
,0x83,0x9F,0x54,0xDE,0x62,0xFD,0x30,0x25,0xDB,0x2A,0x51,0xDB,0xA4,0x49,0x8E
,0x1F,0x63,0xA7,0x7B,0x30,0xEE,0x70,0xB4,0x5A,0xEE,0xC8,0x4E,0x78,0xF3,0x89
,0xF1,0x02,0x63,0x1C,0xC4,0x68,0x65,0xFA,0x25,0xBB,0x1C,0xAF,0x74,0x2D,0x22
,0x13,0x1A,0x23,0x27,0x2A,0xEA,0x9E,0xF6,0x15,0xF4,0xE1,0x21,0x53,0xD6,0xF0
,0x9C,0x50,0xAD,0x44,0xDE,0x98,0xE2,0xC2,0xE6,0x51,0x5B,0x16,0x57,0xF8,0x1C
,0x95,0x5A,0x0D,0x53,0xDD,0x35,0x8C,0x81,0xB9,0x6B,0x48,0x16,0x47,0x28,0x64
,0xF7,0x49,0x4C,0xF9,0x3A,0x09,0xC5,0x1E,0x12,0xF2,0xF7,0x2B,0x0F,0x6D,0x1F
,0xE5,0x9C,0x73,0x3E,0x55,0xB8,0xD5,0xB8,0x0D,0x2A,0x66,0xC4,0xC8,0x31,0xCB
,0x5B,0x11,0xED,0xCB,0xCA,0x9A,0xD9,0x24,0xD8,0xD8,0xE6,0x90,0x47,0x2E,0xC8
,0x29,0x5C,0x7D,0x47,0x6A,0xD1,0xE9,0xD4,0x7D,0x43,0x21,0xE0,0xF2,0x87,0x68
,0x43,0x63,0xF9,0x29,0xA6,0xFB,0x99,0x18,0xAF,0xAC,0x29,0x86,0xDB,0xE9,0xC4
,0xF9,0xE1,0xDD,0x8D,0xA6,0x74,0xD0,0xA2,0xDC,0x03,0x68,0xAA,0x1A,0x13,0x8A
,0x93,0xCD,0x8C,0x1D,0x8A,0x7A,0x1D,0x87,0x33,0x8D,0x10,0x7D,0xC0,0x89,0x56
,0x4C,0xF9,0x02,0x03,0x01,0x00,0x01,0xA3,0x81,0xC9,0x30,0x81,0xC6,0x30,0x0E
,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x01,0x06,0x30
,0x0F,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,0x30,0x03,0x01,0x01
,0xFF,0x30,0x50,0x06,0x03,0x55,0x1D,0x20,0x04,0x49,0x30,0x47,0x30,0x45,0x06
,0x09,0x03,0x90,0x0E,0x09,0x06,0x01,0x1F,0x01,0x01,0x30,0x38,0x30,0x36,0x06
,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x2A,0x68,0x74,0x74,0x70
,0x3A,0x2F,0x2F,0x72,0x65,0x70,0x6F,0x73,0x69,0x74,0x6F,0x72,0x79,0x2E,0x73
,0x70,0x65,0x63,0x69,0x6D,0x65,0x6E,0x2D,0x65,0x69,0x64,0x2E,0x62,0x65,0x6C
,0x67,0x69,0x75,0x6D,0x2E,0x62,0x65,0x2F,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E
,0x04,0x16,0x04,0x14,0x67,0x5C,0x8F,0x5D,0x98,0xA4,0x3C,0xCC,0xC5,0xF6,0x1F
,0x71,0x20,0xD3,0x86,0x0F,0x3D,0x1B,0x2F,0x35,0x30,0x11,0x06,0x09,0x60,0x86
,0x48,0x01,0x86,0xF8,0x42,0x01,0x01,0x04,0x04,0x03,0x02,0x00,0x07,0x30,0x1F
,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x67,0x5C,0x8F,0x5D
,0x98,0xA4,0x3C,0xCC,0xC5,0xF6,0x1F,0x71,0x20,0xD3,0x86,0x0F,0x3D,0x1B,0x2F
,0x35,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05
,0x00,0x03,0x82,0x01,0x01,0x00,0x2F,0xCC,0xF7,0xB0,0x2F,0x29,0x2B,0x00,0x5A
,0x2C,0xBC,0x31,0x5A,0x4A,0xE5,0x20,0x6B,0xC8,0x06,0x09,0x4D,0x2F,0xC9,0x1D
,0x7E,0x6A,0x55,0xF4,0xD4,0x50,0x81,0xC5,0x75,0x49,0xDA,0xDE,0x12,0xCD,0x14
,0xA9,0x89,0xDB,0xCA,0x16,0x8E,0xE0,0x01,0xD3,0x85,0x2E,0xE1,0x1F,0xE6,0x61
,0x76,0x61,0xBB,0x8E,0xB1,0xE3,0x6C,0x19,0xA6,0x2C,0x8F,0x82,0x99,0x0E,0x98
,0xD1,0x8B,0x60,0xCE,0xBB,0x3F,0x92,0x1A,0xAA,0xDD,0xCB,0x5B,0xCA,0x2A,0xC7
,0x77,0x47,0xB5,0x38,0x12,0xC1,0x67,0x1C,0x50,0x64,0xE4,0x98,0xB1,0x9D,0x70
,0xE7,0xBC,0x3D,0xA7,0x61,0xCE,0xA3,0x76,0xE9,0xF7,0x23,0x8A,0x6D,0xC1,0x2D
,0xE1,0x0E,0x75,0x20,0x71,0x45,0xB4,0x56,0x1E,0x4B,0xE0,0x97,0x8C,0x3B,0xBB
,0x77,0xFC,0xDD,0xEC,0xA3,0x26,0xFD,0xD6,0x9A,0x58,0x14,0x9C,0x6A,0x30,0xA3
,0x26,0xDD,0x67,0x22,0x6A,0xCD,0xF7,0xDB,0x7F,0x2D,0x48,0xB5,0x93,0x3B,0x5C
,0x4E,0xEC,0x6C,0x86,0xBE,0x8F,0x47,0x7A,0xDE,0xCD,0x69,0xBA,0x8A,0xA1,0x22
,0xB0,0x3E,0x83,0x16,0x5F,0x9B,0xB5,0x33,0x95,0x7C,0x5A,0x31,0x55,0xD8,0x9A
,0xCB,0xCA,0xEC,0x57,0x7C,0x18,0xDC,0x30,0x47,0x20,0xEA,0x35,0x15,0x7D,0xB8
,0x3C,0x60,0xB3,0x59,0x56,0x50,0xB3,0xA8,0x03,0xC8,0x2D,0x28,0x0D,0x2D,0x12
,0x1D,0x35,0x62,0xE0,0xAB,0xED,0xE5,0x53,0x54,0x43,0x4B,0x68,0xBB,0x98,0x00
,0xB6,0x78,0xE7,0xC7,0x93,0x06,0xE4,0x46,0xC6,0x5C,0x65,0x19,0xC4,0x00,0xD3
,0x79,0x4D,0xC4,0x45,0x76,0x0F,0xDF
};
*/

static const unsigned char *const _beid_root_certs[] = 
{
	_beid_root_cert1,
	_beid_root_cert2,
	NULL
};

static const unsigned long _beid_root_certs_size[] = 
{
	sizeof(_beid_root_cert1),
	sizeof(_beid_root_cert2)
};

bool d2i_X509_Wrapper(X509** pX509,const unsigned char* pucContent, int iContentSize){
#if (OPENSSL_VERSION_NUMBER > 0x009070ffL)
  *pX509 = d2i_X509(pX509, &pucContent, iContentSize);
#else
  unsigned char* pucContent_nonconst = const_cast<unsigned char*> (pucContent);
  *pX509 = d2i_X509(pX509, &pucContent_nonconst, iContentSize);
#endif
  if(*pX509 == NULL) return false;

  return true;
}

bool d2i_X509_CRL_Wrapper(X509_CRL** pX509Crl,const unsigned char* pucContent, int iContentSize){
#if (OPENSSL_VERSION_NUMBER > 0x009070ffL)
  *pX509Crl = d2i_X509_CRL(pX509Crl, &pucContent, iContentSize);
#else
  unsigned char* pucContent_nonconst = const_cast<unsigned char*> (pucContent);
  *pX509Crl = d2i_X509_CRL(pX509Crl, &pucContent_nonconst, iContentSize);
#endif
  if(*pX509Crl == NULL) return false;

  return true;
}


CVerify::CVerify()
{
}

CVerify::~CVerify()
{
}

void CVerify::InitX509Store(spc_x509store_t *spc_store)
{
  spc_store->callback = 0;
  spc_store->certs = sk_X509_new_null();
  spc_store->crls = sk_X509_CRL_new_null();
  spc_store->use_certfile = 0;
  spc_store->use_certs = sk_X509_new_null();
  spc_store->use_keyfile = 0;
  spc_store->use_key = 0;
  spc_store->flags = 0;
}

void CVerify::CleanupX509store(spc_x509store_t *spc_store)
{
    int i = 0;
  if (spc_store->use_certfile) free(spc_store->use_certfile);
  if (spc_store->use_keyfile) free(spc_store->use_keyfile);
  if (spc_store->use_key) EVP_PKEY_free(spc_store->use_key);
  for (i = 0; i < sk_X509_num(spc_store->certs); i++)
  {
     X509 *pX509 = sk_X509_value(spc_store->certs, i);
     if (pX509 != NULL)
     {
         X509_free(pX509);
         pX509 = NULL;
     }
  }
  sk_X509_free(spc_store->certs);
  for (i = 0; i < sk_X509_num(spc_store->crls); i++)
  {
     X509_CRL *pX509CRL = sk_X509_CRL_value(spc_store->crls, i);
     if (pX509CRL != NULL)
     {
         X509_CRL_free(pX509CRL);
         pX509CRL = NULL;
     }
  }
  sk_X509_free(spc_store->crls);
  sk_X509_free(spc_store->use_certs);
}

void CVerify::X509StoreAddcert(spc_x509store_t *spc_store, X509 *cert)
{
    // Check Certificate already in store
    bool bFound = false;
    for (int i = 0; i < sk_X509_num(spc_store->certs); i++)
    {
        X509 *pX509 = sk_X509_value(spc_store->certs, i);
        if (X509_V_OK == X509_cmp(pX509, cert))
        {
            bFound = true;
            X509_free(cert);
            break;
        }
    }
    if(!bFound)
        sk_X509_push(spc_store->certs, cert);
}
   
void CVerify::X509StoreAddCRL(spc_x509store_t *spc_store, X509_CRL *crl)
{
    // Check CRL already in store
    bool bFound = false;
    for (int i = 0; i < sk_X509_num(spc_store->crls); i++)
    {
        X509_CRL *pX509CRL = sk_X509_CRL_value(spc_store->crls, i);
        if (X509_V_OK == X509_CRL_cmp(pX509CRL, crl))
        {
            bFound = true;
            X509_CRL_free(crl);
            break;
        }
    }
    if(!bFound)
        sk_X509_CRL_push(spc_store->crls, crl);
}

void CVerify::X509StoreSetcallback(spc_x509store_t *spc_store, spc_x509verifycallback_t callback)
{
  spc_store->callback = callback;
}
   
void CVerify::X509StoreSetflags(spc_x509store_t *spc_store, int flags)
{
  spc_store->flags |= flags;
}
   
void CVerify::X509StoreClearflags(spc_x509store_t *spc_store, int flags)
{
  spc_store->flags &= ~flags;
}

X509_STORE *CVerify::CreateX509store(spc_x509store_t *spc_store)
{
  int i = 0;
  X509_STORE *store = 0;
  X509_LOOKUP *lookup = 0;
   
  store = X509_STORE_new();
  if (spc_store->callback)
    X509_STORE_set_verify_cb_func(store, spc_store->callback);
   
  if (!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file())))
    goto error_exit;

   X509_STORE_set_flags(store, spc_store->flags);

  if (!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir())))
    goto error_exit;
  
  for (i = 0; i < sk_X509_num(spc_store->certs); i++)
    if (!X509_STORE_add_cert(store, sk_X509_value(spc_store->certs, i)))
      goto error_exit;
  for (i = 0; i < sk_X509_CRL_num(spc_store->crls); i++)
    if (!X509_STORE_add_crl(store, sk_X509_CRL_value(spc_store->crls, i)))
      goto error_exit;
   
  return store;
   
error_exit:
  if (store) X509_STORE_free(store);
  return 0;
}

const char *CVerify::GetExtensionString( X509_EXTENSION *extension ) 
{ 
    ASN1_OBJECT *obj = NULL; 
    int nid = NID_undef; 
    const char *ext_sn; 

    obj = X509_EXTENSION_get_object( extension ); 
    if ( !obj ) return NULL; 

    nid = OBJ_obj2nid( obj ); 
    if ( NID_undef == nid ) return NULL; 

    ext_sn = OBJ_nid2sn( nid ); 
    return ext_sn; 
} 

X509 *CVerify::DownloadCert(X509 *pCert)
{
    CConfig oConfig;
    oConfig.Load();
    X509 *pX509 = NULL;
    X509_NAME * pName = X509_get_issuer_name(pCert);
    std::string szCertStorePath = oConfig.GetCertStorePath();
    std::string szHttpStorePath = oConfig.GetHttpStore();
    if(pName != NULL && szCertStorePath.length() > 0 && szHttpStorePath.length() > 0)
    {
        char *pszName = 0;
        char szLocation[256] = {0};
        char szFile[256] = {0};
        pszName = X509_NAME_oneline(pName, NULL, 0);
        if(NULL != strstr(pszName, "Government CA"))
        {
            QStringList oList = oConfig.GetGovCerts();
            for ( QStringList::Iterator it = oList.begin(); it != oList.end(); ++it ) 
            {
                sprintf(szLocation, "%s%s.crt", szHttpStorePath.c_str(), (*it).ascii());
                sprintf(szFile, "%s%s.der", szCertStorePath.c_str(), (*it).ascii());
                QString strNewFile= QString("%1beid-cert-%2.der").arg(szCertStorePath.c_str()).arg(*it);
                if( !QFile::exists(szFile) && !QFile::exists(strNewFile))
                {
                    if(NULL != (pX509 = RetrieveCert(szLocation, szFile)))
                    {
                        if(X509_V_OK == X509_check_issued(pX509, pCert))
                        {
                            break;
                        }
                        X509_free(pX509);
                        pX509 = NULL;
                    }
                }
            }
        }
        else if(NULL != strstr(pszName, "Belgium Root CA"))
        {
            QStringList oList = oConfig.GetRootCerts();
            for ( QStringList::Iterator it = oList.begin(); it != oList.end(); ++it ) 
            {
                sprintf(szLocation, "%s%s.crt", szHttpStorePath.c_str(), (*it).ascii());
                sprintf(szFile, "%s%s.der", szCertStorePath.c_str(), (*it).ascii());
                QString strNewFile= QString("%1beid-cert-%2.der").arg(szCertStorePath.c_str()).arg(*it);
                if( !QFile::exists(szFile) && !QFile::exists(strNewFile))
                {
                    if(NULL != (pX509 = RetrieveCert(szLocation, szFile)))
                    {
                        if(X509_V_OK == X509_check_issued(pX509, pCert))
                        {
                            break;
                        }
                        X509_free(pX509);
                        pX509 = NULL;
                    }
                }
            }            
        }
	    OPENSSL_free (pszName);
	    pszName = NULL;
    }
    return pX509;
}

X509 *CVerify::FindLocalIssuer(X509 *pCert)
{
    X509 *pX509 = NULL;
    CConfig oConfig;
    oConfig.Load();

    std::string szCertStorePath = oConfig.GetCertStorePath();
    if(szCertStorePath.length() == 0)
        return NULL;

    QDir dir( szCertStorePath.c_str() );
    QStringList lst = dir.entryList( "*.der" );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) 
    {
        pX509 = LoadFromCertFile(dir.filePath(*it).ascii());
        if (NULL != pX509)
        {
            if(X509_V_OK == X509_check_issued(pX509, pCert))
            {
                break;
            }
            X509_free(pX509);
        }
        pX509 = NULL;
    }

    if(NULL == pX509)
    {
        // Not Found try to download
        pX509 = DownloadCert(pCert);
    }
    return pX509;
}


char *CVerify::GetExtensionValue(X509 *pCert, int iNID)
{
    int i, j;
    struct stack_st *pStack = NULL;
    const char *pData = NULL;
    BOOL bFound = FALSE;

    pStack = (struct stack_st *) X509_get_ext_d2i(pCert, iNID, NULL, NULL);

    if(pStack != NULL)
    {
        if(iNID == NID_crl_distribution_points)
        {
            for(j = 0; j < sk_DIST_POINT_num(pStack) && !bFound; j++) 
            {
                DIST_POINT *pRes = (DIST_POINT *)sk_DIST_POINT_value(pStack, j);
                if(pRes != NULL)
                {
                    STACK_OF(GENERAL_NAME) *pNames = pRes->distpoint->name.fullname;
                    if(pNames)
                    {
                        for(i = 0; i < sk_GENERAL_NAME_num(pNames) && !bFound; i++) 
                        {
                            GENERAL_NAME *pName = sk_GENERAL_NAME_value(pNames, i);
                            if(pName != NULL && pName->type == GEN_URI )
                            {
                                pData = (const char *)ASN1_STRING_data(pName->d.uniformResourceIdentifier); 
                                bFound = TRUE;
                            }
                         }
                        sk_GENERAL_NAME_free(pNames);
                    }
                }
            } 
            sk_DIST_POINT_free(pStack);
        }
        else if(iNID == NID_info_access)
        {
            for(j = 0; j < sk_ACCESS_DESCRIPTION_num(pStack) && !bFound; j++) 
            {
                ACCESS_DESCRIPTION *pAccess = sk_ACCESS_DESCRIPTION_value(pStack, j);
                if(pAccess != NULL && pAccess->method != NULL && OBJ_obj2nid(pAccess->method) == NID_ad_OCSP)
                {
                    GENERAL_NAME *pName = pAccess->location;
                    if(pName != NULL && pName->type == GEN_URI)
                    {
                        pData = (const char *)ASN1_STRING_data(pName->d.uniformResourceIdentifier); 
                        bFound = TRUE;
                    }
                }
            }
            sk_ACCESS_DESCRIPTION_free(pStack);
        }
    }

    if(bFound)
    {
        return strdup(pData);
    }
    return NULL;
}

int CVerify::VerifyCertHostname(X509 *pCert, char *pszHostname)
{
    struct stack_st *pStack = NULL;
    BOOL bFound = FALSE;

    pStack = (struct stack_st *) X509_get_ext_d2i(pCert, NID_subject_alt_name, NULL, NULL);
    if(pStack != NULL)
    {
        int i, iLen1, iLen2;
	    char *pszDomain = NULL;
	    iLen1 = strlen(pszHostname);
	    pszDomain = strchr(pszHostname,  '.');
	    if (pszDomain)
        {
	    	iLen2 = iLen1 - (pszDomain - pszHostname);
        }

        for(i = 0; i < sk_GENERAL_NAME_num(pStack) && !bFound; i++) 
        {
            GENERAL_NAME *pName = sk_GENERAL_NAME_value(pStack, i);
            if(pName != NULL && pName->type == GEN_DNS )
            {
                char *pData = (char *)ASN1_STRING_data(pName->d.ia5); 
                int iLen = ASN1_STRING_length(pName->d.ia5);

                // Is this an exact match ?
                if ((iLen1 == iLen) && !strnicmp(pszHostname, pData, iLen1))
                {
                    bFound = TRUE;
                }
		        // Is this a wildcard match ?
		        else if ((*pData == '*') && pszDomain && (iLen2 == iLen - 1) && !strnicmp(pszDomain, pData + 1, iLen2))
                {
                    bFound = TRUE;
                }
            }
         }
        sk_GENERAL_NAME_free(pStack);
    }
    if(!bFound)
    {
        char pszName[256] = {0};
        X509_NAME *pxName = NULL;
        pxName = X509_get_subject_name(pCert);
        if(pxName &&  X509_NAME_get_text_by_NID(pxName, NID_commonName, pszName, sizeof(pszName)) > 0) 
        {
            pszName[sizeof(pszName) - 1] = '\0';
            if (!stricmp(pszName, pszHostname)) 
            {
                bFound = TRUE;
            } 
         }
    }

    return bFound;
}


BIO *CVerify::Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx) 
{
    BIO *pConn = NULL;
   
    if (iSSL) 
    {
        if (!(pConn = ConnectSSL(pszHost, iPort, NULL, ppSSLCtx))) goto error_exit;
        return pConn;
    }

    *ppSSLCtx = 0;
    if (!(pConn = BIO_new_connect(pszHost))) goto error_exit;
    BIO_set_conn_int_port(pConn, &iPort);
    if (BIO_do_connect(pConn) <= 0) goto error_exit;
    return pConn;
   
error_exit:
    if (pConn) BIO_free_all(pConn);
    return 0;
}

BIO *CVerify::ConnectSSL(char *pszHost, int iPort, spc_x509store_t *pStore, SSL_CTX **ppSSLCtx) 
{
    BIO *pConn = NULL;
    OpenSSL_add_all_algorithms();
    *ppSSLCtx = SSL_CTX_new(SSLv23_client_method());
    
    if (!(pConn = BIO_new_ssl_connect(*ppSSLCtx))) goto error_exit;
    BIO_set_conn_hostname(pConn, pszHost);
    BIO_set_conn_int_port(pConn, &iPort);
   
    if (BIO_do_connect(pConn) <= 0) goto error_exit;  
    return pConn;
   
error_exit:
  if (pConn) BIO_free_all(pConn);
  if (*ppSSLCtx) 
  {
      SSL_CTX_free(*ppSSLCtx);
      *ppSSLCtx = NULL;
  }
  return 0;
}


X509_CRL *CVerify::LoadFromCRLFile(const std::string & strFrom)
{
    X509_CRL *pX509CRL = 0;
    FILE *pf = NULL;
    pf = fopen(strFrom.c_str(), "rb");
    if(NULL != pf)
    {
        int numread = 0;
        struct _stat buf = {0};
        if(0 == _fstat(_fileno(pf), &buf))
        {
            unsigned char *pBuffer = (unsigned char *)malloc(buf.st_size);
            const unsigned char *pTempBuffer = pBuffer;
            numread = fread(pBuffer, sizeof(unsigned char), buf.st_size, pf);
            char buffer[20] = {0};
            if(numread == buf.st_size)
            {
                d2i_X509_CRL_Wrapper(&pX509CRL, pTempBuffer, numread);                
            }
            if(pBuffer)
            {
                free(pBuffer);
            }
        }
        fclose(pf);
    }
    return pX509CRL;
}

X509_CRL *CVerify::RetrieveCRL(char *pszUri, char *pszFile) 
{
#ifndef WIN32
	mode_t oldMask = umask(0);
#endif

    X509_CRL *pCRL = 0;
    std::string strFrom(pszUri);
    std::string strTo(pszFile);
    if(m_oDownLoad.Download(strFrom, strTo))
    {
        pCRL = LoadFromCRLFile(strTo);
    }
    return pCRL;

#ifndef WIN32
	umask(oldMask);
#endif
}

X509 *CVerify::LoadFromCertFile(const std::string & strFrom)
{
    X509 *pX509 = 0;
    FILE *pf = NULL;
    pf = fopen(strFrom.c_str(), "rb");
    if(NULL != pf)
    {
        unsigned char szBuffer[2048] = {0};
        CByteArray oByteArray;
        int numread = 0;
        while( !feof(pf) )
        {                    
            numread = fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), pf);
            if(numread > 0)
            {
                oByteArray.Append(szBuffer, numread); 
                numread = 0;
            }
        }
        if(oByteArray.GetSize() > 0)
        {
            const unsigned char *pDummy = oByteArray.GetData();
            d2i_X509_Wrapper(&pX509, pDummy, oByteArray.GetSize());
        }
        fclose(pf);
    }
    return pX509;
}

X509 *CVerify::RetrieveCert(char *pszUri, char *pszFile) 
{
    X509 *pX509 = 0;
    std::string strFrom(pszUri);
    std::string strTo(pszFile);
    if(m_oDownLoad.Download(strFrom, strTo))
    {
        pX509 = LoadFromCertFile(strTo);
    }
    return pX509;
}

X509_CRL *CVerify::FindLocalCRL(char *pCRLDistribution, bool bCRLDownload /* true */)
{
    CConfig oConfig;
    oConfig.Load();

    X509_CRL *pX509CRL = NULL;
    bool bDownload = false;
    std::string szCRLStorePath = oConfig.GetCRLStorePath();

    if(pCRLDistribution != NULL && szCRLStorePath.length() > 0)
    {
        char *pTemp; 
        char *pFile = pCRLDistribution;
        while(NULL != (pTemp = strstr(pFile, "/")))
        {
            pFile = ++pTemp;
        }

        char szFile[256] = {0};
        sprintf(szFile, "%s%s", szCRLStorePath.c_str(), pFile);
        pX509CRL = LoadFromCRLFile(szFile);
        if(pX509CRL != NULL)
        {
            int iCheck = 0;
	        iCheck = X509_cmp_time(X509_CRL_get_lastUpdate(pX509CRL), NULL);
	        if (iCheck >= 0)
		    {
                // X509_V_ERR_CRL_NOT_YET_VALID or X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD error
                bDownload = true;
		    }

	        if(X509_CRL_get_nextUpdate(pX509CRL))
		    {
		        iCheck = X509_cmp_time(X509_CRL_get_nextUpdate(pX509CRL), NULL);
		        if (iCheck <= 0)
			    {
                    // X509_V_ERR_CRL_HAS_EXPIRED or X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD error
                    bDownload = true;
			     }
		    }
        }
        else
        {
            bDownload = true;
        }    

        if(bDownload && bCRLDownload)
        {
            if(pX509CRL != NULL)
            {
                X509_CRL_free(pX509CRL);
                pX509CRL = NULL;
            }
            // Try to download CRL and check again
            pX509CRL = RetrieveCRL(pCRLDistribution, szFile); 
        }
    }
    return pX509CRL;
}

int CVerify::VerifyCert(X509 *cert, X509_STORE *store, std::vector<std::string> & CRLDistr, bool bCRLDownload /* true */) 
{
  int  result = -1;
  X509_STORE_CTX *ctx = 0;
  X509_CRL *pX509Crl = NULL;

  
  if ((ctx = X509_STORE_CTX_new()) != 0) 
  {
    if (X509_STORE_CTX_init(ctx, store, cert, 0) == 1)
    {
      // Check all issuers
      bool bOk = true;
      X509 *pCertCheck = cert;
      X509 *pIssuer = NULL;
      do
      {
        if(X509_STORE_CTX_get1_issuer(&pIssuer, ctx, pCertCheck))
        {
            if( 0 == strcmp(pIssuer->name, pCertCheck->name) )
            {
                bOk = false; // Break on Selfsigned
            }
            else
            {
                pCertCheck = pIssuer;
                pIssuer = NULL;
            }
        }
        else
        {
            // Lookup Issuer in additional cert store
            X509 *pFound = FindLocalIssuer(pCertCheck);
            if (pFound == NULL)
            {
                // Issuer certificate not found
                bOk = false;
            }
            else
            {
                // Add certificate to store
                X509_STORE_add_cert(store, pFound);
                char *pCRLDistribution = GetExtensionValue(pFound, NID_crl_distribution_points);
                if(NULL != pCRLDistribution)
                {
                    bool bStop = false;
                    for(unsigned int iNr = 0; iNr < CRLDistr.size(); ++iNr)
                    {
                        if(0 == stricmp(CRLDistr[iNr].c_str(), pCRLDistribution))
                        {
                            bStop = true;
                            break;
                        }
                    }
                    if(!bStop)
                    {
                        CRLDistr.push_back(pCRLDistribution);
                        if(NULL != (pX509Crl = FindLocalCRL(pCRLDistribution, bCRLDownload)))
                        {
                            X509_STORE_add_crl(store, pX509Crl);
                            pX509Crl = NULL;
                        }
                    }
                    free(pCRLDistribution);
                }
            }
        }
      } while (bOk);
      
      result = (X509_verify_cert(ctx) == 1);
    }
    X509_STORE_CTX_free(ctx);
  }
  return result;
}

int CVerify::VerifyCertroot(X509 *cert, X509_STORE *store) 
{
  int  result = -1;
  X509_STORE_CTX *ctx = 0;
  
  if ((ctx = X509_STORE_CTX_new()) != 0) 
  {
    if (X509_STORE_CTX_init(ctx, store, cert, 0) == 1)
    {
      // Check all issuers
      bool bOk = true;
      X509 *pCertCheck = cert;
      X509 *pIssuer = NULL;
      do
      {
        if(X509_STORE_CTX_get1_issuer(&pIssuer, ctx, pCertCheck))
        {
            if( 0 == strcmp(pIssuer->name, pCertCheck->name) )
            {
                // Now Check against hardcoded Root(s)
                X509 *pX509Root = NULL;
				const unsigned char * const *pRoot = NULL;
				unsigned char tucDummy[3000];
				const unsigned char *pucDummy;
				int i = 0;

				for(pRoot=_beid_root_certs,i=0;*pRoot!=NULL;pRoot++,i++)
                {
		    memcpy(tucDummy, *pRoot, _beid_root_certs_size[i] < sizeof(tucDummy) ?
			_beid_root_certs_size[i] : sizeof(tucDummy));
		    pucDummy = tucDummy;

                    if (!d2i_X509_Wrapper(&pX509Root, pucDummy, _beid_root_certs_size[i]))
                        return BEID_SIGNATURE_PROCESSING_ERROR;
                    if(X509_V_OK == X509_cmp(pX509Root, pIssuer))
                    {
                        result = X509_V_OK;
                    }
                    X509_free(pX509Root);
                    pX509Root = NULL;
                }
                bOk = false;
            }
            else
            {
                pCertCheck = pIssuer;
                pIssuer = NULL;
            }
        }
        else
        {
            // Lookup Issuer in additional cert store
            X509 *pFound = FindLocalIssuer(pCertCheck);
            if (pFound == NULL)
            {
                // Issuer certificate not found
                bOk = false;
            }
            else
            {
                // Add certificate to store
                X509_STORE_add_cert(store, pFound);
            }
        }
      } while (bOk);
    }
    X509_STORE_CTX_free(ctx);
  }
  return result;
}

spc_ocspresult_t CVerify::VerifyViaOCSP(spc_ocsprequest_t *data, spc_x509store_t *pStore) 
{
    CConfig oConfig;
    oConfig.Load();
    BIO *pBio = 0;
    int iRetCode = -1;
    int iReason = 0;
    int iSSL = 0;
    int iStatus = 0;
    char *pszHost = 0;
    std::string szConfigHost = oConfig.GetProxyHost();
    std::string szConfigPort = oConfig.GetProxyPort();
    char *pszPath = 0;
    char *pszPort = 0;
    SSL_CTX  *pSSLCtx = 0;
    OCSP_CERTID *pCertID;
    OCSP_REQUEST  *pReq = 0;
    OCSP_RESPONSE *pResp = 0;
    OCSP_BASICRESP *pBasic = 0;
    spc_ocspresult_t  tResult = SPC_OCSPRESULT_ERROR_UNKNOWN;
    ASN1_GENERALIZEDTIME  *producedAt, *thisUpdate, *nextUpdate;
    int iResp = 0;
    unsigned char *pResponse;
    unsigned char *pResponseNext;
   
    if(szConfigHost.length() > 0)
    {
        pszHost = (char *)OPENSSL_malloc(szConfigHost.length() + 1);
        memset(pszHost, 0, szConfigHost.length()  + 1);
        strcpy(pszHost, szConfigHost.c_str()); 
    }
    if(szConfigPort.length() > 0)
    {
        pszPort = (char *)OPENSSL_malloc(szConfigPort.length() + 1);
        memset(pszPort, 0, szConfigPort.length() + 1);
        strcpy(pszPort, szConfigPort.c_str()); 
    }

    tResult = SPC_OCSPRESULT_ERROR_UNKNOWN;
    if(pszHost == NULL || pszPort == NULL)
    {
        if (!OCSP_parse_url(data->url, &pszHost, &pszPort, &pszPath, &iSSL)) 
        {
            tResult = SPC_OCSPRESULT_ERROR_BADOCSPADDRESS;
            goto end;
        }
    }
    else
    {
        pszPath = (char *)OPENSSL_malloc(strlen(data->url) + 1);
        memset(pszPath, 0, strlen(data->url) + 1);
        strcpy(pszPath, data->url); 
    }

    if (!(pReq = OCSP_REQUEST_new())) 
    {
        tResult = SPC_OCSPRESULT_ERROR_OUTOFMEMORY;
        goto end;
    }
   
    pCertID = OCSP_cert_to_id(0, data->cert, data->issuer);
    if (!pCertID || !OCSP_request_add0_id(pReq, pCertID)) goto end;
    OCSP_request_add1_nonce(pReq, 0, -1);
   
    /* sign the request */
    if (data->sign_cert && data->sign_key &&
      !OCSP_request_sign(pReq, data->sign_cert, data->sign_key, EVP_sha1(), 0, 0)) 
    {
        tResult = SPC_OCSPRESULT_ERROR_SIGNFAILURE;
        goto end;
    }
   
    /* establish a connection to the OCSP responder */
    if (!(pBio = Connect(pszHost, atoi(pszPort), iSSL, &pSSLCtx))) 
    {
        tResult = SPC_OCSPRESULT_ERROR_CONNECTFAILURE;
        goto end;
    }
   
    /* send the request and get a response */
    pResp = OCSP_sendreq_bio(pBio, pszPath, pReq);
    if(pResp == NULL)
    {
        tResult = SPC_OCSPRESULT_ERROR_INVALIDRESPONSE;
        goto end;
    }

    if ((iRetCode = OCSP_response_status(pResp)) != OCSP_RESPONSE_STATUS_SUCCESSFUL) 
    {
        switch (iRetCode) 
        {
            case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
            tResult = SPC_OCSPRESULT_ERROR_MALFORMEDREQUEST; break;
            case OCSP_RESPONSE_STATUS_INTERNALERROR:
            tResult = SPC_OCSPRESULT_ERROR_INTERNALERROR; break;
            case OCSP_RESPONSE_STATUS_TRYLATER:
            tResult = SPC_OCSPRESULT_ERROR_TRYLATER; break;
            case OCSP_RESPONSE_STATUS_SIGREQUIRED:
            tResult = SPC_OCSPRESULT_ERROR_SIGREQUIRED; break;
            case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
            tResult = SPC_OCSPRESULT_ERROR_UNAUTHORIZED; break;
    }
    goto end;
  }
  
    /* verify the response */
    tResult = SPC_OCSPRESULT_ERROR_INVALIDRESPONSE;
    iResp = i2d_OCSP_RESPONSE(pResp, NULL);
    if(iResp > 0)
    {
        pResponse = pResponseNext = (unsigned char *)malloc(iResp);
        iResp = i2d_OCSP_RESPONSE(pResp, &pResponseNext);
        // pResponse is the result
        free(pResponse);
    }

    if (!(pBasic = OCSP_response_get1_basic(pResp))) goto end;
    if (OCSP_check_nonce(pReq, pBasic) <= 0) goto end;
    if ((iRetCode = OCSP_basic_verify(pBasic, 0, data->store, 0)) <= 0)
    {
        if(!oConfig.AllowTestRoot()) 
            goto end;
    }

    if (!OCSP_resp_find_status(pBasic, pCertID, &iStatus, &iReason, &producedAt,
                             &thisUpdate, &nextUpdate))
        goto end;

    /*if (!OCSP_check_validity(thisUpdate, nextUpdate, data->skew, data->maxage))
    {
        goto end;
    }
  */

    /* All done.  Set the return code based on the status from the response. */
    if (iStatus == V_OCSP_CERTSTATUS_REVOKED)
        tResult = SPC_OCSPRESULT_CERTIFICATE_REVOKED;
    else if(iStatus == V_OCSP_CERTSTATUS_UNKNOWN)
        tResult = SPC_OCSPRESULT_CERTIFICATE_UNKNOWN;
    else
        tResult = SPC_OCSPRESULT_CERTIFICATE_VALID;
  
end:
    if (pBio) BIO_free_all(pBio);
    if (pszHost) OPENSSL_free(pszHost);
    if (pszPort) OPENSSL_free(pszPort);
    if (pszPath) OPENSSL_free(pszPath);
    if (pReq) OCSP_REQUEST_free(pReq);
    if (pResp) OCSP_RESPONSE_free(pResp);
    if (pBasic) OCSP_BASICRESP_free(pBasic);
    if (pSSLCtx) SSL_CTX_free(pSSLCtx);
    return tResult;
}


X509* CVerify::FindIssuer(X509 *cert, X509_STORE *store)
{
  X509 *pIssuerRet = NULL;
  X509_STORE_CTX *ctx = 0;
  
  if ((ctx = X509_STORE_CTX_new()) != 0) 
  {
    if (X509_STORE_CTX_init(ctx, store, cert, 0) == 1)
    {
      // Check all issuers
      bool bOk = true;
      X509 *pCertCheck = cert;
      X509 *pIssuer = NULL;
      do
      {
        if(X509_STORE_CTX_get1_issuer(&pIssuer, ctx, pCertCheck))
        {
            if(pIssuerRet == NULL)
            {
                pIssuerRet = pIssuer;
            }
            if( 0 == strcmp(pIssuer->name, pCertCheck->name) )
            {
                bOk = false; // Break on Selfsigned
            }
            else
            {
                pCertCheck = pIssuer;
                pIssuer = NULL;
            }

        }
        else
        {
            // Lookup Issuer in additional cert store
            X509 *pFound = FindLocalIssuer(pCertCheck);
            if (pFound == NULL)
            {
                // Issuer certificate not found
                bOk = false;
            }
            else
            {
                // Add certificate to store
                X509_STORE_add_cert(store, pFound);
            }
        }
      } while (bOk);
    }
    X509_STORE_CTX_free(ctx);
  }

    return pIssuerRet;
}

int CVerify::VerifySignature(const unsigned char *pucData, unsigned long ulDataLen, 
                                 const unsigned char *pucSig, unsigned long ulSigLen)
{
    int iRet = BEID_SIGNATURE_PROCESSING_ERROR;
    EVP_MD_CTX cmd_ctx;
    EVP_PKEY *pKey = NULL;
    X509 *pX509 = NULL;
    int  iDiffRNCert = 1;
    eidlib::CCertif *pCertRN = NULL;
    const unsigned char *pucRNCert = NULL;
    unsigned long ulRNCertLen = 0;
    BEID_Certif_Check tCertifs = {0};

    if(m_pCertifManager == NULL)
    {
        return BEID_SIGNATURE_PROCESSING_ERROR;
    }
    pCertRN = m_pCertifManager->GetCertif("RN");
    if(pCertRN == NULL)
    {
        return BEID_SIGNATURE_PROCESSING_ERROR;
    }
    pucRNCert = pCertRN->GetData(); 
    ulRNCertLen = pCertRN->GetLength(); 

    m_pCertifManager->FillCertifs(&tCertifs);

    // Fill Certificate Store
    OpenSSL_add_all_algorithms();
    spc_x509store_t *pStore = new spc_x509store_t();
    InitX509Store(pStore);

    for(int i = 0; i < tCertifs.certificatesLength; ++i)
    {
        char *pID = tCertifs.certificates[i].certifLabel;
        if(0 != strcmp(pID, "RN"))
        {
            const unsigned char *pData = tCertifs.certificates[i].certif;
            unsigned long ulLen = tCertifs.certificates[i].certifLength; 
            if (!d2i_X509_Wrapper(&pX509, pData, ulLen))
                return BEID_SIGNATURE_PROCESSING_ERROR;
            X509StoreAddcert(pStore, pX509);
            pX509 = NULL;
        }
    }

    if (!d2i_X509_Wrapper(&pX509, pucRNCert, ulRNCertLen))
        return BEID_SIGNATURE_PROCESSING_ERROR;

/*
// For testing only : good RRN Certificate
        FILE *pf = fopen("rrn1.der", "r+b");
        unsigned char szBuffer[4096] = {0};
        int iread = 0;
        if(pf != NULL)
        {
            iread = fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), pf);
            fclose(pf);
        }
        unsigned char *pDummy = szBuffer;  
    if (NULL == (pX509 = d2i_X509(&pX509, (unsigned char **)&pDummy, iread)))
        return SIGNATURE_PROCESSING_ERROR;
*/

    if (NULL == (pKey = X509_get_pubkey(pX509)))
        return BEID_SIGNATURE_PROCESSING_ERROR;

    X509_STORE  *store = 0;
    store = CreateX509store(pStore);
    if(X509_V_OK == VerifyCertroot(pX509, store))
    {
        iDiffRNCert = 0;
    }
    X509_STORE_free(store);
    CleanupX509store(pStore);
    delete pStore;

    // Check OID
    if(0 == iDiffRNCert && X509_V_OK != VerifyRRNDN(pX509))
        return BEID_SIGNATURE_INVALID;

    EVP_VerifyInit(&cmd_ctx, EVP_sha1());
    EVP_VerifyUpdate(&cmd_ctx, pucData, ulDataLen);
    iRet = 2*iDiffRNCert + (1!=EVP_VerifyFinal(&cmd_ctx, (unsigned char *)pucSig, ulSigLen, pKey));
    EVP_PKEY_free(pKey);
    X509_free(pX509);
    return iRet;
}

void CVerify::UpdateCertStatus(const std::vector<eidlib::CCertif *> & Certifs, void *pCertX509, long lStatus)
{
    bool bFound = false;
    X509 *pX509 = NULL;
    for(int unsigned i = 0; i < Certifs.size() && !bFound; ++i)
    {
        eidlib::CCertif *pCertif = Certifs[i];
        if(pCertif != NULL)
        {
            const unsigned char *pData = pCertif->GetData();
            if (d2i_X509_Wrapper(&pX509, pData, pCertif->GetLength()))
            {
                if (X509_V_OK == X509_cmp((X509 *)pCertX509, pX509))
                {
                    bFound = true;
                    if(pCertif->GetCertStatus() == BEID_CERTSTATUS_CERT_NOT_VALIDATED)
                    {
                        pCertif->SetCertStatus(lStatus);
                    }
                }
                X509_free(pX509);
                pX509 = NULL;
            }
        }
    }
}

int CVerify::VerifyCallback(int ok, X509_STORE_CTX *store)
{
    X509 *pCert = store->current_cert;
    int iStatus = store->error;
#ifdef _DEBUG
    if (!ok)
    {
        printf("Error: %s\n", X509_verify_cert_error_string(store->error));
    }
#endif

    // Update Status in Certif
    if(NULL != m_pCertifManager)
    {
        m_pCertifManager->UpdateCertStatus(pCert, iStatus); 
    }
    return ok;
}


int CVerify::VerifyHash(const unsigned char *pucData, unsigned long ulDataLen, const unsigned char *pucHash)
{
    int iRet = BEID_SIGNATURE_PROCESSING_ERROR;
    EVP_MD_CTX cmd_ctx;
    X509 *pX509 = NULL;
    int  iDiffRNCert = 1;
    unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
    unsigned int md_len = 0;
    eidlib::CCertif *pCertRN = NULL;
    unsigned char *pucRNCert = NULL;
    unsigned long ulRNCertLen = 0;
    BEID_Certif_Check tCertifs = {0};

    if(m_pCertifManager == NULL)
    {
        return BEID_SIGNATURE_PROCESSING_ERROR;
    }
    pCertRN = m_pCertifManager->GetCertif("RN");
    if(pCertRN == NULL)
    {
        return BEID_SIGNATURE_PROCESSING_ERROR;
    }
    pucRNCert = pCertRN->GetData(); 
    ulRNCertLen = pCertRN->GetLength(); 

    m_pCertifManager->FillCertifs(&tCertifs);

    OpenSSL_add_all_algorithms();
    spc_x509store_t *pStore = new spc_x509store_t();
    InitX509Store(pStore);

    for(int i = 0; i < tCertifs.certificatesLength; ++i)
    {
        char *pID = tCertifs.certificates[i].certifLabel;
        if(0 != strcmp(pID, "RN"))
        {
            const unsigned char *pData = tCertifs.certificates[i].certif;
            unsigned long ulLen = tCertifs.certificates[i].certifLength; 
            if (!d2i_X509_Wrapper(&pX509, pData, ulLen))
                return BEID_SIGNATURE_PROCESSING_ERROR;
            X509StoreAddcert(pStore, pX509);
            pX509 = NULL;
        }
    }

    if (!d2i_X509_Wrapper(&pX509, pucRNCert, ulRNCertLen))
        return BEID_SIGNATURE_PROCESSING_ERROR;


    X509_STORE  *store = 0;
    store = CreateX509store(pStore);
    if(X509_V_OK == VerifyCertroot(pX509, store))
    {
        iDiffRNCert = 0;
    }
    X509_STORE_free(store);
    CleanupX509store(pStore);
    delete pStore;

    // Check OID
    if(0 == iDiffRNCert && X509_V_OK != VerifyRRNDN(pX509))
        return BEID_SIGNATURE_INVALID;

    EVP_DigestInit(&cmd_ctx, EVP_sha1());
    EVP_DigestUpdate(&cmd_ctx, pucData, ulDataLen);
    EVP_DigestFinal(&cmd_ctx, md_value, &md_len);

    X509_free(pX509);

    iRet =  2*iDiffRNCert + (memcmp(md_value, pucHash, md_len) == 0 ? BEID_SIGNATURE_VALID : BEID_SIGNATURE_INVALID);
    return iRet;
}


long CVerify::VerifyCRL(const std::vector<eidlib::CCertif *> & Certifs, bool bCRLDownload /* true*/)
{
    long lRet = 0;

    OpenSSL_add_all_algorithms();
    unsigned int i = 0;
    std::vector<std::string> CRLDistr;
    X509 *pX509 = NULL;
    X509_CRL *pX509Crl = NULL;
    spc_x509store_t *pStore = new spc_x509store_t();
    InitX509Store(pStore);
    X509StoreSetflags(pStore, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);
    X509StoreSetcallback(pStore, VerifyCallback);

    // Fill Certificate Store
    for(i = 0; i < Certifs.size(); ++i)
    {
        eidlib::CCertif *pCertif = Certifs[i];
        if(pCertif != NULL && BEID_CERTSTATUS_CERT_NOT_VALIDATED == pCertif->GetCertStatus())
        {
            const unsigned char *pData = pCertif->GetData();
            if (!d2i_X509_Wrapper(&pX509, pData, pCertif->GetLength()))
                return BEID_SIGNATURE_PROCESSING_ERROR;
            X509StoreAddcert(pStore, pX509);
            char *pCRLDistribution = GetExtensionValue(pX509, NID_crl_distribution_points);
            
            if(NULL != pCRLDistribution)
            {
                bool bStop = false;
                for(unsigned int iNr = 0; iNr < CRLDistr.size(); ++iNr)
                {
                    if(0 == stricmp(CRLDistr[iNr].c_str(), pCRLDistribution))
                    {
                        bStop = true;
                        break;
                    }
                }
                if(!bStop)
                {
                    CRLDistr.push_back(pCRLDistribution);
                    if(NULL != (pX509Crl = FindLocalCRL(pCRLDistribution, bCRLDownload)))
                    {
                        X509StoreAddCRL(pStore, pX509Crl);
                        pX509Crl = NULL;
                    }
                }
                free(pCRLDistribution);
            }
            pX509 = NULL;
        }
    }

    //////////////////////////////////////////////////
    /// For testing only

   /* unsigned char *pDummy = NULL;
    // Add Root Cert
    for (i = 0; i < ROOT_CERT_COUNT; ++i)
    {
        pDummy = (unsigned char*)_root_cert[i];
        if (NULL == (pX509 = d2i_X509(&pX509, &pDummy, ROOT_CERT_SIZE)))
            return BEID_SIGNATURE_PROCESSING_ERROR;
        X509StoreAddcert(pStore, pX509);
        pX509 = NULL;
    }
*/

    //////////////////////////////////////////////////
    /// For testing only
    //Add rrncerts
 /*   {    
    unsigned char *pDummy = NULL;
        FILE *pf = fopen("rrn1.der", "r+b");
        unsigned char szBuffer[4096] = {0};
        int iread = 0;
        if(pf != NULL)
        {
            iread = fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), pf);
            fclose(pf);
        }
        pDummy = szBuffer;
    if (NULL == (pX509 = d2i_X509(&pX509, (unsigned char **)&pDummy, iread)))
        return BEID_SIGNATURE_PROCESSING_ERROR;
            X509StoreAddcert(pStore, pX509);
            
            char *pCRLDistribution = GetExtensionValue(pX509, "crlDistributionPoints", "URI");
            if(NULL != pCRLDistribution)
            {
                bool bStop = false;
                for(int iNr = 0; iNr < CRLDistr.size(); ++iNr)
                {
                    if(0 == stricmp(CRLDistr[iNr].c_str(), pCRLDistribution))
                    {
                        bStop = true;
                        break;
                    }
                }
                if(!bStop)
                {
                    CRLDistr.push_back(pCRLDistribution);
                    if(NULL != (pX509Crl = FindLocalCRL(pCRLDistribution)))
                    {
                        X509StoreAddCRL(pStore, pX509Crl);
                        pX509Crl = NULL;
                    }
                }
                free(pCRLDistribution);
            }
            pX509 = NULL;
    }
*/

  X509_STORE  *store = 0;
  store = CreateX509store(pStore);

    for(i = 0; i < (unsigned int)sk_X509_num(pStore->certs); ++i)
    {
        X509 *pValue = sk_X509_value(pStore->certs, i);
        if(pValue != NULL)
        {
            VerifyCert(pValue, store, CRLDistr, bCRLDownload);
            pValue = NULL;
        }
    }
   X509_STORE_free(store);
   CRLDistr.clear();

    CleanupX509store(pStore);
    delete pStore;

    return lRet;
}


long CVerify::VerifyOCSP(const std::vector<eidlib::CCertif *> & Certifs)
{
    long lRet = 0;

    OpenSSL_add_all_algorithms();
    unsigned int i = 0;

    X509 *pX509 = NULL;
    spc_x509store_t *pStore = new spc_x509store_t();
    InitX509Store(pStore);
    X509StoreSetcallback(pStore, VerifyCallback);

    // Fill Certificate Store
    for(i = 0; i < Certifs.size(); ++i)
    {
        eidlib::CCertif *pCertif = Certifs[i];
        if(pCertif != NULL && BEID_CERTSTATUS_CERT_NOT_VALIDATED == pCertif->GetCertStatus())
        {
            const unsigned char *pData = pCertif->GetData();
            if (!d2i_X509_Wrapper(&pX509, pData, pCertif->GetLength()))
                return BEID_SIGNATURE_PROCESSING_ERROR;
            X509StoreAddcert(pStore, pX509);
            pX509 = NULL;
        }
    }

    //////////////////////////////////////////////////
    /// For testing only
/*
    unsigned char *pDummy = NULL;
    // Add Root Cert
    for (i = 0; i < ROOT_CERT_COUNT; ++i)
    {
        pDummy = (unsigned char*)_root_cert[i];
        if (NULL == (pX509 = d2i_X509(&pX509, &pDummy, ROOT_CERT_SIZE)))
            return SIGNATURE_PROCESSING_ERROR;
        spc_x509store_addcert(pStore, pX509);
        pX509 = NULL;
    }
*/

    //////////////////////////////////////////////////
    /// For testing only
    //Add rrncerts
/*    {   
        unsigned char *pDummy = NULL;
        FILE *pf = fopen("rrn1.der", "r+b");
        unsigned char szBuffer[4096] = {0};
        int iread = 0;
        if(pf != NULL)
        {
            iread = fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), pf);
            fclose(pf);
        }
        pDummy = szBuffer;
    if (NULL == (pX509 = d2i_X509(&pX509, (unsigned char **)&pDummy, iread)))
        return SIGNATURE_PROCESSING_ERROR;
            spc_x509store_addcert(pStore, pX509);
        pX509 = NULL;
    }
*/
  /*          if(NULL != (pX509Crl = FindLocalCRL(pX509)))
            {
                spc_x509store_addcrl(pStore, pX509Crl);
            }
            pX509Crl = NULL;
    }
*/

    X509_STORE  *store = 0;
    store = CreateX509store(pStore);

    spc_ocspresult_t tResult;
    spc_ocsprequest_t tRequest = {0};
    tRequest.store = store;
    tRequest.skew = 5;
    tRequest.maxage = 1;

    bool bContinue = true;

    for(i = 0; i < (unsigned int)sk_X509_num(pStore->certs) && bContinue; ++i)
    {
        X509 *pValue = sk_X509_value(pStore->certs, i);
        if(pValue != NULL)
        {
            int iStatus = BEID_CERTSTATUS_CERT_NOT_VALIDATED;
            X509 *pIssuer = FindIssuer(pValue, store);
            if(pIssuer != NULL)
            {
                char *pOCSPResponder = NULL;
                // Get OCSP Responder
                pOCSPResponder = GetExtensionValue(pValue, NID_info_access);
                
                if(NULL != pOCSPResponder)
                {
                    tRequest.url = pOCSPResponder;
                    tRequest.cert = pValue;
                    tRequest.issuer = pIssuer;
                    tResult = VerifyViaOCSP(&tRequest, pStore);
                    // Update result
                    switch(tResult)
                    {
                        case SPC_OCSPRESULT_CERTIFICATE_VALID: iStatus = BEID_CERTSTATUS_CERT_VALIDATED_OK; break;
                        case SPC_OCSPRESULT_CERTIFICATE_REVOKED: iStatus = BEID_CERTSTATUS_CERT_REVOKED; break;
                        case SPC_OCSPRESULT_CERTIFICATE_UNKNOWN: iStatus = BEID_CERTSTATUS_CERT_UNKNOWN; break;
                        default: lRet = -1; bContinue = false; // Error occured
                    }
                    m_pCertifManager->UpdateCertStatus(pValue, iStatus); 
                    free(pOCSPResponder);
                }
                X509_free(pIssuer);
            }
            else
            {
                m_pCertifManager->UpdateCertStatus(pValue, BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT_LOCALLY); 
            }
            pValue = NULL;
        }
    }
    X509_STORE_free(store);

    CleanupX509store(pStore);
    delete pStore;

    return lRet;
}

int CVerify::VerifyOID(X509 *pCert)
{
    int iRet = -1;
    int j = 0;
    STACK_OF(POLICYINFO) *pol = NULL;
    POLICYINFO *pinfo = NULL;
    BOOL bFound = FALSE;

    pol = (struct stack_st *) X509_get_ext_d2i(pCert, NID_certificate_policies, NULL, NULL);

    if(pol != NULL)
    {
        for(j = 0; j < sk_POLICYINFO_num(pol); j++) 
        {
            pinfo = sk_POLICYINFO_value(pol, j);
            if(pinfo != NULL)
            {
                char szBuffer[64] = {0};
                OBJ_obj2txt(szBuffer, 64, pinfo->policyid, 0);
                if (0 == strcmp(szBuffer, RRN_OID1) || 0 == strcmp(szBuffer, RRN_OID2)  || 0 == strcmp(szBuffer, RRN_OID3))
                {
                    iRet = X509_V_OK;
                    bFound = TRUE;
                    break;
                }
            }
        }
    }

    if(pol !=NULL)
    {
        sk_POLICYINFO_free(pol);
    }
    return iRet;
}

int CVerify::VerifyRRNDN(X509 *pCert)
{
    int iRet = -1;

    if(pCert != NULL)
    {
        char szName[256] = {0};
        char szTemp[128] = {0};
        X509_NAME_get_text_by_NID(X509_get_subject_name(pCert), NID_commonName, szTemp, sizeof(szTemp));
        strcat(szName, szTemp);
        memset(szTemp, 0, sizeof(szTemp));
        X509_NAME_get_text_by_NID(X509_get_subject_name(pCert), NID_organizationName, szTemp, sizeof(szTemp));
        strcat(szName, szTemp);
        memset(szTemp, 0, sizeof(szTemp));
        X509_NAME_get_text_by_NID(X509_get_subject_name(pCert), NID_countryName, szTemp, sizeof(szTemp));
        strcat(szName, szTemp);
        if (0 == strcmp(szName, DN_RRN))
        {
            iRet = X509_V_OK;
        }
    }
    return iRet;
}

bool CVerify::RetrieveData(const std::string & strUri, eidcommon::CByteArray & oData)
{
    return m_oDownLoad.Download(strUri, oData);
}

