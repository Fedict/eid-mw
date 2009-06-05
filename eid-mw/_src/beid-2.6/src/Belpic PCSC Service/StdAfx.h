// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../beidcommon/funcs.h"
#include <winscard.h>
#include <qmutex.h>
#include <qthread.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include <qregexp.h>

#ifdef _WIN32
#pragma warning(disable:4786 4018)
#endif

#include <vector>
#include <map>
#include <string>

#include "../beidcommlib/beidcommlib.h"

#ifndef _WIN32
    #define MAXIMUM_SMARTCARD_READERS  10   // Limit the readers on the system
    #define SCARD_E_NO_ACCESS  0x80100027
#endif

#ifdef _DEBUGLOG
    void DebugLogMessage(const std::string & strMsg);
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
