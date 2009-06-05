// AutoUpdate.h: interface for the CAutoUpdate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOUPDATE_H__1A24BC3C_0E79_4EA6_8C7A_F2FC5B476281__INCLUDED_)
#define AFX_AUTOUPDATE_H__1A24BC3C_0E79_4EA6_8C7A_F2FC5B476281__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VersionInfo.h"

#include <qthread.h>
#include <qmutex.h> 

class CAutoUpdate : public QThread  
{
public:
	CAutoUpdate();
	virtual ~CAutoUpdate();

    // Thread execution starts here
    virtual void run();

    void RetrieveNewVersion (bool bNewCard = false);

protected:
    void ParseVersionFile (const eidcommon::CByteArray & oData, CVersionInfo & oVersion);
    void ExecuteLink (const QString & url); 

private:
    CVersionInfo m_oVersion;
    bool m_bAlreadyAsked;
    QMutex m_cs;
};

#endif // !defined(AFX_AUTOUPDATE_H__1A24BC3C_0E79_4EA6_8C7A_F2FC5B476281__INCLUDED_)
