#ifndef __BEID_VERSION_H__
#define __BEID_VERSION_H__

/**
 * Versions for the Windows binaries
 */

#include "./svn_revision.h"

// To specified in the .rc files

#define BEID_COMPANY_NAME    	"Belgian Government"
#define BEID_COPYRIGHT    	"Copyright (C) 2013"
#define BEID_PRODUCT_NAME    	"Belgium eID MiddleWare"

#define BEID_PRODUCT_VERSION    "4.1.0"
#define BASE_VERSION_STRING    "4, 1, 0, "
#define BASE_VERSION1          4
#define BASE_VERSION2          1
#define BASE_VERSION3          0

// Common Lib
#define WIN_CL_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_CL_VERSION1          BASE_VERSION1          
#define WIN_CL_VERSION2          BASE_VERSION2          
#define WIN_CL_VERSION3          BASE_VERSION3          
#define WIN_CL_VERSION4          SVN_REVISION

// Card Abstraction
#define WIN_CAL_VERSION_STRING   BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_CAL_VERSION1         BASE_VERSION1          
#define WIN_CAL_VERSION2         BASE_VERSION2          
#define WIN_CAL_VERSION3         BASE_VERSION3          
#define WIN_CAL_VERSION4         SVN_REVISION

// Dialogs
#define WIN_DLG_VERSION_STRING   BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_DLG_VERSION1         BASE_VERSION1          
#define WIN_DLG_VERSION2         BASE_VERSION2          
#define WIN_DLG_VERSION3         BASE_VERSION3          
#define WIN_DLG_VERSION4         SVN_REVISION

// CSP
#define WIN_CSP_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_CSP_VERSION1          BASE_VERSION1          
#define WIN_CSP_VERSION2          BASE_VERSION2          
#define WIN_CSP_VERSION3          BASE_VERSION3          
#define WIN_CSP_VERSION4          SVN_REVISION

// MDRV
#define WIN_MDRV_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_MDRV_VERSION1          BASE_VERSION1          
#define WIN_MDRV_VERSION2          BASE_VERSION2          
#define WIN_MDRV_VERSION3          BASE_VERSION3          
#define WIN_MDRV_VERSION4          SVN_REVISION

// cardplugin BEID
#define WIN_CPBEID_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_CPBEID_VERSION1          BASE_VERSION1          
#define WIN_CPBEID_VERSION2          BASE_VERSION2          
#define WIN_CPBEID_VERSION3          BASE_VERSION3          
#define WIN_CPBEID_VERSION4          SVN_REVISION

// PKCS11
#define WIN_PKCS11_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_PKCS11_VERSION1          BASE_VERSION1          
#define WIN_PKCS11_VERSION2          BASE_VERSION2          
#define WIN_PKCS11_VERSION3          BASE_VERSION3          
#define WIN_PKCS11_VERSION4          SVN_REVISION

// CLEANUPTOOL
#define WIN_CLEAN_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_CLEAN_VERSION1          BASE_VERSION1          
#define WIN_CLEAN_VERSION2          BASE_VERSION2          
#define WIN_CLEAN_VERSION3          BASE_VERSION3          
#define WIN_CLEAN_VERSION4          SVN_REVISION

// SCCERTPROP
#define WIN_SCCERT_VERSION_STRING    BASE_VERSION_STRING SVN_REVISION_STR
#define WIN_SCCERT_VERSION1          BASE_VERSION1          
#define WIN_SCCERT_VERSION2          BASE_VERSION2          
#define WIN_SCCERT_VERSION3          BASE_VERSION3          
#define WIN_SCCERT_VERSION4          SVN_REVISION

#endif //__BEID_VERSION_H__
