// VersionInfo.h: interface for the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONINFO_H__03C9E4A2_3D65_4F70_AB13_B5B7CAEE857B__INCLUDED_)
#define AFX_VERSIONINFO_H__03C9E4A2_3D65_4F70_AB13_B5B7CAEE857B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

class CVersionInfo  
{
public:
	CVersionInfo();
	virtual ~CVersionInfo();

    void SetVersion(const QString & strVersion) { m_strVersion = strVersion; }
    const QString GetVersion() { return m_strVersion; }
    void SetDownloadUrl(const QString & strDownloadUrl) { m_strDownloadUrl = strDownloadUrl; }
    const QString GetDownloadUrl() { return m_strDownloadUrl; }

private:
    QString m_strVersion;
    QString m_strDownloadUrl;
};

#endif // !defined(AFX_VERSIONINFO_H__03C9E4A2_3D65_4F70_AB13_B5B7CAEE857B__INCLUDED_)
