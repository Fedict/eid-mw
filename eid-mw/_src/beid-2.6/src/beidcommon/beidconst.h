#ifndef __BEIDCONST_H__
#define __BEIDCONST_H__

#include "eidlib.h"

const unsigned char g_ucMF[] = {0x3F, 0x00};
const unsigned char g_ucBelpicAID[] = {0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};
const unsigned char g_ucIDAID[] = {0xA0, 0x00, 0x00, 0x01, 0x77, 0x49, 0x64, 0x46, 0x69, 0x6C, 0x65, 0x73};
static BEID_Bytes gtBelpicAID = {(unsigned char *)g_ucBelpicAID, sizeof(g_ucBelpicAID)};
static BEID_Bytes gtIDAID = {(unsigned char *)g_ucIDAID, sizeof(g_ucIDAID)};

const unsigned char g_ucDataTag = 0x40;
const unsigned char g_ucCertTag = 0x50;

const unsigned char g_ucDataTagID = 0x31;
const unsigned char g_ucDataTagIDSIG = 0x32;
const unsigned char g_ucDataTagADDR = 0x33;
const unsigned char g_ucDataTagADDRSSIG = 0x34;
const unsigned char g_ucDataTagPHOTO = 0x35;

const unsigned char g_ucDataTagTOKENINFO = 0x32;
const unsigned char g_ucDataTagRN = 0x3C;
const unsigned char g_ucDataTagCA = 0x3A;
const unsigned char g_ucDataTagROOT = 0x3B;
const unsigned char g_ucDataTagAUTH = 0x38;
const unsigned char g_ucDataTagSIG = 0x39;


// Certificates
const unsigned char g_ucDFCert[] = {0xDF, 0x00};
const unsigned char g_ucRN[] = {g_ucCertTag, g_ucDataTagRN};
const unsigned char g_ucCA[] = {g_ucCertTag, g_ucDataTagCA};
const unsigned char g_ucROOT[] = {g_ucCertTag, g_ucDataTagROOT};
const unsigned char g_ucAUTH[] = {g_ucCertTag, g_ucDataTagAUTH};
const unsigned char g_ucSIG[] = {g_ucCertTag, g_ucDataTagSIG};
const unsigned char g_ucTokenInfo[] = {g_ucCertTag, g_ucDataTagTOKENINFO};

// ID Files
const unsigned char g_ucDFID[] = {0xDF, 0x01};
const unsigned char g_ucID[] = {g_ucDataTag, g_ucDataTagID};
const unsigned char g_ucSIGID[] = {g_ucDataTag, g_ucDataTagIDSIG};
const unsigned char g_ucADDR[] = {g_ucDataTag, g_ucDataTagADDR};
const unsigned char g_ucSIGADDR[] = {g_ucDataTag, g_ucDataTagADDRSSIG};
const unsigned char g_ucPHOTO[] = {g_ucDataTag, g_ucDataTagPHOTO};


static const char *g_ucRNLabel = "RN";

#define TOKENINFO_OFFSET 0x25


/******************************************
 Change these consts when a card version changes
******************************************/
#define EIDLIB_CURRENT_GLOBALOS_VERSION 0x0003
#define EIDLIB_CURRENT_ELECTRO_PERSO_ITF 0x00
#define EIDLIB_CURRENT_EF_ID 0x00
#define EIDLIB_CURRENT_EF_ADDRESS 0x00
 

/******************************************
 Change these consts for a new release
******************************************/
#ifdef _WIN32
    #define EIDLIB_CURRENT_VERSION "2.5.9"
    #pragma message ( "*** Current version is " EIDLIB_CURRENT_VERSION " ***") 
#endif
#ifdef linux
    #define EIDLIB_CURRENT_VERSION "2.5.9"
#endif
#ifdef __APPLE__
    #define EIDLIB_CURRENT_VERSION "2.5.1"
#endif

struct tAtrTable 
{
	const unsigned char *atr;
	long atr_len;
};

static struct tAtrTable BelpicAtrs[] = 
{
	/* Applet V2 */
	{ (const unsigned char *) "\x3B\x98\x95\x40\x0A\xA5\x07\x01\x01\x01\xAD\x13\x20", 13 },
	/* Belpic Applet V1.1 */
	{ (const unsigned char *) "\x3B\x98\x13\x40\x0A\xA5\x03\x01\x01\x01\xAD\x13\x11", 13 },
	/* Final with new EMV-compatible ATR */
	{ (const unsigned char *) "\x3B\x98\x94\x40\x0A\xA5\x03\x01\x01\x01\xAD\x13\x10", 13 },
	/* Belpic Applet V5 + Final */
	{ (const unsigned char *) "\x3B\x98\x94\x40\xFF\xA5\x03\x01\x01\x01\xAD\x13\x10", 13 },
	{ NULL, 0 }
};

#endif // __BEIDCONST_H__
