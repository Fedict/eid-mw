// ServerPoll.h: interface for the CServerPoll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERPOLL_H__19AF56E4_F064_419A_A350_E7D901D7CB8B__INCLUDED_)
#define AFX_SERVERPOLL_H__19AF56E4_F064_419A_A350_E7D901D7CB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qthread.h>

class CServerPoll : public QThread
{
public:
	CServerPoll();
	virtual ~CServerPoll();
	virtual void run();
    void Stop();
    bool IsStopped() { return m_bStopped; }
private:
    bool m_bRun;
    bool m_bStopped;
    QWaitCondition m_oWait;
};

#endif // !defined(AFX_SERVERPOLL_H__19AF56E4_F064_419A_A350_E7D901D7CB8B__INCLUDED_)
