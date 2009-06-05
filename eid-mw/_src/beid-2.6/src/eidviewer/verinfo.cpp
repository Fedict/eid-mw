/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CFileVersionInfo - Class for getting file version information
*/
/////////////////////////////////////////////////////////////////////////////

#include "verinfo.h"

/////////////////////////////////////////////////////////////////////////////
// HIWORD( ffi.dwFileVersionMS ) - major
// LOWORD( ffi.dwFileVersionMS ) - minor
// HIWORD( ffi.dwFileVersionLS ) - build
// LOWORD( ffi.dwFileVersionLS ) - QFE
/////////////////////////////////////////////////////////////////////////////

CFileVersionInfo::CFileVersionInfo( void )
				: m_lpbyVIB( NULL )
{		
	Close();
}

CFileVersionInfo::~CFileVersionInfo( void )
{
	Close();
}

wxString CFileVersionInfo::s_ppszStr[] = { 
	wxT( "Comments" ),			wxT( "CompanyName" ),      
	wxT( "FileDescription" ),	wxT( "FileVersion" ),
	wxT( "InternalName" ),		wxT( "LegalCopyright" ),
	wxT( "LegalTrademarks" ),	wxT( "OriginalFilename" ),
	wxT( "PrivateBuild" ),		wxT( "ProductName" ),
	wxT( "ProductVersion" ),		wxT( "SpecialBuild" ),
	wxT( "OLESelfRegister" )
};

////////////////////////////////////////////////////////////////////////////////
// Implementation

BOOL CFileVersionInfo::Open( const wxString & szFileName )
{
	if( szFileName.Length() == 0 )
		return( FALSE );

	Close();
	if( !GetVersionInfo( szFileName ) || !QueryVersionTrans() )
		Close();

	return m_bValid;
};

BOOL CFileVersionInfo::GetVersionInfo(  const wxString & szFileName )
{
	DWORD dwDummy = 0;
	DWORD dwSize  = ::GetFileVersionInfoSizeA(szFileName.mb_str(), &dwDummy);

	if ( dwSize > 0 )
	{		
		m_lpbyVIB = (LPBYTE)malloc( dwSize );

		if ( m_lpbyVIB != NULL && ::GetFileVersionInfoA(szFileName.mb_str(), 0, dwSize, m_lpbyVIB ) )
		{
			UINT   uLen    = 0;
			LPVOID lpVSFFI = NULL;
			
			if ( ::VerQueryValueA( m_lpbyVIB,  "\\", (LPVOID*)&lpVSFFI, &uLen ) )
			{
				::CopyMemory( &m_vsffi, lpVSFFI, sizeof( VS_FIXEDFILEINFO ) );
				m_bValid = ( m_vsffi.dwSignature == VS_FFI_SIGNATURE );
			}
		}
	}

	return m_bValid;
}

BOOL CFileVersionInfo::QueryVersionTrans( void )
{
	if( m_bValid == FALSE )
		return( FALSE );

	UINT   uLen  = 0;
	LPVOID lpBuf = NULL;
  
	if( ::VerQueryValueA( m_lpbyVIB,  "\\VarFileInfo\\Translation", (LPVOID*)&lpBuf, &uLen ) )
	{
		m_lpdwTrans = (LPDWORD)lpBuf;
		m_nTransCnt = ( uLen / sizeof( DWORD ) );		
	}	
	return (BOOL)( m_lpdwTrans != NULL );
}

void CFileVersionInfo::Close( void )
{
	m_nTransCnt  = 0;
	m_nTransCur  = 0;
	m_bValid	 = FALSE;	
	m_lpdwTrans  = NULL;
		
	::ZeroMemory( &m_vsffi, sizeof( VS_FIXEDFILEINFO ) );
	_free( m_lpbyVIB );
}

BOOL CFileVersionInfo::QueryStringValue( const wxString & szItem,
										  wxString &  szValue) const
{
	if( m_bValid  == FALSE || szItem.Length() == 0 )
		return( FALSE );
	
    szValue.Empty();

	char szSFI[ MAX_PATH ] = { 0 };
	sprintf( szSFI,  "\\StringFileInfo\\%04X%04X\\%s", GetCurLID(), GetCurCP(), szItem );

	BOOL   bRes    = FALSE;
	UINT   uLen    = 0;
	char *lpszBuf = NULL;

	if( ::VerQueryValueA( m_lpbyVIB, szSFI, (LPVOID*)&lpszBuf, &uLen ) )
	{
        szValue = wxString::Format(wxT("%s"), lpszBuf);
    	bRes = TRUE;
	}
	
	return ( bRes );
}

BOOL CFileVersionInfo::QueryStringValue(   INT    nIndex, 
										  wxString &  szValue ) const
{
	if( nIndex < VI_STR_COMMENTS || 
		nIndex > VI_STR_OLESELFREGISTER ) 
	{ 
		return( FALSE );
	}	
	return QueryStringValue( s_ppszStr[ nIndex ], szValue );
}

wxString CFileVersionInfo::GetVerStringName(  INT nIndex )
{ 
	if( nIndex < VI_STR_COMMENTS || 
		nIndex > VI_STR_OLESELFREGISTER ) 
	{ 
		return( wxT("") );
	}
	return s_ppszStr[ nIndex ];
}

INT CFileVersionInfo::FindTrans(  LANGID wLID,
								  WORD   wCP ) const
{	
	if( m_bValid == FALSE )
		return( -1 );

	for( UINT n = 0; n < m_nTransCnt; n++ )
	{
		if( LOWORD( m_lpdwTrans[ n ] ) == wLID && 
			HIWORD( m_lpdwTrans[ n ] ) == wCP  )
		{		
			return n;
		}
	}
	return -1;
}

