/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CFileVersionInfo - Class for getting file version information
*/
/////////////////////////////////////////////////////////////////////////////

#ifndef __VERINFO_H__ 
#define __VERINFO_H__

#if defined( _MSC_VER ) && ( _MSC_VER >= 1020 )
	#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef	_INC_SHLWAPI
	#include < Shlwapi.h >
#endif
#pragma comment( lib, "shlwapi.lib" )

#ifndef VER_H
	#include < WinVer.h >
#endif
#pragma comment( lib, "Version.lib " )

#include "wx/string.h"

/////////////////////////////////////////////////////////////////////////////


#ifndef	_free
	#define _free( p )			{ if( p != NULL ){ free( p ); p = NULL; } }
#endif


/////////////////////////////////////////////////////////////////////////////

#ifndef DLLVER_MAJOR_MASK

	typedef struct _DLLVERSIONINFO2 
    {
		DLLVERSIONINFO	info1;
		DWORD			dwFlags;				// No flags currently defined
		ULONGLONG		ullVersion;
	} DLLVERSIONINFO2;

#endif // DLLVER_MAJOR_MASK

#ifndef MAKEDLLVERULL

	#define MAKEDLLVERULL( major, minor, build, qfe )\
		( ( (ULONGLONG)(major) << 48 ) |			 \
		(   (ULONGLONG)(minor) << 32 ) |			 \
		(   (ULONGLONG)(build) << 16 ) |			 \
		(   (ULONGLONG)(  qfe) <<  0 ) )

#endif // MAKEDLLVERULL


/////////////////////////////////////////////////////////////////////////////

typedef enum _VI_CP 
{
	VI_CP_ASCII				= 0,	// 7-bit ASCII 
	VI_CP_JAPAN				= 932,	// Japan (Shift - JIS X-0208) 
	VI_CP_KOREA				= 949,	// Korea (Shift - KSC 5601) 
	VI_CP_TAIWAN			= 950,	// Taiwan (Big5) 
	VI_CP_UNICODE			= 1200,	// Unicode 
	VI_CP_LATIN2			= 1250,	// Latin-2 (Eastern European) 
	VI_CP_CYRILLIC			= 1251,	// Cyrillic 
	VI_CP_MULTILNG			= 1252,	// Multilingual 
	VI_CP_GREEK				= 1253,	// Greek 
	VI_CP_TURKISH			= 1254,	// Turkish 
	VI_CP_HEBREW			= 1255,	// Hebrew 
	VI_CP_ARABIC			= 1256	// Arabic 
} VI_CP;

typedef enum _VI_STR 
{
	VI_STR_COMMENTS			= 0,	// Comments
	VI_STR_COMPANYNAME		= 1,	// CompanyName
	VI_STR_FILEDESCRIPTION	= 2,	// FileDescription
	VI_STR_FILEVERSION		= 3,	// FileVersion
	VI_STR_INTERNALNAME		= 4,	// InternalName
	VI_STR_LEGALCOPYRIGHT	= 5,	// LegalCopyright
	VI_STR_LEGALTRADEMARKS	= 6,	// LegalTrademarks
	VI_STR_ORIGINALFILENAME	= 7,	// OriginalFilename
	VI_STR_PRIVATEBUILD		= 8,	// PrivateBuild
	VI_STR_PRODUCTNAME		= 9,	// ProductName
	VI_STR_PRODUCTVERSION	= 10,	// ProductVersion
	VI_STR_SPECIALBUILD		= 11,	// SpecialBuild
	VI_STR_OLESELFREGISTER	= 12	// OLESelfRegister
} VI_STR;


class CFileVersionInfo
{
public:	// Construction/destruction:

	CFileVersionInfo( void );	
	virtual ~CFileVersionInfo( void );

public:	// Implementation:
	
	BOOL	Open(const wxString & szFileName );
	void	  Close( void );

	BOOL	QueryStringValue(const wxString & szItem, wxString & szValue) const;
	BOOL	QueryStringValue(INT nIndex, wxString & szValue) const;
	wxString GetVerStringName(INT nIndex );	

	BOOL	SetTrans (LANGID wLID = LANG_NEUTRAL, WORD wCP = VI_CP_UNICODE );	
	BOOL	SetTransIndex(UINT nIndex = 0 );
	INT		FindTrans( LANGID wLID, WORD wCP ) const;
	DWORD	GetTransByIndex( UINT nIndex  ) const;	

public: // Static members:
	
	static BOOL		GetLIDName( WORD wLID, wxString & szName );
	static BOOL		GetCPName(  WORD wCP,  wxString & szName );
	
public: 

	const	VS_FIXEDFILEINFO& GetVSFFI( void ) const;		
	BOOL		IsValid( void ) const;

	WORD		GetFileVersionMajor( void ) const;
	WORD		GetFileVersionMinor( void ) const;
	WORD		GetFileVersionBuild( void ) const;
	WORD		GetFileVersionQFE( void )   const;

	WORD		GetProductVersionMajor( void ) const;
	WORD		GetProductVersionMinor( void ) const;
	WORD		GetProductVersionBuild( void ) const;
	WORD		GetProductVersionQFE( void )   const;
	
	UINT		GetTransCount( void ) const;
	UINT		GetCurTransIndex( void ) const;		
	LANGID	GetLIDByIndex(  UINT nIndex ) const;	
	WORD		GetCPByIndex(  UINT nIndex ) const;	
	DWORD	GetCurTrans( void ) const;		
	LANGID	GetCurLID( void ) const;	
	WORD		GetCurCP( void ) const;

protected:

	BOOL	GetVersionInfo( const wxString & szFileName  );
	BOOL	QueryVersionTrans( void );

protected: // Members variables
	
	static wxString s_ppszStr[ 13 ];	// String names
	VS_FIXEDFILEINFO m_vsffi;			// Fixed File Info (FFI)

	LPBYTE		m_lpbyVIB;		// Pointer to version info block (VIB)
	LPDWORD		m_lpdwTrans;	// Pointer to translation array in m_lpbyVIB, LOWORD = LangID and HIWORD = CodePage	
	UINT		m_nTransCur;	// Current translation index
	UINT		m_nTransCnt;	// Translations count
	BOOL		m_bValid;		// Version info is loaded	
};
	
/////////////////////////////////////////////////////////////////////////////
#endif //__VER_H__
/////////////////////////////////////////////////////////////////////////////