BOOL CFileVersionInfo::SetTrans(  LANGID wLID /*LANG_NEUTRAL*/, 
								  WORD   wCP  /*WSLVI_CP_UNICODE*/ )
{	
	if( m_bValid == FALSE )
		return( FALSE );

	if( GetCurLID() == wLID && GetCurCP() == wCP )
		return TRUE;

	INT nPos = FindTrans( wLID, wCP );
	if( nPos != -1 ) m_nTransCur = nPos;

	return ( m_nTransCur == (UINT)nPos );
}

DWORD CFileVersionInfo::GetTransByIndex(  UINT nIndex ) const
{
	if( m_bValid == FALSE || nIndex < 0 || nIndex > m_nTransCnt )
		return( 0 );

	return m_lpdwTrans[ nIndex ];
}

BOOL CFileVersionInfo::SetTransIndex(  UINT nIndex /*0*/ )
{
	if( m_bValid == FALSE )
		return( FALSE );

	if( m_nTransCur == nIndex )
		return TRUE;

	if( nIndex >= 0 && nIndex <= m_nTransCnt )
		m_nTransCur = nIndex;
	
	return ( m_nTransCur == nIndex );
}

/////////////////////////////////////////////////////////////////////////////
// Static members

// If the LID identifier is unknown, it returns a 
// default string ("Language Neutral"):

BOOL CFileVersionInfo::GetLIDName(   WORD   wLID, 
								    wxString & szName)
{
    BOOL bRet = FALSE;
    char pszName[256] = {0};
    bRet = (BOOL)::VerLanguageNameA( wLID, pszName, sizeof(pszName) ) ;
    if(bRet)
    {
        szName = wxString::Format(wxT("%s"), pszName);
    }
    return bRet;
}

// If the CP identifier is unknown, it returns a 
// default string ("Unknown"):

BOOL CFileVersionInfo::GetCPName(   WORD	   wCP,
								   wxString & szName )
{
	BOOL bRes = TRUE;	

	switch ( wCP )
	{
		case VI_CP_ASCII:	 szName = wxT( "7-bit ASCII" );				break;
		case VI_CP_JAPAN:	 szName = wxT( "Japan (Shift – JIS X-0208)" );break;
		case VI_CP_KOREA:	 szName = wxT( "Korea (Shift – KSC 5601)" );	break;
		case VI_CP_TAIWAN:	 szName = wxT( "Taiwan (Big5)" );				break;
		case VI_CP_UNICODE:	 szName = wxT( "Unicode" );					break;
		case VI_CP_LATIN2:	 szName = wxT( "Latin-2 (Eastern European)" );break;
		case VI_CP_CYRILLIC: szName = wxT( "Cyrillic" );					break;
		case VI_CP_MULTILNG: szName = wxT( "Multilingual" );				break;
		case VI_CP_GREEK:	 szName = wxT( "Greek" );						break;
		case VI_CP_TURKISH:	 szName = wxT( "Turkish" );					break;
		case VI_CP_HEBREW:	 szName = wxT( "Hebrew" );					break;
		case VI_CP_ARABIC:	 szName = wxT( "Arabic" );					break;		
		default:			 szName = wxT( "Unknown" ); bRes = FALSE;		break;
	}
	return bRes;
}
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL CFileVersionInfo::IsValid( void ) const
	{ return m_bValid; }

const VS_FIXEDFILEINFO& CFileVersionInfo::GetVSFFI( void ) const
	{ return m_vsffi; }

////////////////////////////////////////////////////////////////////////////////

WORD CFileVersionInfo::GetFileVersionMajor( void ) const
	{  return HIWORD( m_vsffi.dwFileVersionMS ); }

WORD CFileVersionInfo::GetFileVersionMinor( void ) const
	{  return LOWORD( m_vsffi.dwFileVersionMS ); }

WORD CFileVersionInfo::GetFileVersionBuild( void ) const
	{  return HIWORD( m_vsffi.dwFileVersionLS ); }

WORD CFileVersionInfo::GetFileVersionQFE( void ) const
	{  return LOWORD( m_vsffi.dwFileVersionLS ); }

WORD CFileVersionInfo::GetProductVersionMajor( void ) const
	{  return HIWORD( m_vsffi.dwProductVersionMS ); }

WORD CFileVersionInfo::GetProductVersionMinor( void ) const
	{  return LOWORD( m_vsffi.dwProductVersionMS ); }

WORD CFileVersionInfo::GetProductVersionBuild( void ) const
	{  return HIWORD( m_vsffi.dwProductVersionLS ); }

WORD CFileVersionInfo::GetProductVersionQFE( void ) const
	{  return LOWORD( m_vsffi.dwProductVersionLS ); }

////////////////////////////////////////////////////////////////////////////////
// Translation functions

UINT CFileVersionInfo::GetTransCount( void ) const
	{ return m_nTransCnt; }

UINT CFileVersionInfo::GetCurTransIndex( void ) const
	{  return m_nTransCur; }

LANGID CFileVersionInfo::GetLIDByIndex(  UINT nIndex  ) const
	{ return LOWORD( GetTransByIndex( nIndex ) ); }	

WORD CFileVersionInfo::GetCPByIndex(  UINT nIndex  ) const
	{ return HIWORD( GetTransByIndex( nIndex ) ); }	

DWORD CFileVersionInfo::GetCurTrans( void ) const
	{ return GetTransByIndex( GetCurTransIndex() ); }

LANGID CFileVersionInfo::GetCurLID( void ) const
	{ return GetLIDByIndex( GetCurTransIndex() ); }

WORD CFileVersionInfo::GetCurCP( void ) const
	{ return GetCPByIndex( GetCurTransIndex() ); }
